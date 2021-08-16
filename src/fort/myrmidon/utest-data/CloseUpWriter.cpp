#include "CloseUpWriter.hpp"

#include <opencv2/imgcodecs.hpp>
#include <fort/myrmidon/priv/TagCloseUp.hpp>
namespace fort {
namespace myrmidon {


CloseUpWriter::CloseUpWriter(UTestData::TDDInfo & tddInfo,
                             const FrameDrawer::Ptr & drawer)
	: d_tddInfo(tddInfo)
	, d_drawer(drawer) {
	fs::create_directories(d_tddInfo.AbsoluteFilePath / "ants");
}

CloseUpWriter::~CloseUpWriter() = default;

void CloseUpWriter::Prepare(size_t index) {
	d_fullFrameNeeded = d_tddInfo.HasFullFrame;
	d_seen.clear();
}
void CloseUpWriter::WriteFrom(const IdentifiedFrame & data,
                              uint64_t frameID) {
	std::map<AntID,cv::Point2f> neededCloseUp;

	for ( size_t i = 0; i < data.Positions.rows(); ++i) {
		AntID antID = data.Positions(i,0);
		if ( d_seen.count(antID) == 0 ) {
			Eigen::Vector2d position;
			double angle;
			d_drawer->ComputeTagPosition(position,
			                             angle,
			                             antID,
			                             data.Positions.block<1,3>(i,1).transpose());

			neededCloseUp[antID] = cv::Point(position.x(),
			                                 position.y());
		}
	}

	if ( d_fullFrameNeeded == false && neededCloseUp.empty() ) {
		return;
	}

	d_drawer->Draw(d_frameBuffer,data);

	if(d_fullFrameNeeded == true ) {
		d_fullFrameNeeded = false;
		SaveFullFrame(d_frameBuffer,frameID);
		SaveExpectedFullFrame(data,frameID);
	}

	for ( const auto & [antID,position] : neededCloseUp )  {
		d_seen.insert(antID);
		SaveCloseUp(d_frameBuffer,frameID,antID,position);
		SaveExpectedCloseUpFrame(data,frameID,antID);
	}


}

void CloseUpWriter::Finalize(size_t index,bool last) {
	std::sort(d_tddInfo.TagCloseUps.begin(),
	          d_tddInfo.TagCloseUps.end(),
	          [](const priv::TagCloseUp::ConstPtr & a,
	             const priv::TagCloseUp::ConstPtr & b) {
		          if ( a->AbsoluteFilePath() == b->AbsoluteFilePath() ) {
			          return a->TagValue() < b->TagValue();
		          }
		          return a->AbsoluteFilePath() < b->AbsoluteFilePath();
	          });

}

std::string CloseUpWriter::FullFramePath(uint64_t frameID) const {
	return d_tddInfo.AbsoluteFilePath / "ants" / ( "frame_" + std::to_string(frameID) + ".png") ;
}

std::string CloseUpWriter::CloseUpPath(uint64_t frameID, AntID antID) const {
	return d_tddInfo.AbsoluteFilePath
		/ "ants"
		/ ( "ant_"
		    + std::to_string(antID-1)
		    + "_frame_"
		    + std::to_string(frameID)
		    + ".png") ;
}


void CloseUpWriter::SaveFullFrame(const cv::Mat & frame,
                                  uint64_t frameID)  {
	cv::imwrite(FullFramePath(frameID),frame);
}

void CloseUpWriter::SaveCloseUp(const cv::Mat & frame,
                                uint64_t frameID,
                                AntID antID,
                                const cv::Point2f & position) {

	auto roi = cv::Rect_<float>(position-cv::Point2f(150,150),cv::Size(300,300));
	roi.x = std::clamp(std::round(roi.x),0.0f,float(frame.cols-300));
	roi.y = std::clamp(std::round(roi.y),0.0f,float(frame.rows-300));

	cv::imwrite(CloseUpPath(frameID,antID),frame(roi));
}

void CloseUpWriter::SaveExpectedFullFrame(const IdentifiedFrame & data,
                                          uint64_t frameID) {
	auto path = FullFramePath(frameID);
	d_tddInfo.TagCloseUpFiles.insert({frameID,{path,
	                                           nullptr}});
	for ( size_t i = 0; i < data.Positions.rows(); ++i) {
		SaveExpectedCloseUp(path,
		                    data,
		                    frameID,
		                    data.Positions(i,0),
		                    true);
	}

}

void CloseUpWriter::SaveExpectedCloseUpFrame(const IdentifiedFrame & data,
                                             uint64_t frameID,
                                             AntID antID) {
	auto path = CloseUpPath(frameID,antID);
	d_tddInfo.TagCloseUpFiles.insert({frameID,{path,
	                                           std::make_shared<AntID>(antID-1)}});
	SaveExpectedCloseUp(path,
	                    data,
	                    frameID,
	                    antID,
	                    false);
}


void CloseUpWriter::SaveExpectedCloseUp(const fs::path & path,
                                        const IdentifiedFrame & data,
                                        uint64_t frameID,
                                        AntID antID,
                                        bool fullFrame) {
	int index = -1;
	for ( size_t i = 0; i < data.Positions.rows(); ++i ) {
		if ( data.Positions(i,0) == antID ) {
			index = i;
			break;
		}
	}

	if ( index < 0 ) {
		throw std::runtime_error("could not find ant " + std::to_string(antID));
	}

	Eigen::Vector2d position;
	double angle;
	Vector2dList corners;

	auto antPosition = data.Positions.block<1,3>(index,1).transpose();
	d_drawer->ComputeCorners(corners,
	                         antID,
	                         antPosition);
	d_drawer->ComputeTagPosition(position,
	                             angle,
	                             antID,
	                             antPosition);

	if ( fullFrame == false ) {
		Eigen::Vector2d offset;
		offset.x() = std::clamp(150.0-position.x(),300.0-data.Width,0.0);
		offset.y() = std::clamp(150.0-position.y(),300.0-data.Height,0.0);
		position += offset;
		for ( auto & p : corners ) {
			p += offset;
		}
	}

	priv::FrameReference ref(d_tddInfo.AbsoluteFilePath.filename(),
	                         frameID,
	                         data.FrameTime);
	auto tcu = std::make_shared<priv::TagCloseUp>(path,
	                                              ref,
	                                              antID-1,
	                                              position,
	                                              angle,
	                                              corners);
	d_tddInfo.TagCloseUps.push_back(tcu);
}



} // namespace myrmidon
} // namespace fort
