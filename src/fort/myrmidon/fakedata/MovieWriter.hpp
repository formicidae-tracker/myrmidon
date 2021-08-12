#pragma once

#include <opencv2/core.hpp>

#include <fort/myrmidon/utils/FileSystem.hpp>

#include "SegmentedDataWriter.hpp"
#include "FrameDrawer.hpp"



namespace cv {
class VideoWriter;
}


namespace fort {
namespace myrmidon {


class Config;

class MovieWriter : public SegmentedDataWriter {
public:
	MovieWriter(const fs::path & basepath,
	            const Config & config,
	            const FrameDrawer::Ptr & drawer);
	virtual ~MovieWriter();

	void Prepare(size_t index) override;
	void WriteFrom(const IdentifiedFrame & data,
	               uint64_t frameID) override;
	void Finalize(size_t index,bool last) override;
private:
	static std::string NumberSuffix(size_t i);

	fs::path d_basepath;

	FrameDrawer::Ptr d_drawer;
	cv::Mat d_frameBuffer;
	double d_fps;
	cv::Size d_size;
	int d_movieFrame;

	std::unique_ptr<cv::VideoWriter> d_videoWriter;
	std::unique_ptr<std::ofstream>   d_frameMatching;
};


} // namespace myrmidon
} // namespace fort
