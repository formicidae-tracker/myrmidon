#pragma once

#include <fort/myrmidon/utils/FileSystem.hpp>

#include "FrameDrawer.hpp"
#include "SegmentedDataWriter.hpp"

#include <fort/video/Frame.hpp>
#include <tuple>

namespace fort {
namespace video {
class Writer;
}

namespace myrmidon {

class Config;

class MovieWriter : public SegmentedDataWriter {
public:
	MovieWriter(
	    const fs::path         &basepath,
	    const Config           &config,
	    const FrameDrawer::Ptr &drawer
	);
	virtual ~MovieWriter();

	void Prepare(size_t index) override;
	void WriteFrom(const IdentifiedFrame &data, uint64_t frameID) override;
	void Finalize(size_t index, bool last) override;

private:
	static std::string NumberSuffix(size_t i);

	fs::path d_basepath;

	FrameDrawer::Ptr     d_drawer;
	video::Frame         d_frameBuffer;
	video::Ratio<int>    d_framerate;
	std::tuple<int, int> d_size;
	int                  d_movieFrame;

	std::unique_ptr<video::Writer> d_videoWriter;
	std::unique_ptr<std::ofstream> d_frameMatching;
};

} // namespace myrmidon
} // namespace fort
