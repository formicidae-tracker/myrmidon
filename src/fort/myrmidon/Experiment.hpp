#pragma once

#include <memory>
#include <string>

#include <fort/tags/fort-tags.hpp>

#include <fort/myrmidon/types/FixableError.hpp>
#include <fort/myrmidon/types/Reporter.hpp>
#include <fort/myrmidon/types/Typedefs.hpp>

#include "Ant.hpp"
#include "Identification.hpp"
#include "Space.hpp"
#include "TrackingSolver.hpp"

namespace fort {
namespace myrmidon {

class ExperimentHandle;

class Query;

/**
 * Entry point of the **fort-myrmidon** API
 *
 * An Experiment olds a collection of Ant, Identification, Space and
 * Zone and give access to the identified tracking data instantaneous
 * collision and interaction detection through Query .
 *
 * File convention
 * ===============
 *
 * Experiment are save to the disk in `.myrmidon` files. One can use
 * Open(), OpenDataLess(), and Save() to interact with these files.
 *
 * Linking with acquired data
 * ==========================
 *
 * One can use AddTrackingDataDirectory() to link an Experiment with
 * some tracking data, organized by **fort-leto** in a tracking data
 * directory. This data must be assigned to a Space (previously
 * created with CreateSpace()).  Experiment saves relative links to
 * these tracking data directory. These paths are relative, so one can
 * rename a `.myrmidon` file on the filesystem with Save(), but it
 * must remains in the same directory.
 *
 * Tracking meta-data
 * ==================
 *
 * In **fort-myrmidon**, tags are not used directly. Instead user are
 * required to make Ant object (through CreateAnt()) and use
 * Identification (through AddIdentification()) to relate a tag value
 * to an Ant. To perform collision and interaction detection, users
 * can create for each Ant a virtual shape, made of a collection of
 * Capsule. Each Capsule is assigned an AntShapeTypeID (an integer
 * starting from 1) which must be previously defined using
 * CreateAntShapeType(). There is no predefined AntShapeTypeID
 *
 * **fort-studio** allows to make measurement on close-up of each
 * Ant. These measurement must be assigned to a type, created with
 * CreateMeasurementType(). There is a predefined, undeletable
 * MeasurementTypeID: HEAD_TAIL_MEASUREMENT_TYPE. It is used to
 * automatically determine Identification::AntPosition() and
 * Identification::AntAngle() from **fort-studio** measurement.
 *
 * User timed meta-data
 * ====================
 *
 * Each Ant can also holds a dictionnary of key/value pairs. The key
 * name, type and initial value for each Ant must be defined with
 * SetMetaDataKey(). Through Ant::SetValue(), individual, timed value
 * can be assigned to each Ant. There are no predefined keys.
 *
 * Working without acquired data
 * =============================
 *
 * An Experiment is also usuable without linking to any tracking
 * data. OpenDataLess() can be used to open an existing Experiment,
 * previously linked with acquired data, but without requiring the
 * data to be present. Any Query on such Experiment object will report
 * no data, but a TrackingSolver (acquired with
 * CompileTrackingSolver()) could be used, to perform, for example
 * identifications and collision detection on a live tracking
 * datastream of **fort-leto**. Also Tracking and user-defined
 * meta-data can be manipulated without the need of the often very
 * large tracking data directory to be present on the system.
 */
class Experiment {
public:
	/** A pointer to an Experiment. */
	typedef std::unique_ptr<Experiment> Ptr;

	/**
	 * the default MeasurementTypeID for Head-Tail measuremrent
	 */
	static const MeasurementTypeID HEAD_TAIL_MEASUREMENT_TYPE_ID;

	/**
	 * Opens an existing Experiment.
	 *
	 * @param filepath the path to the wanted file
	 *
	 * @return a pointer to the Experiment
	 *
	 * @throws std::runtime_error if **filepath** is not a valid
	 *         `.myrmidon` file.
	 */
	static Experiment::Ptr Open(const std::string &filepath) {
		return Ptr(new Experiment(OpenUnsafe(filepath)));
	}

	static Experiment OpenUnsafe(const std::string &filepath);

	/**
	 * Opens an Experiment without associated tracking data
	 *
	 * Opens an Experiment to a `.myrmidon` file without opening its
	 * associated tracking data. This is useful, by example, identify
	 * or collides ants from realtime tracking data acquired over the
	 * network using a TrackingSolver obtained with
	 * CompileTrackingSolver(). When opened in 'data-less' mode, no
	 * tracking data, tag statistic or measurement will be returned by
	 * any Query ( the experiment will appear empty ).
	 *
	 * @param filepath the path to the wanted file.
	 *
	 * @return the Experiment
	 *
	 * @throws std::runtime_error if **filepath** is not a valid
	 *         `.myrmidon` file.
	 */
	static Experiment::Ptr OpenDataLess(const std::string &filepath) {
		return Ptr(new Experiment(OpenDataLessUnsafe(filepath)));
	}

	static Experiment OpenDataLessUnsafe(const std::string &filepath);

	/**
	 * Creates a new Experiment associated with the given filepath.
	 *
	 * Creates a new Experiment virtually associated with the desired
	 * filepath location. It will not create a new file on the
	 * filesystem. The wanted location is required to compute relative
	 * path to the associated Tracking Data Directory.
	 *
	 * @param filepath the filesystem location for the `.myrmidon` file.
	 *
	 * @return the new empty Experiment
	 */
	static Experiment::Ptr Create(const std::string &filepath) {
		return Ptr(new Experiment(CreateUnsafe(filepath)));
	}

	static Experiment CreateUnsafe(const std::string &filepath);

	/**
	 * Saves the Experiment at the desired filepath
	 *
	 * Saves the Experiment to filepath. It is not possible to change
	 * its parent directory (but file renaming is permitted).
	 *
	 * @param filepath the desired filesystem location to save the Experiment to
	 *
	 * @throws std::invalid_argument if **filepath** will change the
	 *         parent directory of the Experiment.
	 */
	void Save(const std::string &filepath);

	/**
	 * Path to the underlying `.myrmidon` file
	 *
	 * @return the absolute filepath to the `.myrmidon` file
	 */
	std::string AbsoluteFilePath() const;

	/**
	 * Creates a new Space
	 *
	 * @param name wanted name for the new Space
	 *
	 * @return the newly created Space.
	 */
	Space::Ptr CreateSpace(const std::string &name);

	/**
	 * Deletes a Space
	 *
	 * @param spaceID the SpaceID of the Space we want to delete.
	 *
	 * @throws std::out_of_range if **spaceID** is not a valid ID for
	 *         one of this Experiment Space.
	 * @throws std::runtime_error if **spaceID** still contains any
	 *         tracking data directories.
	 */
	void DeleteSpace(SpaceID spaceID);

	/**
	 * Gets Space defined in the Experiment
	 *
	 * @return a map of the Experiment Space indexed by their SpaceID
	 */
	const SpaceByID &Spaces() const;

	/**
	 * Adds a tracking data directory to one of Experiment's Space
	 *
	 * Adds a tracking data director acquired with the FORT system to
	 * the wanted Space.
	 *
	 * @param spaceID the Space the data directory should be associated with
	 * @param filepath path to the directory we want to add
	 * @param fixCorruptedData silently fix any corrupted data,with
	 *        potentially loss of a large chunk of data. Otherwise a
	 *        FixableErrors will be thrown.
	 *
	 * @return the URI used to designate the tracking data directory
	 *
	 * @throws std::out_of_range if **spaceID** is not valid for this
	 *         Experiment
	 * @throws std::runtime_error if **filepath** is not a valid tracking
	 *         data directory
	 * @throws FixableErrors if the directory contains corrupted data,
	 *         and fixCorruptedData is false.
	 * @throws std::domain_error if **filepath** contains data that
	 *         would overlap in Time with another tracking data
	 *         directory associated with the same space.
	 * @throws std::invalid_argument if the tracking data directory
	 *         is already in use in this experiment.
	 */
	std::string AddTrackingDataDirectory(
	    SpaceID            spaceID,
	    const std::string &filepath,
	    bool               fixCorruptedData = false
	);
	/**
	 * Removes a Tracking Data Directory from the Experiment.
	 *
	 * @param URI the URI of the tracking data directory to remove
	 *
	 * @throws std::invalid_argument if **URI** does not designate a
	 *         tracking data directory in the experiment.
	 */
	void RemoveTrackingDataDirectory(const std::string &URI);

	/**
	 * Creates a new Ant in the Experiment.
	 *
	 * @return the newly created Ant
	 */
	Ant::Ptr CreateAnt();

	/**
	 * Gets the Ant in the Experiment
	 *
	 * @return the Ant indexed by their AntID in the Experiment.
	 */
	const AntByID &Ants() const;

	/**
	 * Deletes an Ant
	 *
	 * @param antID the AntID of the Ant to delete from the experiment
	 *
	 * @throws std::out_of_range if **antID** is not valid for this Experiment
	 * @throws std::runtime_error if the Ant stills have an identification
	 */
	void DeleteAnt(AntID antID);

	/**
	 * Adds an Identification to the Experiment
	 *
	 * Adds an Identification to the Experiment. Identification are
	 * valid for [**start**,**end**[. One may obtain a valid Time
	 * range using FreeIdentificationRangeAt().
	 *
	 * @param antID the targetted Ant designated by its AntID
	 * @param tagID the tag to associate with the Ant
	 * @param start the first valid Time. It can be Time::SinceEver()
	 * @param end the first invalid Time. It can be Time::Forever()
	 *
	 * @return the new Identification
	 *
	 * @throws std::out_of_range if **antID** is not valid for this
	 *         Experiment
	 * @throws OverlapingIdentification if it will conflict in time
	 *         with another Identification with the same **antID** or
	 *         **tagID**.
	 */
	Identification::Ptr AddIdentification(
	    AntID antID, TagID tagID, const Time &start, const Time &end
	);
	/**
	 * Deletes an Identification
	 *
	 * @param identification the Identification to delete
	 *
	 * @throws std::invalid_argument if **identification** is not an
	 *         Identification for an Ant of this Experiment.
	 */
	void DeleteIdentification(const Identification::Ptr &identification);

	/**
	 * Computes a valid time range for a tagID.
	 *
	 * @param tagID the TagID we want a range for
	 * @param time the Time that must be included in the result time range
	 *
	 * Queries for a valid time range for a given TagID and Time. The
	 * result will be a range [start,end[ containing
	 * **time**. In this range **tagID** is not used to identify any
	 * Ant.
	 *
	 * @return two Time that represents an available [start,end[ range
	 *         for **tagID**
	 *
	 * @throws std::runtime_error if **tagID** already identifies an
	 *         Ant at **time**.
	 */
	std::tuple<fort::Time, fort::Time>
	FreeIdentificationRangeAt(TagID tagID, const Time &time) const;

	/**
	 * The name of the Experiment.
	 *
	 * @return a reference to the Experiment's name
	 */
	const std::string &Name() const;

	/**
	 * Sets the Experiment's name.
	 *
	 * @param name the new Experiment name
	 */
	void SetName(const std::string &name);

	/**
	 * The author of the Experiment
	 *
	 * @return a reference to the Experiment's author name
	 */
	const std::string &Author() const;

	/**
	 * Sets the Experiment's author
	 *
	 * @param author the new value for the Experiment's author
	 */
	void SetAuthor(const std::string &author);

	/**
	 * Comments about the experiment
	 *
	 * @return a reference to the Experiment's comment
	 */
	const std::string &Comment() const;

	/**
	 * Sets the comment of the Experiment
	 *
	 * @param comment the wanted Experiment's comment
	 */
	void SetComment(const std::string &comment);

	/**
	 * The kind of tag used in the Experiment
	 *
	 * Gets the family of the tags used in this Experiment. It is
	 * automatically determined from the information in
	 * TrackingDataDirectory.
	 *
	 * @return the family of tag used in the Experiment
	 */
	fort::tags::Family Family() const;

	/**
	 * The default physical tag size
	 *
	 * Usually an Ant colony are tagged with a majority of tag of a
	 * given size in millimeters. Some individuals (like Queens) may
	 * often use a bigger tag size that should individually be set in
	 * their Identification. This value is then used for
	 * Query::ComputeMeasurementFor.
	 *
	 * **fort-myrmidon** uses without white border convention for
	 * ARTag and with white border convention Apriltag.
	 *
	 * @return the default tag size for the Experiment in millimeters
	 */
	double DefaultTagSize() const;

	/**
	 * Sets the default tag siye in mm
	 *
	 * @param defaultTagSize the tag size in millimeter ( the one
	 *        defined on the tag sheet )
	 *
	 */
	void SetDefaultTagSize(double defaultTagSize);

	/**
	 * Creates a measurement type
	 *
	 * @param name the wanted name for the new measurement
	 *
	 * @return the MeasurementTypeID identifying the new measurement
	 *         type
	 */
	MeasurementTypeID CreateMeasurementType(const std::string &name);

	/**
	 * Deletes a measurement type
	 *
	 * @param measurementTypeID the MeasurementTypeID to delete
	 *
	 * @throws std::out_of_range if **measurementTypeID** is not valid
	 *         for this Experiment.
	 * @throws std::invalid_argument if **measurementTypeID** is
	 *         HEAD_TAIL_MEASUREMENT_TYPE_ID.
	 * @throws std::runtime_error if some measurement for
	 *         **measurementTypeID** exists in the Experiment.
	 */
	void DeleteMeasurementType(MeasurementTypeID measurementTypeID);

	/**
	 * Sets the name of a measurement type
	 *
	 * @param measurementTypeID the MeasurementTypeID to modify
	 * @param name the wanted name
	 *
	 * @throws std::out_of_range if **measurementTypeID** is not valid
	 *         for this Experiment.
	 */
	void SetMeasurementTypeName(
	    MeasurementTypeID measurementTypeID, const std::string &name
	);

	/**
	 * Gets the Experiment defined measurement types
	 *
	 * @return a map of measurement type name by their
	 *         MeasurementTypeID
	 */
	std::map<MeasurementTypeID, std::string> MeasurementTypeNames() const;

	/**
	 * Creates a new Ant shape type
	 *
	 * @param name the user defined name for the Ant Shape Type
	 *
	 * @return the AntShapeTypeID identifying the new Ant shape type.
	 */
	AntShapeTypeID CreateAntShapeType(const std::string &name);

	/**
	 * Gets the defined Ant shape type
	 *
	 * @return the Ant shape type name by their AntShapeTypeID
	 */
	std::map<AntShapeTypeID, std::string> AntShapeTypeNames() const;

	/**
	 * Changes the name of an Ant Shape type
	 *
	 * @param shapeTypeID the AntShapeTypeID of the shape type to rename
	 * @param name param the new name for the Ant shape type
	 *
	 * @throws std::out_of_range if **shapeTypeID** is not valid for
	 *         this Experiment.
	 */
	void
	SetAntShapeTypeName(AntShapeTypeID shapeTypeID, const std::string &name);

	/**
	 * Removes a virtual Ant shape type
	 *
	 * @param shapeTypeID the AntShapeTypeID of the shape type to remove
	 *
	 * @throws std::out_of_range if **shapeTypeID** is not valid for
	 *         this Experiment.
	 * @throws std::runtime_error if at least one Ant still have a
	 *         Capsule for **shapeTypeID**
	 */
	void DeleteAntShapeType(AntShapeTypeID shapeTypeID);

	/**
	 * Adds or modify a user-defined meta data key.
	 *
	 * @param key the unique key to add or modify
	 * @param defaultValue the default value for that key. It also
	 *        determines the type for the key.
	 *
	 * Adds a non-tracking metadata **key** with type and
	 * default defined by **defaultValue**.
	 *
	 * @throws std::runtime_error if the following conditions are met:
	 *         * **key** is already registered
	 *         * **defaultValue** would change the type of key
	 *         * at least one Ant has a value registered for **key**
	 */
	void SetMetaDataKey(const std::string &key, Value defaultValue);

	/**
	 * Removes a meta data key.
	 *
	 * @param key the key to remove
	 *
	 * @throws std::out_of_range if **key** is not valid for this
	 *         Experiment.
	 * @throws std::runtime_error if at least one Ant has a defined
	 *         value for **key**.
	 */
	void DeleteMetaDataKey(const std::string &key);

	/**
	 * Gets the meta data keys for this Experiment
	 *
	 * @return default Value indexed by key
	 */
	std::map<std::string, Value> MetaDataKeys() const;

	/**
	 * Renames a meta data key
	 *
	 * @param oldKey the key to rename
	 * @param newKey the new key name
	 *
	 *
	 * @throws std::out_of_range if **oldKey** is not valid for this
	 *         Experiment.
	 * @throws std::invalid_argument if **newKey** is already used in this
	 *         Experiment.
	 */
	void
	RenameMetaDataKey(const std::string &oldKey, const std::string &newKey);

	/**
	 * Gets AntID <-> TagID correspondances at a given time
	 *
	 * @param time the wanted Time to query for the correspondances
	 * @param removeUnidentifiedAnt if `true`, just do not report
	 *        unidentified at this time. If `false`
	 *        std::numeric_limits<TagID>::max() will be returned as
	 *        a TagID for unidentified Ant.
	 *
	 * @return a map with the correspondance between AntID and TagID.
	 */
	std::map<AntID, TagID>
	IdentificationsAt(const Time &time, bool removeUnidentifiedAnt) const;

	/**
	 * Compiles a TrackingSolver
	 *
	 * Compiles a TrackingSolver, typically use to identify and
	 * collide frame from online acquired tracking data.
	 *
	 * @return a TrackingSolver for the Experiment.
	 */
	TrackingSolver::Ptr CompileTrackingSolver(bool collisionsIgnoreZones) const;

	/**
	 * Ensures that all tracking data is loaded.
	 *
	 * This operation can take some time.
	 *
	 * @param progressCallback a callback to indicate progress. Only
	 *        called if an operation is actually performed.
	 * @param fixCorruptedData if \true, will try to silently fix
	 *        corrupted data, leading to potential loss of large
	 *        chunck of tracking data or a few tag
	 *        close-ups. Otherwise an exception will be thrown in case
	 *        of corruption.
	 *
	 * @throws std::runtime_error in case of data corruption if
	 *         fixCorruptedData is \false
	 */
	void EnsureAllDataIsLoaded(
	    ProgressReporter::Ptr &&progressCallback, bool fixCorruptedData = false
	) const;

	~Experiment();

private:
	friend class Query;

	// Private implementation constructor
	// @pExperiment opaque pointer to implementation
	//
	// User cannot create an Experiment directly. They must use
	// <Open>, <OpenReadOnly>, <Create> and <NewFile>.
	Experiment(std::unique_ptr<ExperimentHandle> handle);

	Experiment &operator=(const Experiment &) = delete;
	Experiment(const Experiment &)            = delete;

	std::unique_ptr<ExperimentHandle> d_p;
};

} // namespace myrmidon
} // namespace fort
