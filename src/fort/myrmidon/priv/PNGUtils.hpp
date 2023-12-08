#pragma once

#include <apriltag/common/image_u8.h>
#include <filesystem>

namespace fort {
namespace myrmidon {
namespace priv {

image_u8_t ReadPNG(const std::filesystem::path &path);

void WritePNG(const std::filesystem::path &path, const image_u8_t &image);

} // namespace priv
} // namespace myrmidon
} // namespace fort
