#pragma once

#include <variant>
#include <map>
#include <vector>

#include <Eigen/Core>

#include <fort/time/Time.hpp>


#include "ForwardDeclaration.hpp"

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
 * The ID for a tag
 *
 * The identifier for a tag, which relates to Ant using
 * Identification.
 */
typedef uint32_t TagID;


/**
 * The ID for an Ant.
 *
 * Ant are uniquely identified within an Experiment with an AntID,
 * which is at least `1`. `0` is an invalid AntID.
 */
typedef uint32_t AntID;

/**
 * The ID for a Space.
 *
 * Space are uniquely identified within an Experiment with a SpaceID,
 * which is at least `1`. `0` is an invalid SpaceID.
 */
typedef uint32_t SpaceID;

/**
 * The ID for a Zone.
 *
 * Zone are uniquely identified within an Experiment with a ZoneID, which is
 * at least `1`. `0` is an invalid/undefined Zone.
 */
typedef uint32_t ZoneID;

/**
 * C++ type for named values.
 *
 * A c++ type that can hold only one of any #AntMetaDataType.
 */
typedef std::variant<bool,int32_t,double,std::string,Time> AntStaticValue;

/**
 * A List of 2D Vector.
 *
 */
typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d>> Vector2dList;


class ZoneDefinition;
/**
 * A List of ZoneDefinition
 */
typedef std::vector<std::shared_ptr<ZoneDefinition>> ZoneDefinitionList;

class Zone;
typedef std::map<ZoneID,std::shared_ptr<Zone>> ZoneByID;

class Space;
typedef std::map<SpaceID,std::shared_ptr<Space>> SpaceByID;

class Ant;
typedef std::map<AntID,std::shared_ptr<Ant>> AntByID;

class Identification;
typedef std::vector<std::shared_ptr<Identification>> IdentificationList;

/**
 * The ID for Ant virtual body parts
 *
 * Uniquely identifies an Ant shape type in an Experiment, from
 * `1`. `0` is an invalid value.
 */
typedef uint32_t AntShapeTypeID;


/**
 * The ID for Ant manual measurement types
 *
 * Uniquely identifies an Ant measurement type in an Experiment, from
 * `1`. `0` is an invalid value. The value `1` always refers to the
 * valid MeasurementTypeID #HEAD_TAIL_MEASUREMENT_TYPE.
 */
typedef uint32_t MeasurementTypeID;

/**
 * The head-tail Measurement type.
 *
 * This Measurement type is always define for any Experiment and
 * cannot be deleted. However, it can be renamed.
 */
const MeasurementTypeID HEAD_TAIL_MEASUREMENT_TYPE = 1;

/** A list of Ant virtual shape part
 *
 */
typedef std::vector<std::pair<AntShapeTypeID,std::shared_ptr<Capsule>>> TypedCapsuleList;

/**
 * AntMetaDataType enumerates possible type for AntStaticValue
 *
 */
enum class AntMetaDataType {
                            /**
                             * A boolean
                             */
                            BOOL = 0,
                            /**
                             * An integer
                             */
                            INT,
                            /**
                             * a float
                             */
                            DOUBLE,
                            /**
                             * a std::string
                             */
                            STRING,
                            /**
                             * a Time
                             */
                            TIME,
};

AntMetaDataType TypeForAntStaticValue(const AntStaticValue & value);

/**
 * Represents a Measurement in millimeters at a given Time.
 *
 * Measurement in myrmidon are automatically converted to MM given the
 * Experiment tag family and size, and the size of the tag measured
 * in the image.
 */
struct ComputedMeasurement {
	/**
	 * A list of measurement
	 */
	typedef std::vector<ComputedMeasurement> List;
	/**
	 * the Time of the Measurement
	 */
	fort::Time Time;
	/**
	 * the value in mm of the measurement
	 */
	double     LengthMM;
	/**
	 * the value of the measurement in pixels
	 */
	double     LengthPixel;
};
/**
 * Statistics about a TagID in the experiment.
 */
struct TagStatistics {
	/** A map of TagStatistics indexed by TagID */
	typedef std::map<TagID,TagStatistics>   ByTagID;
	/** A vector of count. */
	typedef Eigen::Matrix<uint64_t,Eigen::Dynamic,1> CountVector;
	// Designating each index of <CountVector>
	enum CountHeader {
	    // Number of time the <TagID> was seen in the <Experiment>
		TOTAL_SEEN        = 0,
		// Number of time the <TagID> was seen multiple time in the same Frame.
		MULTIPLE_SEEN     = 1,
		// Number of time their was a gap less than 500 milliseconds were the tracking was lost.
		GAP_500MS         = 2,
		// Number of time their was a gap less than 1 second were the tracking was lost.
		GAP_1S            = 3,
		// Number of time their was a gap less than 10 seconds were the tracking was lost.
		GAP_10S           = 4,
		// Number of times their was a gap less than 1 minute were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_1M            = 5,
		// Number of times their was a gap less than 10 minutes were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_10M           = 6,
		// Number of times their was a gap less than 1 hour were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_1H            = 7,
		// Number of times their was a gap less than 10 hours were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_10H           = 8,
		// Number of times their was a gap of more than 10 hours were the tracking was lost. If using multiple space in an experiment, consider only smaller gap, and add all columns from <GAP_1M> up to this one to consider only gap bigger than 10S.
		GAP_MORE          = 9,
	};

	// The <TagID> this statistics refers too
	TagID       ID;
	// The first <Time> in the <Experiment> this <TagID> was detected.
	Time        FirstSeen;
	// The last <Time> in the <Experiment> this <TagID> was detected.
	Time        LastSeen;
	// Counts were the tag was seen
	CountVector Counts;
};

/**
 * A video frame were Ant have been identified from their TagID
 */
struct IdentifiedFrame {
	/**
	 * A pointer to an IdentifiedFrame
	 */
	typedef std::shared_ptr<IdentifiedFrame>       Ptr;

	/**
	 * A Matrix of ant position.
	 *
	 * * The first column is the AntID
	 * * The second and third columns are the x,y position in the original video frame.
	 * * The fourth column is the ant angle
	 * * The fifth column is the ant current zone or 0 if in no zone or zones aren't computed.
	 */
	typedef Eigen::Matrix<double,Eigen::Dynamic,5,Eigen::RowMajor> PositionMatrix;

	/**
	 * The acquisition Time of this frame
	 */
	Time              FrameTime;
	/**
	 *  The Space this frame belongs to.
	 */
	SpaceID           Space;
	/**
	 *  The original height of the video frame
	 */
	size_t            Height;
	/**
	 *  The original width of the video frame
	 */
	size_t            Width;
	/**
	 * The position of each ant in that frame
	 */
	PositionMatrix    Positions;

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
	 * @return the AntID, x,y,angle position and the ZoneID for the
	 *         Ant at index.
	 */
	std::tuple<AntID,const Eigen::Ref<const Eigen::Vector3d>,ZoneID> At(size_t index) const;
};

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

};

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
};

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

};

/**
 * Reports information about a tracking data directory.
 */
struct TrackingDataDirectoryInfo {
	/**
	 * The URI used in the GUI to designate the tracking data directory
	 */
	std::string URI;
	/**
	 * The absolute filepath on the directory on the system
	 */
	std::string AbsoluteFilePath;
	/**
	 * The number of Frames in this directory
	 */
	uint64_t    Frames;
	/**
	 * The first frame Time
	 */
	Time        Start;
	/**
	 * The last frame Time
	 */
	Time        End;
};

/**
 * Reports global tracking data stats for a Space
 */
struct SpaceDataInfo {
	/**
	 * The URI used to designate the Space
	 */
	std::string URI;
	/**
	 * The name of the Space
	 */
	std::string Name;
	/**
	 * The number of frame in the Space
	 */
	uint64_t    Frames;
	/**
	 * The first Time present in the Space
	 */
	Time        Start;
	/**
	 * The last Time present in the Space
	 */
	Time        End;
	/**
	 * Infos for all tracking data directories, ordered in Time
	 */
	std::vector<TrackingDataDirectoryInfo> TrackingDataDirectories;
};

/**
 * Reports global tracking data stats for an Experiment
 */
struct ExperimentDataInfo {
	/**
	 * The number of tracked frame in the Experiment
	 */
	uint64_t Frames;
	/**
	 * The Time of the first tracked frame
	 */
	Time     Start;
	/**
	 * the Time of the last tracked frame
	 */
	Time     End;

	/**
	 * Data infos for all Space
	 */
	std::map<SpaceID,SpaceDataInfo> Spaces;
};


struct MovieSegmentData {
	fort::Time Start;
	std::string AbsoluteFilePath;
	size_t BeginFramePosition,EndFramePosition;
	std::vector<fort::Duration> FrameTimeOffsets;
};

/**
 * Formats a TagID to conventional format
 * @param tagID the TagID to format
 *
 * @return tagID formatted to the myrmidon convetion for TagID.
 */
std::string FormatTagID(TagID tagID);

/**
 * Formats a AntID to conventional format
 * @param antID the TagID to format
 *
 * @return antID formatted to the myrmidon convention for AntID.
 */
std::string FormatAntID(AntID antID);


typedef Eigen::AlignedBox<double,2> AABB;
}
}

/**
 * C++ Formatting operator for AntStaticValue
 * @param out the std::ostream to format the value to
 * @param v the fort::myrmidon::AntStaticValue to format
 *
 * @return a reference to out
 */
std::ostream & operator<<(std::ostream & out, const fort::myrmidon::AntStaticValue & v);
