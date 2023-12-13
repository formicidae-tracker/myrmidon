#include "VideoReader.hpp"

#include <cpptrace/cpptrace.hpp>
#include <fort/time/Time.hpp>
#include <fort/utils/Once.hpp>
#include <memory>
#include <utility>

#include <cxxabi.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include <iostream>

namespace fort {

namespace myrmidon {
namespace priv {

using AVFormatContextPtr =
    std::unique_ptr<AVFormatContext, void (*)(AVFormatContext *)>;

using AVCodecContextPtr =
    std::unique_ptr<AVCodecContext, void (*)(AVCodecContext *)>;

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

struct VideoReader::Implementation {
	Implementation(const std::filesystem::path &path)
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
	}

	AVStream *Stream() const noexcept {
		return Context->streams[Index];
	}

	AVFormatContextPtr Context = {nullptr, nullptr};
	int                Index   = -1;
	AVCodecContextPtr  Codec   = {nullptr, nullptr};

private:
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
};

VideoReader::VideoReader(const std::filesystem::path &path)
    : d_implementation{std::make_unique<Implementation>(path)} {}

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

void VideoReader::Grab(const image_u8_t &image) {}

void VideoReader::Seek(size_t position) {}

void VideoReader::Seek(fort::Duration duration) {}

} // namespace priv
} // namespace myrmidon
} // namespace fort
