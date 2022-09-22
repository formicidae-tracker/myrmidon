#pragma once

#include <memory>
#include <vector>

#include <Eigen/Core>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/Typedefs.hpp>

namespace fort {
namespace myrmidon {
class Identification;
class Experiment;

// Formats an Identification to an std::ostream
// @out the stream to format to
// @identification the <fort::myrmidon::Identification> to format
//
// @return a reference to <out>
std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Identification & identification);

}
}



namespace fort {
namespace myrmidon {

namespace priv {
class Identification;
}

class IdentificationHandle;

/**
 * Relates tags to Ant.
 *
 * An Identification relates a ::TagID to an Ant, with Time validity
 * data and geometric data.
 *
 * @note An Identification can only be created from an Experiment with
 * Experiment::AddIdentification.
 *
 * Time validy
 * ===========
 *
 * Identification are bounded in Time in the range
 * [Start(),End()[. Start() can be Time::SinceEver() and End() can be
 * Time::ForEver(). These value are modifiable with SetStart() and
 * SetEnd().
 *
 * Internally `fort-myrmidon` ensure time validity of
 * Identification. It means that:
 *
 * * Two Identification using the same TagValue cannot overlap in Time.
 * * Two Identification pointing to the same Ant cannot overlap in Time.
 *
 * Pose information
 * ================
 *
 * Identification also contains geometric information on how the
 * detected tag is related to the observed Ant. These are the
 * translation and rotation of the Ant, in the tag coordinate reference.
 *
 * Usually, this information is automatically generated from the manual
 * measurement #HEAD_TAIL_MEASUREMENT_TYPE made in
 * `fort-studio`. Alternatively, users can override this behavior by
 * setting themselves the relative pose using
 * SetUserDefinedAntPose(). ClearUserDefinedAntPose() can be used to
 * revert to the internally computed pose.
 *
 * \note Any angle is measured in radians, with a standard
 * mathematical convention. Since in image processing the y-axis is
 * pointing from the top of the image to the bottom, positive angles
 * appears clockwise.
 *
 * Tag Size
 * ========
 *
 * Identifications also contains the information of the physical tag
 * size used to identify the individual. It can be accessed and set
 * with TagSize and SetTagSize. The value DEFAULT_TAG_SIZE (i.e. 0.0)
 * indicates that the Experiment::DefaultTagSize should be
 * used. Therefore, for most Ant, this field should be kept to
 * DEFAULT_TAG_SIZE, appart for a few individuals, for examples,
 * Queens.
 */
class Identification {
public:
	/** A pointer to an Identification */
	typedef std::shared_ptr<Identification>       Ptr;

	/**
	 *  Gets the TagID of this Identification
	 *
	 * The associated TagID of an Identification is immuable.
	 *
	 * @return the TagID used by this Identification
	 */
	TagID TagValue() const;

	/*
	 * Gets the AntID of the targeted Ant
	 *
	 * The targeted Ant of an Identification is immuable.
	 *
	 * @return the ::AntID of the targetted Ant
	 */
	AntID TargetAntID() const;

	/*
	 * Sets the starting validity Time for this Identification
	 *
	 * @param start the starting Time. It can be Time::SinceEver().
	 *
	 * Sets the starting validity Time for this Identification,
	 * i.e. the first Time this Identification is valid
	 * (Identification are valid for [Start(),End()[)
	 *
	 * @throws OverlappingIdentification if start will make two
	 *         Identification overlap in Time.
	 */
	void SetStart(const Time & start);

	/**
	 * Sets the ending validity time for this Identification
	 *
	 * @param end the ending Time. It can be Time::Forever().
	 *
	 * Sets the ending validity Time for this Identification, i.e. the
	 * first Time where this Identification becomes invalid
	 * (Identification are valid for [Start(),End()[).
	 *
	 * @throws OverlappingIdentification if end will make two
	 *         Identification overlap in Time.
	 *
	 */
	void SetEnd(const Time & end);

	/**
	 * Gets the starting validity time
	 *
	 * First Time where this Identification becomes valid.
	 * @return the Time where where this Identification becomes
	 *         valid. It can return Time::SinceEver()
	 */
	const Time & Start() const;

	/**
	 * Gets the ending validity time
	 *
	 * First Time where this Identification becomes unvalid.
	 * @return the first Time where this Identification becomes
	 *         unvalid. It can return Time::Forever()
	 */
	const Time & End() const;

	/**
	 * Gets the Ant center position relatively to the tag center.
	 *
	 * Gets the Ant center position relatively to the tag center. This offset
	 * is expressed in the tag reference frame.
	 *
	 * @return an Eigen::Vector2d of the Ant center relative to
	 *         the tag center.
	 */
	Eigen::Vector2d AntPosition() const;

	/**
	 * Gets the Ant angle relatively to the tag rotation
	 *
	 * Gets the Ant position relatively to the tag center. This offset
	 * is expressed in the tag reference frame.
	 *
	 * \note Angles use standard mathematical orientation. One has to
	 * remember that the y-axis in image processing is pointing from
	 * top to bottom, so when looking at the image, positive angle are
	 * clockwise, which is the opposite of most mathematical drawing
	 * when y is pointing from bottom to top.
	 *
	 * @return the angle in radian between the tag orientation and the
	 *         ant orientation.
	 */
	double AntAngle() const;


	/**
	 * Indicates if Identification has a user defined pose.
	 *
	 *
	 * @return `true` if the Identification has a user defined pose
	 *         set with SetUserDefinedAntPose()
	 */
	bool HasUserDefinedAntPose() const;

	/**
	 * Sets a user defined Ant pose.
	 *
	 * @param antPosition the offset, from the tag center to the Ant
	 *                    center, expressed in the tag reference frame.
	 * @param antAngle the Ant angle, relative to the tag angle.
	 *
	 */
	void SetUserDefinedAntPose(const Eigen::Vector2d & antPosition, double antAngle);

	/**
	 * Clears any user defined pose.
	 *
	 * Clears any user defined pose for this
	 * Identification. `fort-myrmidon` will re-compute the Ant pose
	 * from #HEAD_TAIL_MEASUREMENT_TYPE measurement made in
	 * `fort-studio`.
	 */
	void ClearUserDefinedAntPose();

	/**
	 * Value use to mark the Identification to use the
	 * Experiment::DefaultTagSize()
	 */
	static const double DEFAULT_TAG_SIZE;

	/**
	 * Sets the tag size for this Identification
	 *
	 * @param size the tag size in millimeters for this
	 *        Identification. Can be Identification::DEFAULT_TAG_SIZE
	 *        to force the use of the Experiment::DefaultTagSize().
	 */
	void SetTagSize(double size);

	/**
	 * Gets the tag size for this Identification
	 *
	 * @return size the tag size in millimeters for this
	 *        Identification. If equal to Identification::DEFAULT_TAG_SIZE
	 *        the Experiment::DefaultTagSize() is used instead.
	 */
	double TagSize() const;


	/**
	 * Checks if Experiment::DefaultTagSize() is used.
	 *
	 * @return `true` if this Identification uses
	 *         Experiment::DefaultTagSize()
	 */
	bool HasDefaultTagSize() const;

	~Identification();

private:
	friend class AntHandle;
	friend class ExperimentHandle;
	friend std::ostream & operator<<(std::ostream &, const Identification&);



	// Private implementation constructor
	// @pptr opaque pointer to implementation
	//
	// User cannot build Identification directly. They must be build
	// from <Experiment> and accessed from <Ant>
	Identification(std::unique_ptr<IdentificationHandle> handle);

	Identification & operator= (const Identification &) = delete;
	Identification(const Identification &) = delete;

	std::unique_ptr<IdentificationHandle> d_p;

};

/**
 *  Exception when two Identification overlaps in time.
 *
 * Two Identification overlaps in time if they have overlapping
 * boundary and they either use the same TagID or targets the same
 * Ant. This is an invariant condition that should never happen and
 * modification that will break this invariant will throw this
 * exception.
 */
class OverlappingIdentification : public std::runtime_error {
public:
	// Default constructor
	// @a the first overlapping identification
	// @b the second overlapping identification
	OverlappingIdentification(const priv::Identification & a,
	                          const priv::Identification & b) noexcept;

	// virtual destructor
	virtual ~OverlappingIdentification();
private:
	static std::string Reason(const priv::Identification & a,
	                          const priv::Identification & b) noexcept;
};



} // namespace fort
} // namespace myrmidon
