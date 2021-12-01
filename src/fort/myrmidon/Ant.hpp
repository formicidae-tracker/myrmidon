#pragma once

#include <memory>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/Typedefs.hpp>
#include <fort/myrmidon/types/ForwardDeclaration.hpp>
#include <fort/myrmidon/types/Color.hpp>
#include <fort/myrmidon/types/Value.hpp>

#include "Shapes.hpp"

namespace fort {
namespace myrmidon {

class AntHandle;

/**
 * Main object of interest of any Experiment
 *
 * Ant are the main object of interest of an Experiment. They are
 * identified from tags with Identification, have a virtual shape to
 * perform collision and interaction detection, and holds user defined
 * metadata.
 *
 * @note Ant can only be created from an Experiment with
 * Experiment::CreateAnt().
 *
 * Naming
 * ======
 *
 * Ant are uniquely identified by their ID. By convention we use
 * decimal notation with up to two `0` prefix to display an ID, as
 * returned by AntFormatID.
 *
 * Identification
 * ==============
 *
 * Instead of working directly with TagID **fort-myrmidon** uses
 * Identification to relates TagID to an Ant. An Ant could have
 * different Identification, allowing us to use different TagID to
 * refer to the same individual. One would use IdentifiedAt() to
 * obtain the TagID that identifies an Ant at a given Time.
 *
 * Ant Virtual Shape
 * =================
 *
 * Each Ant has an associated virtual shape that is used to compute
 * instantaneous Collision detection ( Query::CollideFrame() ), or
 * timed AntInteraction ( Query::ComputeAntInteraction ). These shape
 * can be defined manually in `fort-studio` or programmatically
 * accessed and modified with Capsules(), AddCaspule(),
 * DeleteCapsule() and ClearCapsules().
 *
 * Visualization meta-data
 * =======================
 *
 * Basic visualization of Experiment data can be done through
 * **fort-studio**. Ants are displayed according to their
 * DisplayStatus() and DisplayColor(), which can be programmaticaly
 * modified using SetDisplayStatus() and SetDisplayColor().
 *
 * User defined meta-data (named values)
 * =====================================
 *
 * Ant can stores timed user defined metadata. These are modifiable
 * using SetValue() and DeleteValue() and accesible through
 * GetValue().
 *
 */
class Ant {
public:
	/**
	 * A pointer to an Ant
	 */
	typedef std::shared_ptr<Ant>       Ptr;

	/**
	 * The DisplayState of an Ant in an Experiment
	 */
	enum class DisplayState {
	                         /**
	                          * the Ant is visible
	                          */
	                         VISIBLE = 0,
	                         /**
	                          * the Ant is hidden
	                          */
	                         HIDDEN  = 1,
	                         /**
	                          * Ant is visible and all non-SOLO Ant will be hidden.
	                          */
	                         SOLO    = 2,
	};

	/**
	 * Gets the ::TagID identifying this Ant at a given time.
	 *
	 * @param time the Time for which we want the identification
	 *
	 * Gets the ::TagID identifying this Ant at a given Time. If no
	 * Identification are valid for this time, an an exception will be
	 * thrown.
	 *
	 *
	 * @return a ::TagID that identify this ant at this time.
	 *
	 * @throws std::runtime_error if there no valid Identification for this time.
	 */
	TagID IdentifiedAt(const Time & time) const;


	/**
	 * Gets the Identification targetting this Ant.
	 *
	 * Gets the Identification targetting this Ant. These
	 * Identification will always be sorted in Time and never overlaps.
	 *
	 * @return an Identification::List of Identification that target this object.
	 */
	const IdentificationList & Identifications() const;

	/**
	 *  Gets the AntID of an Ant.
	 *
	 * Ants gets an unique ID in an Experiment, with a minimal value
	 * of `1`. `0` is an invalid/undefined ::AntID.
	 *
	 * @return the ::AntID of the Ant
	 */
	fort::myrmidon::AntID ID() const;


	/**
	 *  Gets the Display Color of an Ant.
	 *
	 *  Each Ant has a defined Color for display in `fort-studio`.
	 *
	 * @return a const reference to the Color used to display the Ant
	 *         in `fort-studio`.
	 */
	const Color & DisplayColor() const;


	/**
	 * Sets the Ant display color
	 *
	 * @param color the new Color to use to display the Ant in `fort-studio`.
	 */

	void SetDisplayColor(const Color & color);

	/**
	 *  Gets the Ant display state
	 *
	 * When visualizing ant in `fort-studio`, any Ant has
	 * different DisplayState :
	 *
	 * * DisplayState::VISIBLE: the Ant is visible if
	 *   they are no other Ant which are in DisplayState::SOLO
	 * * DisplayState::HIDDEN: the Ant is not displayed
	 * * DisplayState::SOLO: the Ant is visible as any other Ant
	 *   which are in DisplayState::SOLO.
	 *
	 * @return the DisplayState for this Ant.
	 */
	DisplayState DisplayStatus() const;

	/**
	 * Sets the Ant display state.
	 *
	 * @param s the wanted DisplayState
	 */
	void SetDisplayStatus(DisplayState s);

	/**
	 * Gets user defined timed metadata.
	 *
	 * @param key the key of the user defined key in Experiment
	 * @param time the Time we want the value for (infinite Time are valid)
	 *
	 * Gets the value for a key at time. Values are set with
	 * SetValue(). If no value is sets prior to Time (including -∞),
	 * the Experiment default value for key will be returned.
	 *
	 * @return the wanted Value for key at time, or the Experiment default one
	 *
	 * @throws std::out_of_range if name is not a defined metadata key in Experiment.
	 */
	const Value & GetValue(const std::string & key,
	                                const Time & time) const;

	/**
	 *  Sets a user defined timed metadata
	 *
	 * @param key the metadata key to set
	 * @param value the desired Value
	 * @param time the first Time after which name will be set to
	 *        value. It can be Time::SinceEver().
	 *
	 * Sets key to value starting from time. If time is
	 * Time::SinceEver(), sets the starting value for name instead of
	 * the Experiment's default value for key.
	 *
	 * @throws std::out_of_range if name is not a defined key in Experiment
	 * @throws std::invalid_argument if time is Time::Forever()
	 * @throws std::runtime_error if value is not of the right type for key
	 *
	 */
	void SetValue(const std::string & key,
	              const Value & value,
	              const Time & time);

	/**
	 * Removes any user defined value at a given time
	 *
	 * @param key the key to remove
	 * @param time the Time to remove. It can be Time::SinceEver().
	 *
	 * Removes any value defined at a time.
	 *
	 * @throws std::out_of_range if no value for key at time have
	 *         been previously set with SetValue().
	 */
	void DeleteValue(const std::string & key,
	                 const Time & time);

	/**
	 * Gets all metadata key value changes over time
	 *
	 * @param key the key to list
	 * @return the values of key over the time
	 */
	const std::map<Time,Value> & GetValues(const std::string & key) const;

	/**
	 *  Adds a Capsule to the Ant virtual shape list.
	 *
	 * @param shapeTypeID the AntShapeTypeID for the Capsule
	 * @param capsule the Capsule
	 *
	 * Adds a Capsule to the Ant virtual shape, associated with the
	 * shapeTypeID body part.
	 *
	 * @throws std::invalid_argument if shapeTypeID is not defined in Experiment
	 */
	void AddCapsule(AntShapeTypeID shapeTypeID,
	                const std::shared_ptr<Capsule> & capsule);

	/**
	 * Gets all capsules for this Ant
	 *
	 * @return a TypedCapsuleList representing the virtual shape of
	 *        the Ant
	 */
	const TypedCapsuleList & Capsules() const;

	/**
	 * Delete one of the virtual shape
	 *
	 * @param index the index in the Capsules() to remove
	 *
	 * @throws std::out_of_range if index is greate or equal to the size of Capsules().
	 */
	void DeleteCapsule(const size_t index);

	/**
	 *  Deletes all virtual shape parts
	 *
	 */
	void ClearCapsules();

	~Ant();

private:
	friend class ExperimentHandle;

	// Private implementation constructor
	// @pAnt opaque pointer to implementation
	//
	// User cannot build Ant directly. They must be build and accessed
	// from <Experiment>.
	Ant(std::unique_ptr<AntHandle> handle);

	Ant & operator=(const Ant &) = delete;
	Ant(const Ant &) = delete;

	std::unique_ptr<AntHandle> d_p;
};


} // namespace myrmidon
} //namespace fort
