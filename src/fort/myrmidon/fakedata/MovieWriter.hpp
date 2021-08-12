#pragma once

#include "SegmentedDataWriter.hpp"
#include "FrameDrawer.hpp"

namespace fort {
namespace myrmidon {


class MovieWriter : public SegmentedDataWriter {
public:
	MovieWriter(const FrameDrawer::Ptr & drawer);
	virtual ~MovieWriter();

	void Prepare(size_t index) override {}
	void WriteFrom(const IdentifiedFrame::Ptr & data,
	               uint64_t frameID) override {}
	void Finalize(size_t index,bool last) override {}
private:
	FrameDrawer::Ptr d_drawer;
};


} // namespace myrmidon
} // namespace fort
