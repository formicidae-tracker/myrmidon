#pragma once

#include <memory>

#include <Eigen/Core>

#include <fort/time/Time.hpp>

#include "TraitsCategory.hpp"
#include "Typedefs.hpp"

namespace fort {
namespace myrmidon {

/**
 * Defines a trajectory for an Ant
 */
struct AntTrajectory {
	/**
	 * A pointer to the trajectory
	 */
	typedef std::shared_ptr<AntTrajectory> Ptr;

	/**
	 * Reports the AntID of the Ant this trajectory refers to.
	 */
	AntID   Ant;
	/**
	 * Reports the Space this trajectory is taking place.
	 */
	SpaceID Space;
	/**
	 * Reports the starting Time of this trajectory.
	 *
	 * Reports the starting Time of this trajectory. Positions
	 * first column are second offset from this time.
	 */
	Time    Start;
	/**
	 * Reports the time and position in the frame.
	 *
	 * Reports the time and position in the frame.
	 *
	 * * first column: offset in second since Start
	 * * second and third column: X,Y position in the image
	 * * fourth column: Angle in ]-π,π], in trigonometric
	 *   orientation. As in images Y axis points bottom, positove
	 *   angle appears clockwise.
	 * * fith column: the zone of the ant
	 */
	Eigen::Matrix<double,Eigen::Dynamic,5> Positions;

	/**
	 * End Time for this Trajectory
	 *
	 * @return a Time computed from Start and the Positions
	 *         data.
	 */
	Time End() const;

	typedef time_ranged_data data_category;
};

} // namespace myrmidon
} // namespace fort
