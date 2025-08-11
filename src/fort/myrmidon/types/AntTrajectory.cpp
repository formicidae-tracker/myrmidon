#include "AntTrajectory.hpp"

namespace fort {
namespace myrmidon {

Time AntTrajectory::End() const {
	if (Positions.rows() == 0) {
		return Start;
	}
	return Start.Add(Duration(Positions(Positions.rows() - 1, 0) * 1.0e9));
}

} // namespace myrmidon
} // namespace fort
