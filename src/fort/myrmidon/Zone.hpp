#pragma once

#include <vector>

#include "Shapes.hpp"

namespace fort {
namespace myrmidon {

class ZoneDefinitionHandle;
class ZoneHandle;

/**
 * Defines the shape of a Zone in Time
 *
 * ZoneDefinition contains for a time range [Start(),End()[ the Shapes()
 * for a Zone.
 */
class ZoneDefinition {
public:
	/**
	 * A pointer to a Zone
	 */
	typedef std::shared_ptr<ZoneDefinition> Ptr;

	/**
	 * Gets the geometry of this ZoneDefinition
	 *
	 * * Python: `py_fort_myrmidon.ZoneDefinition.Shapes
	 *           (List[py_fort_myrmidon.Shape])` read-write property
	 *           of `py_fort_myrmidon.ZoneDefinition` objects.
	 * * R:
	 * ```R
	 * fmZoneDefinitionShapes <- function(zoneDefinition) # returns a list of Rcpp_fmShape
	 * ```
	 *
	 * @return a list of Shape that define the Shape of the Zone for
	 *         [Start();End()[
	 */
	const Shape::List & Shapes() const;

	/**
	 * Sets the Shapes of this ZoneDefinition
	 *
	 * * Python: `py_fort_myrmidon.ZoneDefinition.Shapes
	 *           (List[py_fort_myrmidon.Shape])` read-write property
	 *           of `py_fort_myrmidon.ZoneDefinition` objects.
	 * * R:
	 * ```R
	 * fmZoneDefinitionSetShapes <- function (zoneDefinition,shapes = list())
	 * ```
	 *
	 * @param shapes a union of Shape defining the <Zone> shapes.
	 */
	void SetShapes(const Shape::List & shapes);

	/**
	 * Gets the first valid time of the ZoneDefinition
	 *
	 * * Python: `py_fort_myrmidon.ZoneDefinition.Start
	 *           (py_fort_myrmidon.Time)` read-write property of
	 *           `py_fort_myrmidon.ZoneDefinition` objects.
	 * * R:
	 * ```
	 * fmZoneDefinitionStart <- function(zoneDefinition) # returns a Rcpp_fmTime
	 * ```
	 *
	 * @return the first valid Time of this definition. It can be
	 *         Time::SinceEver().
	 */
	const Time & Start() const;

	/**
	 * Gets the first invalid time of the ZoneDefinition
	 *
	 * * Python: `py_fort_myrmidon.ZoneDefinition.End
	 *            (py_fort_myrmidon.Time)` read-write property of
	 *            `py_fort_myrmidon.ZoneDefinition` objects.
	 * * R:
	 * ```
	 * fmZoneDefinitionEnd <- function(zoneDefinition) # returns a Rcpp_fmTime
	 * ```
	 *
	 * @return the first invalid Time of this definition. It can be
	 *         Time::Forever().
	 */
	const Time & End() const;

	/**
	 * Sets the first valid time of the ZoneDefinition
	 *
	 * * Python: `py_fort_myrmidon.ZoneDefinition.Start
	 *            (py_fort_myrmidon.Time)` read-write property of
	 *            `py_fort_myrmidon.ZoneDefinition` objects.
	 * * R:
	 * ```
	 * fmZoneDefinitionSetStart <- function(zoneDefinition,time = fmTimeSinceEver() )
	 * ```
	 *
	 * @param start the first valid Time of this definition. It can be
	 *        Time::SinceEver().
	 */
	void SetStart(const Time & start);

	/**
	 * Sets the first invalid time of the ZoneDefinition
	 *
	 * * Python: `py_fort_myrmidon.ZoneDefinition.Start
	 *           (py_fort_myrmidon.Time)` read-write property of
	 *           `py_fort_myrmidon.ZoneDefinition` objects.
	 * * R:
	 * ```
	 * fmZoneDefinitionSetStart <- function(zoneDefinition,time = fmTimeSinceEver() )
	 * ```
	 *
	 * @param start the first valid Time of this definition. It can be
	 *        Time::SinceEver().
	 */
	void SetEnd(const Time & end);

	ZoneDefinition & operator=( const ZoneDefinition &) = delete;
	ZoneDefinition(const ZoneDefinition & ) = delete;

	~ZoneDefinition();

private:
	friend class ZoneHandle;

	// Private implementation constructor
	// @pDefinition opaque pointer to implementation
	//
	// User cannot build Defoninition directly. They must be build and
	// accessed from <Zone>.
	ZoneDefinition(std::unique_ptr<ZoneDefinitionHandle> handle);

	std::unique_ptr<ZoneDefinitionHandle> d_p;
};


/**
 * A tracking region where collisions/interactions are computed.
 *
 * A Zone defines a tracked area region where collisions and
 * interactions can be computed. I.e. two Ant in different Zone won't
 * report any collisions or interactions.
 *
 * Naming and Identification
 * =========================
 *
 * Zone are managed by Space, and have a unique ZoneID within that
 * space. However two Zone from different Space can share the same
 * ZoneID. Zone have a user definable Name() but internally only their
 * ZoneID is used.
 *
 * By default, any Ant is considered to be within the `null` Zone
 * with ZoneID 0, so all possible interactions are reported. User
 * needs to add Zone only to prune unwanted interactions or to be able
 * to query where an Ant is at any given Time.
 *
 * Geometric definition
 * ====================
 *
 * Zone have time valid ZoneDefinition. In most cases there would be a
 * single ZoneDefinition for any Zone valid for
 * ]Time::SinceEver(),Time::Forever()[, but it is possible to assign
 * different ZoneDefinition::Shapes() for different time range using
 * multiple ZoneDefinition.
 *
 * ZoneDefinition are manipulated using AddDefinition() and
 * DeleteDefinition().
 */
class Zone {
public:
	/**
	 * a pointer to a Zone
	 */
	typedef std::shared_ptr<Zone> Ptr;
	/**
	 * Adds a new timed ZoneDefinition
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Zone.AddDefinition(self,shapes: List[py_fort_myrmidon.Shape], start: py_fort_myrmidon.Time, end: py_fort_myrmidon.Time) -> py_fort_myrmidon.ZoneDefinition
	 * ```
	 * * R:
	 * ```
	 * fmZoneAddDefinition <- function(zone,shapes = list(), start = fmTimeSinceEver(), end = fmTimeForever()) # returns a Rcpp_fmZoneDefinition
	 * ```
	 *
	 * Adds a new timed ZoneDefinition valid for [start,end[ to this Zone. It
	 * accepts Time::SinceEver() and Time::Forever() for start or
	 * end.
	 *
	 * @param shapes the shape of the Zone as a Shape::List
	 * @param start the first valid Time for this definition.
	 * @end the end valid Time for this definition
	 *
	 * @return the new ZoneDefinition
	 *
	 * @throws std::invalid_argument if start or end would make the
	 *         resulting definition overlap in time with another
	 *         ZoneDefinition for this Zone.
	 */
	const ZoneDefinition::Ptr & AddDefinition(const Shape::List & shapes,
	                                          const Time & start,
	                                          const Time & end);

	/**
	 * Gets the Zone's ZoneDefinition
	 *
	 * * Python: `py_fort_myrmidon.Zone.Definitions (List[py_fort_myrmidon.ZoneDefinition])` read-only property of `py_fort_myrmidon.Zone` objects.
	 * * R:
	 * ```R
	 * fmZoneDefinitions <- function(zone) # returns a list of Rcpp_fmZoneDefinition
	 * ```
	 *
	 * @return a ZoneDefinition::List of ZoneDefinition for this Zone
	 *
	 */
	const ZoneDefinitionList & Definitions() const;

	/**
	 *
	 * Removes a ZoneDefinition
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Zone.DeleteDefinition(self,index)
	 * ```
	 * * R:
	 * ```R
	 * fmZoneDeleteDefinition <- function(zone,index = 0) # warning index starts at zero
	 * ```
	 *
	 * @index the index in Definitions() to remove.
	 *
	 * @throws std::out_of_range if index >= Definitions().size()
	 */
	void DeleteDefinition(size_t index);

	/**
	 * Gets the Zone name
	 *
	 * * Python: `py_fort_myrmidon.Zone.Name (str)` read-write property of `py_fort_myrmidon.Zone` objects
	 * * R:
	 * ```R
	 * fmZoneName <- function(zone) # returns a character
	 * ```
	 *
	 * @return the Zone name
	 */
	const std::string & Name() const;

	/**
	 * Gets the Zone name
	 *
	 * * Python: `py_fort_myrmidon.Zone.Name (str)` read-write property of `py_fort_myrmidon.Zone` objects
	 * * R:
	 * ```R
	 * fmZoneSetName <- function(zone,name = '')
	 * ```
	 *
	 * @param name the wanted Name()
	 */
	void SetName(const std::string & name);

	/**
	 * Gets the Zone ID
	 *
	 * * Python: `py_fort_myrmidon.Zone.ID (int)` read-only property of `py_fort_myrmidon.Zone` objects
	 * * R:
	 * ```
	 * fmZoneID <- function(zone) # returns an integer
	 * ```
	 *
	 * @return the Zone ZoneID
	 */
	ZoneID ID() const;


	~Zone();

private:
	friend class Space;

	// Private implementation constructor
	// @pZone opaque pointer to implementation
	//
	// User cannot build Zone directly. They must be build and
	// accessed from <Space>.
	Zone(std::unique_ptr<ZoneHandle> handle);

	Zone & operator=( const Zone &) = delete;
	Zone(const Zone &) = delete;

	std::unique_ptr<ZoneHandle> d_p;
};


} // namespace myrmidon
} // namespace fort
