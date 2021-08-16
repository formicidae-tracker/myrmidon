#pragma once

#include "UTestData.hpp"
#include "SegmentedDataWriter.hpp"
#include "FrameDrawer.hpp"

#include <fort/myrmidon/utils/FileSystem.hpp>

#include <opencv2/core.hpp>

namespace fort {
namespace myrmidon {


class CloseUpWriter : public SegmentedDataWriter {
public:
	CloseUpWriter(UTestData::TDDInfo & tddInfo,
	              const FrameDrawer::Ptr & drawer);
	virtual ~CloseUpWriter();
	void Prepare(size_t index) override;
	void WriteFrom(const IdentifiedFrame & data,
	               uint64_t frameID) override;
	void Finalize(size_t index,bool last) override;
private:
	UTestData::TDDInfo & d_tddInfo;
	bool d_fullFrameNeeded;

	void SaveFullFrame(const cv::Mat & frame,
	                   uint64_t frameID);

	void SaveCloseUp(const cv::Mat & frame,
	                 uint64_t frameID,
	                 AntID antID,
	                 const cv::Point2f & position);

	void SaveExpectedFullFrame(const IdentifiedFrame & data,
	                           uint64_t frameID);

	void SaveExpectedCloseUp(const IdentifiedFrame & data,
	                         uint64_t frameID,
	                         AntID antID);

	std::string FullFramePath(uint64_t frameID) const;

	std::string CloseUpPath(uint64_t frameID, AntID antID) const;


	std::set<AntID> d_seen;

	FrameDrawer::Ptr d_drawer;
	cv::Mat          d_frameBuffer;
};


} // namespace myrmidon
} // namespace fort
