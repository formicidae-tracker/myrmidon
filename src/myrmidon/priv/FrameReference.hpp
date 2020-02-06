#pragma once

#include <myrmidon/Time.hpp>

#include "Types.hpp"
#include "LocatableTypes.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

class FrameReference : public Identifiable {
public:
	typedef std::shared_ptr<FrameReference> Ptr;
	typedef std::shared_ptr<const FrameReference> ConstPtr;

	FrameReference();

	FrameReference(const fs::path & parentURI,
	               FrameID frameID,
	               const fort::myrmidon::Time & Time);

	virtual ~FrameReference();

	const fs::path & ParentURI() const;

	// The Time of the Frame
	//
	// @return the <Time> of the designated frame
	const fort::myrmidon::Time & Time() const;

	// The FrameID of the frame
	//
	// @return the <FrameID> of the designated frame
	FrameID ID() const;

	// A Path uniquely defining the FramePointer
	//
	// @return a fs::path uniquely identifying the Frame
	const fs::path & URI() const override;

private:
	fs::path             d_parentURI;
	fs::path             d_URI;
	FrameID              d_id;
	fort::myrmidon::Time d_time;
};

} //namespace priv

} //namespace myrmidon

} //namespace fort

// Formats a FrameReference
// @out the std::ostream to format to
// @p the <fort::myrmidon::priv::FrameReference> to format
// @return a reference to <out>
std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::FrameReference & p);
