#pragma once

#include "fort/myrmidon/priv/PNGUtils.hpp"
#include <fort/time/Time.hpp>

#include <filesystem>
#include <functional>
#include <memory>
#include <tuple>

extern "C" {
struct image_u8;
typedef struct image_u8 image_u8_t;
}

namespace fort {
namespace myrmidon {
namespace priv {

class VideoFrame {
public:
	using Ptr = std::unique_ptr<VideoFrame>;

	image_u8_t AsImageU8() const noexcept;

	size_t Index() const noexcept;

	Duration Timestamp() const noexcept;

private:
	struct Implementation;

	friend class VideoReader;

	std::unique_ptr<Implementation, std::function<void(Implementation *)>>
	    d_implementation;
};

class VideoReader {
public:
	VideoReader(
	    const std::filesystem::path &path,
	    std::tuple<int, int>         targetSize = {-1, -1}
	);

	~VideoReader();

	std::tuple<int, int> Size() const noexcept;

	fort::Duration Duration() const noexcept;

	size_t Length() const noexcept;

	size_t Position() const noexcept;

	void Seek(size_t position);

	void Seek(fort::Duration duration);

	VideoFrame::Ptr Grab();

private:
	struct Implementation;

	std::unique_ptr<Implementation> d_implementation;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
