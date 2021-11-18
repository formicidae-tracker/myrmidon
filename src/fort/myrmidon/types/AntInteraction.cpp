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

} // namespace myrmidon
} // namespace fort
