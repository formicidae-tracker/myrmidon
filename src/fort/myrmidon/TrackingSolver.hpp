#pragma once

#include <memory>

#include <fort/hermes/FrameReadout.pb.h>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/Typedefs.hpp>
#include <fort/myrmidon/types/IdentifiedFrame.hpp>
#include <fort/myrmidon/types/Collision.hpp>


namespace fort {
namespace myrmidon {

class Experiment;

namespace priv {
class TrackingSolver;
} // namespace priv


/**
 * Identifies and Collides Ant from raw tracking data
 *
 * This class lets the user to manually identify and track ants from
 * raw tracking data, as for example, obtained from a network stream
 * with `fort-hermes`
 */

class TrackingSolver {
public :
	typedef std::unique_ptr<TrackingSolver> Ptr;
	/**
	 *  Identifies a single ant
	 *
	 * @param tagID the TagID to identify
	 * @param time the time to consider to identify the tag
	 *
	 * @return returns the AntID of the Ant tagID is identifying at
	 *         time, or 0 if there is no matching identification.
	 */
	AntID IdentifyAnt(TagID tagID, const Time & time);

	/**
	 * Identifies Ants from a `fort::hermes::FrameReadout`
	 *
	 * @param identified an IdentifiedFrame that will hold the Ant
	 *        positions
	 * @param frame the `fort::hermes::FrameReadout` to identify
	 * @param spaceID the spaceID the frame correspond to
	 *
	 */
	void IdentifyFrame(IdentifiedFrame & identified,
	                   const fort::hermes::FrameReadout & frame,
	                   SpaceID spaceID) const;

	/**
	 * Collides Ants from an IdentifiedFrame
	 *
	 * @param identified the IdentifiedFrame with the ant position
	 *        data and to return ant location.
	 * @param collision the CollisionFrame to return the collision.
	 *
	 * Collides Ants from an IdentifiedFrame. identified will be
	 * modified to contains for each Ant its current zone. collision
	 * frame will be set with all current collision found in
	 * identified.
	 *
	 */
	void CollideFrame(IdentifiedFrame & identified,
	                  CollisionFrame & collision) const;

private:
	friend class Experiment;
	// Opaque pointer to implementation
	typedef const std::shared_ptr<priv::TrackingSolver> PPtr;

	// Private implementation constructor
	// @pTracker opaque pointer to implementation
	//
	// User cannot create a TrackingSolver directly. They must use
	// <Experiment::CompileTrackingSolver>.
	TrackingSolver(const PPtr & pTracker);

	TrackingSolver & operator=(const TrackingSolver &) = delete;
	TrackingSolver(const TrackingSolver &) = delete;

	PPtr d_p;
};



} //namespace myrmidon
} //namespace fort
