#pragma once

#include <fort/myrmidon/Types.hpp>

namespace fort {
namespace myrmidon {



class SegmentedDataWriter {
public:
	typedef std::shared_ptr<SegmentedDataWriter> Ptr;
	typedef std::vector<Ptr> List;

	virtual ~SegmentedDataWriter(){};

	virtual void Prepare(size_t index) = 0;
	virtual void WriteFrom(const IdentifiedFrame::Ptr & data,
	                       uint64_t frameID) = 0;
	virtual void Finalize(size_t index,bool last) = 0;
};

} // namespace myrmidon
} // namespace fort
