#include "VideoReader.hpp"
#include "concurrentqueue.h"

#include <cpptrace/cpptrace.hpp>
#include <cstdint>
#include <fort/time/Time.hpp>
#include <fort/utils/Defer.hpp>
#include <fort/utils/ObjectPool.hpp>
#include <limits>
#include <memory>
#include <queue>
#include <utility>

#include <cxxabi.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <iostream>

namespace fort {

namespace myrmidon {
namespace priv {

class AVError : public cpptrace::runtime_error {
public:
	template <typename Function>
	AVError(int error, Function &&fn)
	    : cpptrace::runtime_error{Reason(error, std::forward<Function>(fn))}
	    , d_error{error} {}

	template <typename Function>
	static std::string Reason(int error, Function &&fn) {
		char buffer[1024];
		av_strerror(error, buffer, 1024);
		return std::string(abi::__cxa_demangle(
		           typeid(Function).name(),
		           NULL,
		           NULL,
		           NULL
		       )) +
		       " error (" + std::to_string(error) + "): " + buffer;
	}

	int Code() const {
		return d_error;
	}

private:
	int d_error;
};

template <typename Function, typename... Args>
int AVCall(Function &&fn, Args &&...args) {

	int res = std::forward<Function>(fn)(std::forward<Args>(args)...);
	if (res < 0) {
		throw AVError(res, std::forward<Function>(fn));
	}
	return res;
}

struct VideoFrame::Implementation {
	uint8_t *Planes[4];
	int      Strides[4];
	int      Width, Height;
	size_t   Index;
	Duration Timestamp;
};

size_t VideoFrame::Index() const noexcept {
	return self->Index;
}

Duration VideoFrame::Timestamp() const noexcept {
	return self->Timestamp;
}

image_u8_t VideoFrame::AsImageU8() const noexcept {
	return image_u8_t{
	    .width  = self->Width,
	    .height = self->Height,
	    .stride = self->Strides[0],
	    .buf    = self->Planes[0],
	};
}

template <typename Frame> struct FrameOrderer {
	constexpr bool operator()(const Frame &a, const Frame &b) const {
		return a->Index > b->Index;
	};
};

struct VideoReader::Implementation {
	using ImageDataPool = utils::ObjectPool<
	    VideoFrame::Implementation,
	    std::function<VideoFrame::Implementation *()>,
	    std::function<void(VideoFrame::Implementation *)>>;

	using AVFormatContextPtr =
	    std::unique_ptr<AVFormatContext, void (*)(AVFormatContext *)>;

	using AVCodecContextPtr =
	    std::unique_ptr<AVCodecContext, void (*)(AVCodecContext *)>;

	using AVPacketPtr = std::unique_ptr<AVPacket, void (*)(AVPacket *)>;

	using AVFramePtr = std::unique_ptr<AVFrame, void (*)(AVFrame *)>;

	using SwsContextPtr = std::unique_ptr<SwsContext, void (*)(SwsContext *)>;

	using ImageDataQueue = std::priority_queue<
	    ImageDataPool::ObjectPtr,
	    std::vector<ImageDataPool::ObjectPtr>,
	    FrameOrderer<ImageDataPool::ObjectPtr>>;

	AVFormatContextPtr d_context = {nullptr, nullptr};
	int                d_index   = -1;
	AVCodecContextPtr  d_codec   = {nullptr, nullptr};
	AVPacketPtr        d_packet  = {
	            av_packet_alloc(),
	            [](AVPacket *pkt) { av_packet_free(&pkt); },
    };
	SwsContextPtr d_scaleContext = {nullptr, nullptr};
	AVFramePtr    d_frame        = {
	              av_frame_alloc(),
	              [](AVFrame *frame) { av_frame_free(&frame); },
    };
	ImageDataPool::Ptr d_imagePool;
	ImageDataQueue     d_queue;
	size_t             d_next = 0;
	fort::Duration     d_timebase;
	int64_t            d_framerate;

	Implementation(
	    const std::filesystem::path &path, std::tuple<int, int> targetSize
	)
	    : d_context{open(path), [](AVFormatContext *c) {
		                if (c) {
			                avformat_close_input(&c);
		                }
	                }} {
		// Needed for mpeg2 formats
		AVCall(avformat_find_stream_info, d_context.get(), nullptr);

		d_index = AVCall(
		    av_find_best_stream,
		    d_context.get(),
		    AVMEDIA_TYPE_VIDEO,
		    -1,
		    -1,
		    nullptr,
		    0
		);

		auto dec = avcodec_find_decoder(Stream()->codecpar->codec_id);
		if (dec == nullptr) {
			throw cpptrace::runtime_error{
			    "could not find video codec " +
			        std::to_string(Stream()->codecpar->codec_id),
			};
		}

		auto decCtx = avcodec_alloc_context3(dec);
		if (decCtx == nullptr) {
			throw cpptrace::runtime_error{
			    "could not allocate  context for codec " +
			    std::string(dec->long_name)};
		}
		d_codec = {decCtx, [](AVCodecContext *ctx) {
			           if (ctx) {
				           avcodec_free_context(&ctx);
			           }
		           }};

		AVCall(avcodec_parameters_to_context, decCtx, Stream()->codecpar);
		AVCall(avcodec_open2, decCtx, dec, nullptr);

		d_timebase =
		    (Duration::Second * Stream()->time_base.num).Nanoseconds() /
		    (Stream()->time_base.den);

		auto [outputWidth, outputHeight] = targetSize;
		if (outputWidth <= 0 || outputHeight <= 0) {
			outputWidth  = d_codec->width;
			outputHeight = d_codec->height;
		}

		d_imagePool = ImageDataPool::Create(
		    [w = outputWidth, h = outputHeight]() {
			    auto res = new VideoFrame::Implementation{};
			    AVCall(
			        av_image_alloc,
			        res->Planes,
			        res->Strides,
			        w,
			        h,
			        AV_PIX_FMT_GRAY8,
			        16
			    );
			    res->Width  = w;
			    res->Height = h;
			    return res;
		    },
		    [](VideoFrame::Implementation *data) {
			    av_freep(data->Planes);
			    delete data;
		    }
		);

		if (d_codec->width != outputWidth || d_codec->height != outputHeight ||
		    d_codec->pix_fmt != AV_PIX_FMT_GRAY8) {
			d_scaleContext = {
			    sws_getContext(
			        d_codec->width,
			        d_codec->height,
			        d_codec->pix_fmt,
			        outputWidth,
			        outputHeight,
			        AV_PIX_FMT_GRAY8,
			        SWS_BILINEAR,
			        nullptr,
			        nullptr,
			        nullptr
			    ),
			    sws_freeContext,
			};
		}
	}

	VideoFrame::Ptr Grab() {
		auto res = std::make_unique<VideoFrame>();

		res->self = grab();
		if (!res->self) {
			return nullptr;
		}
		return res;
	}

	ImageDataPool::ObjectPtr grab(bool checkIFrame = false) {
		ImageDataPool::ObjectPtr res = nullptr;
		if (!d_queue.empty()) {
			res = std::move(const_cast<ImageDataPool::ObjectPtr &>(d_queue.top()
			));
			d_queue.pop();
			return res;
		}
		if (!d_packet) {
			return nullptr;
		}
		try {
			AVCall(av_read_frame, d_context.get(), d_packet.get());
		} catch (const AVError &e) {
			if (e.Code() != AVERROR_EOF) {
				throw;
			} else {
				d_packet.reset();
			}
		}
		defer {
			if (d_packet) {
				av_packet_unref(d_packet.get());
			}
		};

		AVCall(avcodec_send_packet, d_codec.get(), d_packet.get());
		while (true) {
			try {
				AVCall(avcodec_receive_frame, d_codec.get(), d_frame.get());
			} catch (const AVError &e) {
				if (e.Code() == AVERROR(EAGAIN) || e.Code() == AVERROR_EOF) {
					break;
				}
				throw;
			}

			defer {
				av_frame_unref(d_frame.get());
			};

			if (checkIFrame && d_frame->pict_type != AV_PICTURE_TYPE_I) {
				throw cpptrace::runtime_error(
				    std::string("Only I-Frame requested, but received a ") +
				    av_get_picture_type_char(d_frame->pict_type)
				);
			}

			auto newFrame = d_imagePool->Get();

			if (d_scaleContext) {
				AVCall(
				    sws_scale,
				    d_scaleContext.get(),
				    d_frame->data,
				    d_frame->linesize,
				    0,
				    d_codec->height,
				    newFrame->Planes,
				    newFrame->Strides
				);
			} else {
				av_image_copy(
				    newFrame->Planes,
				    newFrame->Strides,
				    const_cast<const uint8_t **>(d_frame->data),
				    d_frame->linesize,
				    AV_PIX_FMT_GRAY8,
				    d_codec->width,
				    d_codec->height
				);
			}

			newFrame->Timestamp = av_rescale_q(
			    d_frame->pts,
			    Stream()->time_base,
			    {1, int64_t(1e9)}
			);
			newFrame->Index = av_rescale_q(
			    d_frame->pts,
			    Stream()->time_base,
			    {Stream()->avg_frame_rate.den, Stream()->avg_frame_rate.num}
			);

			d_queue.push(std::move(newFrame));
		}

		if (!d_queue.empty()) {
			res = std::move(const_cast<ImageDataPool::ObjectPtr &>(d_queue.top()
			));
			d_queue.pop();
			return res;
		}

		return grab();
	}

	AVStream *Stream() const noexcept {
		return d_context->streams[d_index];
	}

	AVFormatContext *open(const std::filesystem::path &path) {
		AVFormatContext *ctx = nullptr;
		AVCall(
		    avformat_open_input,
		    &ctx,
		    path.string().c_str(),
		    nullptr,
		    nullptr
		);
		return ctx;
	}

	void seekFrame(size_t position) {
		AVCall(
		    avformat_seek_file,
		    d_context.get(),
		    d_index,
		    0, // std::numeric_limits<int64_t>::min(),
		    position,
		    position,
		    AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD
		);

		avcodec_flush_buffers(d_codec.get());
		ImageDataPool::ObjectPtr frame;

		bool checkIFrame = true;
		do {
			frame = grab(checkIFrame);

			checkIFrame = false;
		} while (frame && frame->Index < position);
		while (!d_queue.empty()) {
			d_queue.pop();
		}
		if (frame) {
			d_queue.push(std::move(frame));
		}
	}
};

VideoReader::VideoReader(
    const std::filesystem::path &path, std::tuple<int, int> targetSize
)
    : self{std::make_unique<Implementation>(path, targetSize)} {}

VideoReader::~VideoReader() {}

std::tuple<int, int> VideoReader::Size() const noexcept {
	const auto codecpar = self->Stream()->codecpar;
	return {codecpar->width, codecpar->height};
}

fort::Duration VideoReader::Duration() const noexcept {
	return (self->d_context->duration) *
	       fort::Duration{1000000000 / AV_TIME_BASE};
}

size_t VideoReader::Length() const noexcept {
	return self->Stream()->nb_frames;
}

size_t VideoReader::Position() const noexcept {
	return 0;
}

VideoFrame::Ptr VideoReader::Grab() {
	return self->Grab();
}

void VideoReader::SeekFrame(size_t position) {
	self->seekFrame(position);
}

void VideoReader::SeekTime(fort::Duration duration) {}

} // namespace priv
} // namespace myrmidon
} // namespace fort
