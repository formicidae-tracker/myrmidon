#include "MovieWriter.hpp"

#include "Config.hpp"

#include <opencv2/videoio.hpp>
#include <fstream>

namespace fort {
namespace myrmidon {

MovieWriter::MovieWriter(const fs::path & basepath,
                         const Config & config,
                         const FrameDrawer::Ptr & drawer)
	: d_basepath(basepath)
	, d_fps(Duration::Second.Seconds() / config.Framerate.Seconds())
	, d_size(config.Width,config.Height)
	, d_drawer(drawer) {
}

MovieWriter::~MovieWriter() {
}

std::string MovieWriter::NumberSuffix(size_t i) {
	std::ostringstream oss;
	oss << std::setw(4) << std::setfill('0') << i;
	return oss.str();
}

void MovieWriter::Prepare(size_t index) {

	auto moviePath = d_basepath / ("stream" + NumberSuffix(index) + ".mp4");
	auto matchPath = d_basepath / ("stream.frame-matching" + NumberSuffix(index) + ".txt");

	d_videoWriter = std::make_unique<cv::VideoWriter>(moviePath.string(),
	                                                  cv::VideoWriter::fourcc('H','2','6','4'),
	                                                  d_fps,
	                                                  d_size,
	                                                  false);
	d_frameMatching = std::make_unique<std::ofstream>(matchPath.c_str());

	d_movieFrame = 0;
}

void MovieWriter::WriteFrom(const IdentifiedFrame & data,
                            uint64_t frameID) {
	d_drawer->Draw(d_frameBuffer,data);
	*d_videoWriter << d_frameBuffer;
	*d_frameMatching << d_movieFrame++ << " " << frameID << std::endl;
}

void MovieWriter::Finalize(size_t index,bool last) {
	d_videoWriter.reset();
	d_frameMatching.reset();
}




} // namespace myrmidon
} // namespace fort
