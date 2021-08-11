#pragma once

#include "SegmentedDataWriter.hpp"

namespace fort {
namespace myrmidon {


class CloseUpWriter : public SegmentedDataWriter {
public:
	void Prepare(size_t index) override {}
	void WriteFrom(const IdentifiedFrame::Ptr & data,
	               uint64_t frameID) override {}
	void Finalize(size_t index,bool last) override {}
};


} // namespace myrmidon
} // namespace fort
