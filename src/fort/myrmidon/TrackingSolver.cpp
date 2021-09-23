#include "TrackingSolver.hpp"

#include <fort/myrmidon/priv/TrackingSolver.hpp>


namespace fort {
namespace myrmidon {


TrackingSolver::TrackingSolver(const PPtr & pTracker)
	: d_p(pTracker) {
}

void TrackingSolver::IdentifyFrame(IdentifiedFrame & identified,
                                                   const fort::hermes::FrameReadout & frame,
                                                   SpaceID spaceID) const {
	d_p->IdentifyFrame(identified,frame,spaceID);
}

void TrackingSolver::CollideFrame(IdentifiedFrame & identified,
                                  CollisionFrame & collision) const {
	d_p->CollideFrame(collision,identified);
}

AntID TrackingSolver::IdentifyAnt(TagID tagID, const Time & time) {
	return d_p->IdentifyTag(tagID,time);
}


} // namespace myrmidon
} // namespace fort
