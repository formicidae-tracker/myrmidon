#pragma once

#include "FrameDrawer.hpp"
#include "SegmentedDataWriter.hpp"
#include "UTestData.hpp"

#include <fort/myrmidon/utils/FileSystem.hpp>

namespace fort {
namespace myrmidon {

class CloseUpWriter : public SegmentedDataWriter {
public:
	CloseUpWriter(UTestData::TDDInfo &tddInfo, const FrameDrawer::Ptr &drawer);
	virtual ~CloseUpWriter();
	void Prepare(size_t index) override;
	void WriteFrom(const IdentifiedFrame &data, uint64_t frameID) override;
	void Finalize(size_t index, bool last) override;

private:
	UTestData::TDDInfo &d_tddInfo;
	bool                d_fullFrameNeeded;

	void SaveFullFrame(const video::Frame &frame, uint64_t frameID);

	void SaveCloseUp(
	    const video::Frame    &frame,
	    uint64_t               frameID,
	    AntID                  antID,
	    const Eigen::Vector2f &position
	);

	void SaveExpectedFullFrame(const IdentifiedFrame &data, uint64_t frameID);

	void SaveExpectedCloseUpFrame(
	    const IdentifiedFrame &data, uint64_t frameID, AntID antID
	);

	void SaveExpectedCloseUp(
	    const fs::path        &path,
	    const IdentifiedFrame &data,
	    uint64_t               frameID,
	    AntID                  antID,
	    bool                   fullFrame
	);

	std::string FullFramePath(uint64_t frameID) const;

	std::string CloseUpPath(uint64_t frameID, AntID antID) const;

	std::set<AntID> d_seen;

	FrameDrawer::Ptr d_drawer;
};

} // namespace myrmidon
} // namespace fort
