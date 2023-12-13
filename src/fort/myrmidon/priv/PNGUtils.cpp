#include "PNGUtils.hpp"

#include <cstdint>
#include <cstdio>
#include <cxxabi.h>

#include <fstream>
#include <spng.h>

#include <cpptrace/cpptrace.hpp>

#include <fort/utils/Defer.hpp>
#include <sstream>
#include <stdexcept>
#include <system_error>

namespace fort {
namespace myrmidon {
namespace priv {

class SPNGError : public cpptrace::runtime_error {
public:
	template <typename Function>
	SPNGError(int error, Function && f)
		: cpptrace::runtime_error{
		    abi::__cxa_demangle(typeid(Function).name(), NULL, NULL, NULL) +
		    std::string(" error (") + std::to_string(error) +
		    "): " + spng_strerror(error)}
		, d_code{error} {
	}

	int Code() const noexcept {
		return d_code;
	}

private:
	int d_code;
};

template <typename Function, typename... Args>
void SPNGCall(Function &&fn, Args &&...args) {
	int err = std::forward<Function>(fn)(std::forward<Args>(args)...);
	if (err != 0) {
		throw SPNGError(err, std::forward<Function>(fn));
	}
}

ImageU8Ptr ReadPNG(const std::filesystem::path &path) {
	FILE *file = std::fopen(path.string().c_str(), "rb");
	if (file == nullptr) {
		std::ostringstream oss;
		oss << "open(" << path << ", \"rb\")";
		throw std::system_error{errno, std::generic_category(), oss.str()};
	}
	auto ctx = spng_ctx_new(0); // create a decoder context
	defer {
		spng_ctx_free(ctx);
		fclose(file);
	};

	SPNGCall(spng_set_png_file, ctx, file);

	struct spng_ihdr ihdr;
	SPNGCall(spng_get_ihdr, ctx, &ihdr);

	size_t decodedSize;
	SPNGCall(spng_decoded_image_size, ctx, SPNG_FMT_G8, &decodedSize);

	constexpr static size_t MAX_IMAGE_SIZE = 8192 * 8192;

	if (decodedSize > MAX_IMAGE_SIZE) {

		std::ostringstream oss;
		oss << "PNG file " << path << " is too large: decoded size "
		    << decodedSize << " is larger than " << MAX_IMAGE_SIZE
		    << " declared width: " << ihdr.width
		    << " declared height: " << ihdr.height;
		throw cpptrace::runtime_error(oss.str());
	}

	ImageU8Ptr res{image_u8_create(ihdr.width, ihdr.height), &image_u8_destroy};

	SPNGCall(
	    spng_decode_image,
	    ctx,
	    nullptr,
	    0,
	    SPNG_FMT_G8,
	    SPNG_DECODE_PROGRESSIVE
	);

	int                  error;
	struct spng_row_info rowInfo;

	try {
		while (true) {
			SPNGCall(spng_get_row_info, ctx, &rowInfo);

			// note that image_u8 is not necessarly packed, use stride instead
			// of width.
			uint8_t *rowAddr = res->buf + res->stride * rowInfo.row_num;

			SPNGCall(spng_decode_row, ctx, rowAddr, res->stride);
		}
	} catch (const SPNGError &e) {
		if (e.Code() != SPNG_EOI) {
			throw;
		}
	}

	return res;
}

void WritePNG(const std::filesystem::path &path, const image_u8_t &image) {
	auto file = fopen(path.string().c_str(), "wb");
	if (file == nullptr) {
		std::ostringstream oss;
		oss << "fopen(" << path << ", \"wb\") error";
		throw std::system_error{errno, std::generic_category(), oss.str()};
	}
	auto ctx = spng_ctx_new(SPNG_CTX_ENCODER);
	defer {
		spng_ctx_free(ctx);
		fclose(file);
	};

	SPNGCall(spng_set_png_file, ctx, file);

	struct spng_ihdr ihdr {
		.width = uint32_t(image.width), .height = uint32_t(image.height),
		.bit_depth = 8, .color_type = SPNG_COLOR_TYPE_GRAYSCALE,
		.filter_method    = SPNG_FILTER_NONE,
		.interlace_method = SPNG_INTERLACE_NONE,
	};

	SPNGCall(spng_set_ihdr, ctx, &ihdr);

	SPNGCall(
	    spng_encode_image,
	    ctx,
	    nullptr,
	    0,
	    SPNG_FMT_PNG,
	    SPNG_ENCODE_FINALIZE | SPNG_ENCODE_PROGRESSIVE
	);

	try {
		struct spng_row_info rowInfo;
		while (true) {
			SPNGCall(spng_get_row_info, ctx, &rowInfo);

			uint8_t *addr = image.buf + image.stride * rowInfo.row_num;

			SPNGCall(spng_encode_row, ctx, addr, image.width);
		}
	} catch (const SPNGError &e) {
		if (e.Code() != SPNG_EOI) {
			throw;
		}
	}
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
