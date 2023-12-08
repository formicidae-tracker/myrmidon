#include "PNGUtils.hpp"

#include <spng.h>

namespace fort {
namespace myrmidon {
namespace priv {

image_u8_t ReadPNG(const std::filesystem::path &path) {
	auto ctx = spng_ctx_new(0);
}

void WritePNG(const std::filesystem::path &path, const image_u8_t &image);

} // namespace priv
} // namespace myrmidon
} // namespace fort
