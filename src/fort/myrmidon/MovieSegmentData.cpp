#include "MovieSegmentData.hpp"

#include <opencv2/videoio.hpp>

namespace fort {
namespace myrmidon {

void MovieSegmentData::ForEachFrames(const List & list,
                                     std::function<void (cv::Mat & frame, const MovieFrameData & data)> operation) {
	for ( const auto & s : list) {
		cv::VideoCapture cap(s.AbsoluteFilePath);
		cap.set(cv::CAP_PROP_POS_FRAMES,s.Begin);
		cv::Mat frame;
		auto iter = s.Data.cbegin();
		for( auto moviePos = s.Begin; moviePos < s.End; ++moviePos) {
			cap >> frame;
			if ( frame.empty() ) {
				break;
			}
			while(iter != s.Data.end() && iter->FramePosition < moviePos) {
				++iter;
			}
			if ( iter != s.Data.end() && iter->FramePosition == moviePos ) {
				operation(frame,*iter);
			} else {
				operation(frame,{.FramePosition = moviePos,.Time = Time::SinceEver()});
			}
		}
	}
}


} // namespace myrmidon
} // namespace fort
