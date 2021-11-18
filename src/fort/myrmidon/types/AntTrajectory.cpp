#include "AntTrajectory.hpp"

namespace fort {
namespace myrmidon {

Time AntTrajectory::End() const {
	if ( Positions.rows() == 0 ) {
		return Start;
	}
	return Start.Add(Duration(Positions(Positions.rows()-1,0) * Duration::Second.Nanoseconds()));
}

} // namespace myrmidon
} // namespace fort
