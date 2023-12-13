#pragma once

#include <fort/time/Time.hpp>

#include <filesystem>
#include <memory>
#include <tuple>

struct image_u8;
typedef struct image_u8 image_u8_t;

namespace fort {
namespace myrmidon {
namespace priv {

class VideoReader {
public:
	VideoReader(const std::filesystem::path &path);

	~VideoReader();

	std::tuple<int, int> Size() const noexcept;

	fort::Duration Duration() const noexcept;

	size_t Length() const noexcept;

	size_t Position() const noexcept;

	void Grab(const image_u8_t &image);

	void Seek(size_t position);

	void Seek(fort::Duration duration);

private:
	struct Implementation;
	std::unique_ptr<Implementation> d_implementation;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
