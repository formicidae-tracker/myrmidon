#pragma once

#include <memory>
#include <cstdint>

#include <Eigen/Core>

#include <fort/time/Time.hpp>

#include "TraitsCategory.hpp"
#include "Typedefs.hpp"

/**
 * the namespace for all the FORmicidae Tracker libraries
 */

namespace fort {

/**
 * FORT post-processing namespace
 *
 */

namespace myrmidon {

/**
 * A video frame were Ant have been identified from their TagID
 */
struct IdentifiedFrame {
	/**
	 * A pointer to an IdentifiedFrame
	 */
	typedef std::shared_ptr<IdentifiedFrame> Ptr;

	/**
	 * A Matrix of ant position.
	 *
	 * * The first column is the AntID
	 * * The second and third columns are the x,y position in the original video
	 * frame.
	 * * The fourth column is the ant angle
	 * * The fifth column and further (if presents) are the ant current zones
	 *   IDs or 0 if not in any zones.
	 */
	typedef Eigen::
	    Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
	        PositionMatrix;

	/**
	 * The acquisition Time of this frame
	 */
	Time           FrameTime;
	/**
	 *  The Space this frame belongs to.
	 */
	SpaceID        Space;
	/**
	 *  The original height of the video frame
	 */
	size_t         Height;
	/**
	 *  The original width of the video frame
	 */
	size_t         Width;
	/**
	 * The position of each ant in that frame
	 */
	PositionMatrix Positions;

	/**
	 * Tests if the frame contains an Ant
	 * @param antID the AntID of the Ant to test for.
	 *
	 * @return `true` if antID is in Positions
	 */
	bool Contains(uint64_t antID) const;

	/**
	 * Extract Ant position data at a given row
	 * @param index the row we want to extract data from
	 *
	 * @return the AntID, x,y,angle position and the zones IDs for the
	 *         Ant at index.
	 */
	std::tuple<
	    AntID,
	    const Eigen::Ref<const Eigen::Vector3d>,
	    const Eigen::Ref<const Eigen::VectorXd>>
	At(size_t index) const;

	// type traits;
	typedef timed_data data_category;
};

enum class ZonePriority {
	PREDECENCE_LOWER  = 0,
	PREDECENCE_HIGHER = 1,
};

} // namespace myrmidon
} // namespace fort
