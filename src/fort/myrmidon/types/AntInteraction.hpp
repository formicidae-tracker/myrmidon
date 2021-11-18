#pragma once

#include <cstdint>
#include <set>
#include <memory>
#include <variant>

#include <Eigen/Core>

#include <fort/time/Time.hpp>

#include "Typedefs.hpp"
#include "Collision.hpp"
#include "AntTrajectory.hpp"
#include "TraitsCategory.hpp"

namespace fort {
namespace myrmidon {

/**
 * Defines a sub segment of a trajectory
 */
struct AntTrajectorySegment {
	/**
	 * The refering trajectory
	 */
	AntTrajectory::Ptr Trajectory;
	/**
	 * The starting index of the segment in the referring trajectory.
	 */
	size_t Begin;
	/**
	 * The index after the last index in the referring trajectory.
	 */
	size_t End;

	Time StartTime() const;
	Time EndTime() const;

};

/**
 * Defines a trajectory sub-segment summary
 */
struct AntTrajectorySummary {
	/**
	 * The mean position during the trajectory.
	 */
	Eigen::Vector3d Mean;
	/**
	 * The list of zone traversed by the trajectory.
	 */
	std::set<ZoneID> Zones;
};


/**
 * Defines an interaction between two Ants
 */
struct AntInteraction {
	/**
	 * A pointer to the interaction structure
	 */
	typedef std::shared_ptr<AntInteraction> Ptr;

	/**
	 * The IDs of the two Ant.
	 *
	 * The ID of the two Ant. Always reports `IDs.first <
	 * IDs.second`.
	 */
	InteractionID                      IDs;
	/**
	 * Virtual shape body part that were in contact.
	 *
	 * Virtual shape body part that were in contact during the
	 * interaction.
	 */
	InteractionTypes                  Types;
	/**
	 * Reports the AntTrajectory or their summary for each Ant during
	 * the interaction. The trajectories are truncated to the
	 * interaction timing.
	 */
	std::variant<std::pair<AntTrajectorySegment,
	                       AntTrajectorySegment>,
	             std::pair<AntTrajectorySummary,
	                       AntTrajectorySummary>> Trajectories;
	/**
	 * Reports the Time the interaction starts
	 */
	Time                               Start;
	/**
	 * Reports the Time the interaction ends
	 */
	Time                               End;
	/**
	 * Reports the SpaceID where the interaction happend
	 */
	SpaceID                            Space;

	typedef time_ranged_data data_category;
};

} // namespace myrmidon
} // namespace fort
