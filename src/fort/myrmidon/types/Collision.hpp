#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include <memory>

#include <Eigen/Core>

#include <fort/time/Time.hpp>

#include "Typedefs.hpp"
#include "TraitsCategory.hpp"
#include "IdentifiedFrame.hpp"

namespace fort {
namespace myrmidon {

/**
 * Designates an interaction between two Ant
 *
 * Designates an interaction between two Ant, using their
 * AntID. InteractionID are always constructed such as the first ID
 * is strictly smaller than the second ID, so it ensures uniqueness of
 * the InteractionID to reports interactions and collisions.
*/
typedef std::pair<AntID,AntID>                   InteractionID;

/**
 * Designates list of interaction type for an interaction.
 *
 * Designates an interaction type for an interaction. Each line
 * represent a colliding capsules type. First column specifies the
 * type for the first ant and the second column the second
 * ant. Therefore (2,1) is not identical to (1,2).
 */
typedef Eigen::Matrix<uint32_t,Eigen::Dynamic,2> InteractionTypes;

/**
 * Defines an interaction between two Ant, ponctual in Time
 */
struct Collision {
	/**
	 * The AntID of the two Ants interacting.
	 *
	 * The AntID of the two Ants interacting. Please note that
	 * `IDS.first < IDs.second` remains always true, to ensure
	 * uniqueness of IDs for AntInteraction.
	 */
	InteractionID                IDs;
	/**
	 * Reports all virtual AntShapeTypeID interacting between the two Ants.
	 */
	InteractionTypes             Types;
	/**
	 * Reports the Zone where the interaction happened.
	 *
	 * Reports the Zone where the interaction happened, the
	 * corresponding Space is reported in CollisionFrame. 0 means
	 * the default zone.
	 */
	ZoneID                       Zone;
};

/**
 * Reports all Collision happening at a given time.
 */
struct CollisionFrame {
	/**
	 * A pointer to a CollisionFrame
	 */
	typedef std::shared_ptr<CollisionFrame> Ptr;
	/**
	 * The Time when the interaction happens
	 */
	Time                   FrameTime;
	/**
	 * Reports the Space this frame is taken from
	 */
	SpaceID                Space;
	/**
	 * The Collision taking place at FrameTime
	 */
	std::vector<Collision> Collisions;

	// type traits;
	typedef timed_data data_category;
};

/**
 *  Data returned by Query::CollideFrames
 */
typedef std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr> CollisionData;

template <typename T> struct data_traits;
template <>
struct data_traits<CollisionData> {
	typedef timed_data data_category;
	const static bool spaced_data = true;

	inline static SpaceID space(const CollisionData & v) {
		return v.first->Space;
	}

	inline static const fort::Time & time(const CollisionData & v) {
		return v.first->FrameTime;
	}

	inline static bool compare(const CollisionData & a,
	                           const CollisionData & b) {
		return a.first->FrameTime < b.first->FrameTime;
	}
};


} // namespace myrmidon
} // namespace fort
