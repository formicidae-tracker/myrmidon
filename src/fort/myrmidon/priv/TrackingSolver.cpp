#include "TrackingSolver.hpp"
#include "CollisionSolver.hpp"
#include "Identifier.hpp"
#include "fort/myrmidon/types/IdentifiedFrame.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

TrackingSolver::TrackingSolver(
    const std::shared_ptr<const Identifier> &identifier,
    const CollisionSolver::ConstPtr         &solver
)
    : d_solver(solver) {
	d_identifier = Identifier::Compile(identifier);
}

void TrackingSolver::IdentifyFrame(
    IdentifiedFrame                  &identified,
    const fort::hermes::FrameReadout &frame,
    SpaceID                           spaceID,
    size_t                            zoneDepth,
    ZonePriority                      zoneOrder
) const {

	identified.Space     = spaceID;
	identified.FrameTime = Time::FromTimestamp(frame.time());
	identified.Width     = frame.width();
	identified.Height    = frame.height();
	identified.Positions.resize(frame.tags().size(), 5);
	size_t index = 0;
	for (const auto &t : frame.tags()) {
		auto identification =
		    d_identifier->Identify(t.id(), identified.FrameTime);
		if (!identification == true) {
			continue;
		}
		identified.Positions(index, 0) = identification->Target()->AntID();
		if (zoneDepth > 0) {
			identified.Positions.block(index, 4, 1, zoneDepth).setConstant(0.0);
		}
		identification->ComputePositionFromTag(
		    identified.Positions(index, 1),
		    identified.Positions(index, 2),
		    identified.Positions(index, 3),
		    Eigen::Vector2d(t.x(), t.y()),
		    t.theta()
		);
		++index;
	}
	identified.Positions.conservativeResize(index, 4 + zoneDepth);
	if (zoneDepth > 0) {
		auto zoner = d_solver->ZonerFor(identified);
		zoner->LocateAnts(identified.Positions, zoneOrder);
	}
}

void TrackingSolver::CollideFrame(
    CollisionFrame &collision, IdentifiedFrame &identified
) const {
	d_solver->ComputeCollisions(collision, identified);
}

AntID TrackingSolver::IdentifyTag(TagID tagID, const Time &time) {
	auto identification = d_identifier->Identify(tagID, time);
	if (!identification) {
		return 0;
	}
	return identification->Target()->AntID();
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
