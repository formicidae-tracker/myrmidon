#pragma once

#include <apriltag/common/image_u8.h>
#include <filesystem>
#include <memory>

namespace fort {
namespace myrmidon {
namespace priv {

// Using a stateless lambda to have unique pointer the same size than raw
// pointers.

using ImageU8Ptr = std::unique_ptr<image_u8_t, void (*)(image_u8_t *)>;

ImageU8Ptr ReadPNG(const std::filesystem::path &path);

void WritePNG(const std::filesystem::path &path, const image_u8_t &image);

} // namespace priv
} // namespace myrmidon
} // namespace fort
