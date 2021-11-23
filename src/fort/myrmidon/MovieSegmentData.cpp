#include "MovieSegmentData.hpp"

#include <opencv2/videoio.hpp>

namespace fort {
namespace myrmidon {

void MovieSegmentData::IterateOverFrames(const List & list,
                                         std::function<void (cv::Mat & frame, const MatchedData & data)> operation) {
	for ( const auto & s : list) {
		cv::VideoCapture cap(s.AbsoluteFilePath);
		cap.set(cv::CV_CAP_FRAME_POS,s.Start);
		cv::Mat frame;
		const auto iter = s.Data.begin();
		for( auto moviePos = s.Begin; moviePos < s.End; ++moviePos) {
			cap >> frame;
			if ( frame.empty() ) {
				break;
			}
			while(iter != s.Data.end() && iter->FramePosition < moviePos) {
				++iter;
			}
			if ( iter->FramePosition == moviePos ) {
				operation(frame,*iter);
			} else {
				operation(frame,MatchedData{.FramePosition = moviePos,.Time = Time::SinceEver()});
			}
		}
	}
}
