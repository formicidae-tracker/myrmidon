#include "VideoReader.hpp"

namespace fort {

namespace myrmidon {
namespace priv {

struct VideoReader::Implementation {};

VideoReader::VideoReader(const std::filesystem::path &path)
    : d_implementation{std::make_unique<Implementation>()} {}

std::tuple<int, int> VideoReader::Size() const noexcept {
	return {0, 0};
}

fort::Duration VideoReader::Duration() const noexcept {
	return 0;
}

size_t VideoReader::Length() const noexcept {
	return 0;
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
