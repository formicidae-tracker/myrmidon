#pragma once

#include "SegmentedDataWriter.hpp"
#include "FrameDrawer.hpp"

namespace fort {
namespace myrmidon {


class CloseUpWriter : public SegmentedDataWriter {
public:
	CloseUpWriter(const FrameDrawer::Ptr & drawer);
	virtual ~CloseUpWriter();
	void Prepare(size_t index) override {}
	void WriteFrom(const IdentifiedFrame & data,
	               uint64_t frameID) override {}
	void Finalize(size_t index,bool last) override {}
private:

	FrameDrawer::Ptr d_drawer;
};


} // namespace myrmidon
} // namespace fort
