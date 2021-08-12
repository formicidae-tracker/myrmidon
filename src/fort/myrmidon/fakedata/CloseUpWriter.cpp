#include "CloseUpWriter.hpp"

#include <opencv2/imgcodecs.hpp>

namespace fort {
namespace myrmidon {


CloseUpWriter::CloseUpWriter(const fs::path & tddPath,
                             bool writeFullFrame,
                             const FrameDrawer::Ptr & drawer)
	: d_drawer(drawer)
	, d_basepath(tddPath/"ants")
	, d_writeFullFrame(writeFullFrame) {
	fs::create_directories(d_basepath);
}

CloseUpWriter::~CloseUpWriter() = default;

void CloseUpWriter::Prepare(size_t index) {
	d_fullFrameNeeded = d_writeFullFrame;
	d_seen.clear();
}
void CloseUpWriter::WriteFrom(const IdentifiedFrame & data,
                              uint64_t frameID) {
	std::map<AntID,cv::Point2f> neededCloseUp;

	for ( size_t i = 0; i < data.Positions.rows(); ++i) {
		AntID antID = data.Positions(i,0);
		if ( d_seen.count(antID) == 0 ) {
			neededCloseUp[antID] = cv::Point(data.Positions(i,1),
			                                 data.Positions(i,2));
		}
	}

	if ( d_fullFrameNeeded == false && neededCloseUp.empty() ) {
		return;
	}

	d_drawer->Draw(d_frameBuffer,data);

	if(d_fullFrameNeeded == true ) {
		d_fullFrameNeeded = false;
		SaveFullFrame(d_frameBuffer,frameID);
	}

	for ( const auto & [antID,position] : neededCloseUp )  {
		d_seen.insert(antID);
		SaveCloseUp(d_frameBuffer,frameID,antID,position);
	}


}

void CloseUpWriter::Finalize(size_t index,bool last) {
}


void CloseUpWriter::SaveFullFrame(const cv::Mat & frame,
                                  uint64_t frameID)  {
	auto filename = d_basepath / ( "frame_" + std::to_string(frameID) + ".png") ;
	cv::imwrite(filename.string(),frame);
}

void CloseUpWriter::SaveCloseUp(const cv::Mat & frame,
                                uint64_t frameID,
                                AntID antID,
                                const cv::Point2f & position) {
	auto filename = d_basepath / ( "ant_"
	                               + std::to_string(antID)
	                               + "_frame_"
	                               + std::to_string(frameID)
	                               + ".png") ;

	auto roi = cv::Rect_<float>(position-cv::Point2f(150,150),cv::Size(300,300));
	roi.x = std::clamp(roi.x,0.0f,float(frame.cols-300));
	roi.y = std::clamp(roi.y,0.0f,float(frame.rows-300));

	cv::imwrite(filename.string(),frame(roi));

}


} // namespace myrmidon
} // namespace fort
