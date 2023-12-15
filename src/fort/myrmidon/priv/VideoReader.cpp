#include "VideoReader.hpp"

#include <cpptrace/cpptrace.hpp>
#include <fort/time/Time.hpp>
#include <fort/utils/Defer.hpp>
#include <fort/utils/ObjectPool.hpp>
#include <memory>
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

struct VideoFrame::ImageData {
	uint8_t *Planes[4];
	int      Strides[4];
};

struct VideoReader::Implementation {
	Implementation(
	    const std::filesystem::path &path, std::tuple<int, int> targetSize
	)
	    : Context{open(path), [](AVFormatContext *c) {
		              if (c) {
			              avformat_close_input(&c);
		              }
	              }} {
		// Needed for mpeg2 formats
		AVCall(avformat_find_stream_info, Context.get(), nullptr);

		Index = AVCall(
		    av_find_best_stream,
		    Context.get(),
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
		Codec = {decCtx, [](AVCodecContext *ctx) {
			         if (ctx) {
				         avcodec_free_context(&ctx);
			         }
		         }};

		AVCall(avcodec_parameters_to_context, decCtx, Stream()->codecpar);
		AVCall(avcodec_open2, decCtx, dec, nullptr);

		auto [outputWidth, outputHeight] = targetSize;
		if (outputWidth <= 0 || outputHeight <= 0) {
			outputWidth  = Codec->width;
			outputHeight = Codec->height;
		}

		ImagePool = ImageDataPool::Create(
		    [w = outputWidth, h = outputHeight]() {
			    auto res = new VideoFrame::ImageData{};
			    AVCall(
			        av_image_alloc,
			        res->Planes,
			        res->Strides,
			        w,
			        h,
			        AV_PIX_FMT_GRAY8,
			        16
			    );
			    return res;
		    },
		    [](VideoFrame::ImageData *data) {
			    av_freep(data->Planes);
			    delete data;
		    }
		);

		if (Codec->width != outputWidth || Codec->height != outputHeight ||
		    Codec->pix_fmt != AV_PIX_FMT_GRAY8) {
			ScaleContext = {
			    sws_getContext(
			        Codec->width,
			        Codec->height,
			        Codec->pix_fmt,
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

	VideoFrame::Ptr receiveFrame() {
		AVCall(avcodec_receive_frame, Codec.get(), Frame.get());
		auto res = ImagePool->Get();
		if (!ScaleContext) {
			av_image_copy(
			    res->Planes,
			    res->Strides,
			    const_cast<const uint8_t **>(Frame->data),
			    Frame->linesize,
			    Codec->pix_fmt,
			    Codec->width,
			    Codec->height
			);
		} else {
			AVCall(
			    sws_scale,
			    ScaleContext.get(),
			    Frame->data,
			    Frame->linesize,
			    0,
			    Codec->height,
			    res->Planes,
			    res->Strides
			);
		}
		auto frame         = new VideoFrame{};
		frame->d_imageData = std::move(res);
		return VideoFrame::Ptr{frame};
	}

	AVStream *Stream() const noexcept {
		return Context->streams[Index];
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

	using ImageDataPool = utils::ObjectPool<
	    VideoFrame::ImageData,
	    std::function<VideoFrame::ImageData *()>,
	    std::function<void(VideoFrame::ImageData *)>>;

	using AVFormatContextPtr =
	    std::unique_ptr<AVFormatContext, void (*)(AVFormatContext *)>;

	using AVCodecContextPtr =
	    std::unique_ptr<AVCodecContext, void (*)(AVCodecContext *)>;

	using AVPacketPtr = std::unique_ptr<AVPacket, void (*)(AVPacket *)>;

	using AVFramePtr = std::unique_ptr<AVFrame, void (*)(AVFrame *)>;

	using SwsContextPtr = std::unique_ptr<SwsContext, void (*)(SwsContext *)>;

	AVFormatContextPtr Context = {nullptr, nullptr};
	int                Index   = -1;
	AVCodecContextPtr  Codec   = {nullptr, nullptr};
	AVPacketPtr        Packet  = {
	            av_packet_alloc(),
	            [](AVPacket *pkt) { av_packet_free(&pkt); },
    };
	SwsContextPtr ScaleContext = {nullptr, nullptr};
	AVFramePtr    Frame        = {
	              av_frame_alloc(),
	              [](AVFrame *frame) { av_frame_free(&frame); },
    };
	ImageDataPool::Ptr ImagePool;
};

VideoReader::VideoReader(
    const std::filesystem::path &path, std::tuple<int, int> targetSize
)
    : d_implementation{std::make_unique<Implementation>(path, targetSize)} {}

VideoReader::~VideoReader() {}

std::tuple<int, int> VideoReader::Size() const noexcept {
	const auto codecpar = d_implementation->Stream()->codecpar;
	return {codecpar->width, codecpar->height};
}

fort::Duration VideoReader::Duration() const noexcept {
	return (d_implementation->Context->duration) *
	       fort::Duration{1000000000 / AV_TIME_BASE};
}

size_t VideoReader::Length() const noexcept {
	return d_implementation->Stream()->nb_frames;
}

size_t VideoReader::Position() const noexcept {
	return 0;
}

VideoFrame::Ptr VideoReader::Grab() {

	if (d_implementation->Packet->buf == nullptr) {
		AVCall(
		    av_read_frame,
		    d_implementation->Context.get(),
		    d_implementation->Packet.get()
		);
	}

	try {
		AVCall(
		    avcodec_send_packet,
		    d_implementation->Codec.get(),
		    d_implementation->Packet.get()
		);

	} catch (const AVError &e) {
		if (e.Code() != AVERROR(EAGAIN)) {
			av_packet_unref(d_implementation->Packet.get());
			throw;
		}
	}
	defer {
		av_packet_unref(d_implementation->Packet.get());
	};
	try {
		auto res = d_implementation->receiveFrame();
		return res;

	} catch (const AVError &e) {
		if (e.Code() == AVERROR(EAGAIN)) {
			av_packet_unref(d_implementation->Packet.get());
			return Grab();
		}
		throw;
	}
}

    void VideoReader::Seek(size_t position) {}

	void VideoReader::Seek(fort::Duration duration) {}

} // namespace priv
} // namespace myrmidon
} // namespace fort
