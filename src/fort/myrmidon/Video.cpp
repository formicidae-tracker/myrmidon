#include "Video.hpp"

namespace fort {
namespace myrmidon {

void VideoSequence::ForEach(
    const VideoSegment::List                       &list,
    std::function<void(const VideoFrameData &data)> operation
) {
	for (const auto &s : list) {
		throw std::logic_error("Not yet implemented");
		// cv::VideoCapture cap(s.AbsoluteFilePath);
		// cap.set(cv::CAP_PROP_POS_FRAMES, s.Begin);
		// cv::Mat frame;
		// auto    iter = s.Data.cbegin();
		// for (auto moviePos = s.Begin; moviePos < s.End; ++moviePos) {
		// 	cap >> frame;
		// 	if (frame.empty()) {
		// 		break;
		// 	}
		// 	while (iter != s.Data.end() && iter->Position < moviePos) {
		// 		++iter;
		// 	}
		// 	if (iter != s.Data.end() && iter->Position == moviePos) {
		// 		operation(frame, *iter);
		// 	} else {
		// 		operation(
		// 		    frame,
		// 		    {.Position = moviePos, .Time = Time::SinceEver()}
		// 		);
	}
}

} // namespace myrmidon
} // namespace fort
