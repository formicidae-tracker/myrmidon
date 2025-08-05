#include "AntInteraction.hpp"


namespace fort {
namespace myrmidon {

Time AntTrajectorySegment::StartTime() const {
	if ( !Trajectory ) {
		return Time::SinceEver();
	}
	return Trajectory->Start.Add(Trajectory->Positions(Begin,0)*Duration::Second.Nanoseconds());
}

Time AntTrajectorySegment::EndTime() const {
	if ( !Trajectory ) {
		return Time::Forever();
	}
	return Trajectory->Start.Add(Trajectory->Positions(End-1,0)*Duration::Second.Nanoseconds());
}

bool AntInteraction::HasInteractionType(
    AntShapeTypeID type1, AntShapeTypeID type2
) const {
	const auto &types = this->Types.array();
	return ((types.col(0) == type1) * (types.col(1) == type2)).any();
}

} // namespace myrmidon
} // namespace fort
