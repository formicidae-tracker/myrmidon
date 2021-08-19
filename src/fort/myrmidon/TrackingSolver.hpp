#pragma once

#include <fort/hermes/FrameReadout.pb.h>

#include "Types.hpp"
#include "Ant.hpp"
#include "Space.hpp"



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
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.TrackingSolver(self,tagID: int,time: py_fort_myrmidon.Time) -> int
	 * ```
	 * * R:
	 * fmTrackingSolverIdentifyAnt <- function(solver, tagID = 0, time = fmTimeNow()) # returns an integer
	 * ```
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
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Trackingsolver.IdentifyFrame(self,frame: py_fort_hermes.FrameReadout, spaceID: int) -> py_fort_myrmidon.IdentifiedFrame
	 * ```
	 * * R:
	 * ```R
	 * fmTrackingSolverIdentifyFrame <- function(solver, frame, spaceID = 0)
	 * ```
	 *
	 * @param frame the `fort::hermes::FrameReadout` to identify
	 * @param spaceID the spaceID the frame correspond to
	 *
	 * @return an IdentifiedFrame with all identified ant without their zone
	 */
	void IdentifyFrame(IdentifiedFrame & identified,
	                   const fort::hermes::FrameReadout & frame,
	                   SpaceID spaceID) const;

	/**
	 * Collides Ants from an IdentifiedFrame
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Trackingsolver.CollideFrame(self,identified: py_fort_myrmidon.IdentifiedFrame) -> py_fort_myrmidon.CollisionFrame
	 * ```
	 * * R:
	 * ```R
	 * fmTrackingSolverCollideFrame <- function(solver, identified)
	 * ```
	 *
	 * @param identified the IdentifiedFrame with the ant position data.
	 *
	 * Collides Ants from an IdentifiedFrame. identified will be
	 * modified to contains for each Ant its current zone.
	 *
	 * @return a CollisionFrame with all current Ant collisions.
	 */
	void CollideFrame(CollisionFrame & collision,
	                  IdentifiedFrame & identified) const;

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
