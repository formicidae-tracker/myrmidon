#pragma once

#include <memory>
#include <vector>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/AntInteraction.hpp>
#include <fort/myrmidon/types/AntTrajectory.hpp>
#include <fort/myrmidon/types/Collision.hpp>
#include <fort/myrmidon/types/ComputedMeasurement.hpp>
#include <fort/myrmidon/types/ExperimentDataInfo.hpp>
#include <fort/myrmidon/types/IdentifiedFrame.hpp>
#include <fort/myrmidon/types/Reporter.hpp>
#include <fort/myrmidon/types/TagStatistics.hpp>
#include <fort/myrmidon/types/Value.hpp>

namespace fort {
namespace myrmidon {

class Experiment;
class Matcher;
class VideoSegment;

/**
 * Queries are computation on an Experiment tracking data.
 *
 * This class is a wrapper for all data queries that can be made on an
 * Experiment. They takes advantages of multithreading to have
 * efficient computation time.
 *
 * @note Some queries have two version, one returing up to two
 * std::vector, and one with a storeMethod parameter. The later is to
 * be used with bindings, or when a different data structure is
 * needed.
 *
 * @note For very small Experiment in number of Ant, most of the query
 * operation are IO bounded, and the multithreading overhead will
 * impact performance by 40-50% in computation time, as threads are
 * waiting for data inpout to come. This is the case for
 * IdentifyFrames(), and ComputeAntTrajectories(). When collision
 * detection is required, multi-threading can largely help, especially
 * with a large number of individuals. Threading is controlled with
 * QueryArgs::SingleThreaded
 *
 */
class Query {
public:
	/**
	 * Computes all measurement for an Ant
	 * @param experiment the Experiment to query for
	 * @param antID the desired Ant
	 * @param typeID the desired measurement type
	 * @return a ComputedMeasurement::List of the Measurement for the Ant
	 */
	static ComputedMeasurement::List ComputeMeasurementFor(
	    const Experiment &experiment, AntID antID, MeasurementTypeID typeID
	);

	/**
	 * Gets tracking data statistics about the Experiment
	 *
	 * @return an ExperimentDataInfo structure of informations
	 */
	static ExperimentDataInfo GetDataInformations(const Experiment &experiment);

	/**
	 * Arguments for any Query
	 */
	struct QueryArgs {
		//! First Time to consider (default: Time::SinceEver())
		Time                      Start          = Time::SinceEver();
		//! Last  Time to consider (default: Time::Forever())
		Time                      End            = Time::Forever();
		//! Uses a single thread for computation (default: false)
		bool                      SingleThreaded = false;
		//!  A TimeProgressReporter to report progress and errors
		TimeProgressReporter::Ptr Progress;
	};

	/**
	 * Computes TagStatistics for an experiment
	 * @param experiment the Experiment to query for
	 * @param fixCorruptedData a boolean indicating if data corruption
	 *        should be ignored and silently fixed. Please note that
	 *        it could cause the loss of large chunk of tracking
	 *        data. If false, an exception will be thrown.
	 * @param progress a ProgressReporter to report the progress
	 * @return the TagStatistics indexed by TagID
	 */
	static TagStatistics::ByTagID ComputeTagStatistics(
	    const Experiment       &experiment,
	    ProgressReporter::Ptr &&progress         = nullptr,
	    bool                    fixCorruptedData = false

	);

	/**
	 * Arguments for IdentifyFrames
	 *
	 * Arguments for IdentifyFrames() and IdentifyFramesFunctor().
	 */
	struct IdentifyFramesArgs : public QueryArgs {
		//! enables zone computation without collision detection
		bool ComputeZones = true;
	};

	/**
	 * Identifies ants in frames - functor version
	 * @param experiment the Experiment to query for
	 * @param storeData a functor to store/convert the data
	 * @param args the IdentifyFrameArgs to use for this query
	 *
	 * Identifies Ants in frames, data will be reported ordered by
	 * time.
	 *
	 * @note Zones for Ant will not be computed unless specified with
	 * IdentifyFrameArgs::ComputeZones.
	 *
	 * @note This version aimed to be used by language bindings to
	 * avoid large data copy.
	 */
	static void IdentifyFramesFunctor(
	    const Experiment	                             &experiment,
	    std::function<void(const IdentifiedFrame::Ptr &)> storeData,
	    const IdentifyFramesArgs                         &args
	);

	/**
	 * Identifies ants in frames
	 * @param experiment the Experiment to query for
	 * @param result the resulting IdentifiedFrame
	 * @param args the IdentifyFrameArgs to use for this query
	 *
	 * Identifies Ants in frames, data will be reported ordered by time.
	 *
	 * @note Zones for Ant will not be computed unless specified with
	 * IdentifyFrameArgs::ComputeZones.
	 */
	static void IdentifyFrames(
	    const Experiment                  &experiment,
	    std::vector<IdentifiedFrame::Ptr> &result,
	    const IdentifyFramesArgs          &args
	);

	/**
	 * Arguments for CollideFrames
	 *
	 * Arguments for CollideFrames() and CollideFramesFunctor().
	 */
	struct CollideFramesArgs : public QueryArgs {
		//! Collision detection happens over different zones (default: false).
		bool CollisionsIgnoreZones = false;
	};

	/**
	 * Finds Collision in tracking frames - functor version
	 * @param experiment the Experiment to query for
	 * @param storeData a functor to store the data as it is produced
	 * @param args the QueryArgs to use for this query
	 *
	 * Finds Collision between ants in frames, data will be reported
	 * ordered by time. Zones for each Ant will also be computed.
	 *
	 * @note This version aimed to be used by language bindings to
	 * avoid large data copy.
	 */
	static void CollideFramesFunctor(
	    const Experiment	                          &experiment,
	    std::function<void(const CollisionData &data)> storeData,
	    const CollideFramesArgs                       &args
	);

	/**
	 * Finds Collision in tracking frames
	 * @param experiment the Experiment to query for
	 * @param result the resulting IdentifiedFrame and CollisionFrame
	 * @param args the QueryArgs to use for this query
	 *
	 * Finds Collision between ants in frames, data will be reported
	 * ordered by time. Zones for each Ant will also be computed.
	 */
	static void CollideFrames(
	    const Experiment           &experiment,
	    std::vector<CollisionData> &result,
	    const CollideFramesArgs    &args
	);

	/**
	 * Arguments for ComputeAntTrajectories
	 *
	 * Arguments for ComputeAntTrajectories() and
	 * ComputeAntTrajectoriesFunctor().
	 */
	struct ComputeAntTrajectoriesArgs : public QueryArgs {
		//! Maximum Duration before considering the trajectory be two
		//! different parts (default: 1s)
		Duration MaximumGap = Duration::Second;

		//! Matcher to reduce the query to an Ant subset (default: to
		//! nullptr, i.e. anything).
		std::shared_ptr<myrmidon::Matcher> Matcher = nullptr;

		//! Computes the zone of each Ant (default: false)
		bool ComputeZones = false;

		//! If a combined matcher value changes, create a new object
		bool SegmentOnMatcherValueChange = false;
	};

	/**
	 * Computes trajectories for ants - functor version
	 * @param experiment the Experiment to query for
	 * @param storeTrajectory a functor to store/covert the data
	 * @param args the ComputeAntTrajectoriesArgs to use for this query
	 *
	 * Computes ant trajectories in the experiment. A trajectory is
	 * the consecutive position of an Ant in a Space, with detection
	 * gap under ComputeAntTrajectoriesArgs::MaximumGap. These will be
	 * reported ordered by ending time.
	 *
	 * @note This version aimed to be used by language bindings to
	 * avoid large data copy.
	 */
	static void ComputeAntTrajectoriesFunctor(
	    const Experiment	                           &experiment,
	    std::function<void(const AntTrajectory::Ptr &)> storeTrajectory,
	    const ComputeAntTrajectoriesArgs               &args
	);

	/**
	 * Computes trajectories for ants.
	 * @param experiment the Experiment to query for
	 * @param trajectories a container for storing the results.
	 * @param args the ComputeAntTrajectoriesArgs to use for this query
	 *
	 * Computes ant trajectories in the experiment. A trajectory is
	 * the consecutive position of an Ant in a Space, with detection
	 * gap under ComputeAntTrajectoriesArgs::MaximumGap. These will be
	 * reported ordered by ending time.
	 */
	static void ComputeAntTrajectories(
	    const Experiment                 &experiment,
	    std::vector<AntTrajectory::Ptr>  &trajectories,
	    const ComputeAntTrajectoriesArgs &args
	);

	/**
	 * Arguments for ComputeAntInteractions
	 *
	 * Arguments for ComputeAntInteractions() and
	 * ComputeAntInteractionsFunctor().
	 */
	struct ComputeAntInteractionsArgs : public QueryArgs {
		//! Maximum Duration before considering the trajectory be two
		//! different parts (default: 1s)
		Duration MaximumGap = Duration::Second;

		//! Matcher to reduce the query to an Ant subset (default: to
		//! nullptr, i.e. anything).
		std::shared_ptr<myrmidon::Matcher> Matcher = nullptr;

		//! Reports full trajectories. If false only mean trajectory
		//! during interactions will be reported, otherwise trajectory
		//! will be computed like ComputeAntTrajectories() and
		//! AntInteraction wil points to sub-segment (default: true).
		bool ReportFullTrajectories = true;

		//! Collisions, and therefore interactions, happens over
		//! different zones.
		bool CollisionsIgnoreZones = false;

		//! If a combined matcher value changes, create a new object
		bool SegmentOnMatcherValueChange = false;
	};

	/**
	 * Computes interactions for ants - functor version
	 * @param experiment the Experiment to query for
	 * @param storeTrajectory a functor to store/convert trajectories
	 * @param storeInteraction a functor to store/convert interaction
	 * @param args the ComputeAntInteractionsArgs
	 *
	 * Computes ant interactions, i.e. time intervals where two Ants
	 * collides. These will be reported ordered by ending time.
	 *
	 * The parameter
	 * ComputeAntInteractionsArgs::ReportFullTrajectories controls if
	 * full trajectories should be reported in AntTrajectorySegment or
	 * if only a summary should be reported. The former have an high
	 * impact on the amount of RAM required to perform the query
	 * efficiently or at all.
	 *
	 * @note This version aimed to be used by language bindings to
	 * avoid large data copy.
	 */
	static void ComputeAntInteractionsFunctor(
	    const Experiment	                            &experiment,
	    std::function<void(const AntTrajectory::Ptr &)>  storeTrajectory,
	    std::function<void(const AntInteraction::Ptr &)> storeInteraction,
	    const ComputeAntInteractionsArgs                &args
	);

	/**
	 * Computes interactions for ants
	 * @param experiment the Experiment to query for
	 * @param trajectories container for the AntTrajectory results
	 * @param interactions container for AntInteraction results
	 * @param args the ComputeAntInteractionsArgs
	 *
	 * Computes ant interactions, i.e. time intervals where two Ants
	 * collides. These will be reported ordered by ending time.
	 *
	 * The parameter
	 * ComputeAntInteractionsArgs::ReportFullTrajectories controls if
	 * full trajectories should be reported in AntTrajectorySegment or
	 * if only a summary should be reported. The former have an high
	 * impact on the amount of RAM required to perform the query
	 * efficiently or at all.
	 */
	static void ComputeAntInteractions(
	    const Experiment                 &experiment,
	    std::vector<AntTrajectory::Ptr>  &trajectories,
	    std::vector<AntInteraction::Ptr> &interactions,
	    const ComputeAntInteractionsArgs &args
	);

	/**
	 * Finds the VideoSegment in a Space of the Experiment
	 *
	 * @note it would be extremly computationally inefficient to query
	 * for a large range [start;end].
	 *
	 * @param experiment the Experiment to query for
	 * @param segments container for the VideoSegment results
	 * @param space the SpaceID of the Space to query for
	 * @param start the first Time to query a video frame for
	 * @param end the last Time to query a video frame for
	 */
	static void FindVideoSegments(
	    const Experiment          &experiment,
	    std::vector<VideoSegment> &segments,
	    SpaceID                    space,
	    const fort::Time          &start,
	    const fort::Time          &end
	);

	/**
	 * Gets the time ranges where metadata key has a given value.
	 *
	 * @param e the Experiment to query for
	 * @param key the key to query for
	 * @param value the value to check equality to
	 *
	 * @return a vector of tuple of AntID and time range where key is
	 * equal to value.
	 *
	 * @throws std::out_of_range if key is not defined in Experiment
	 * @throws std::invalid_argument if value is not of the right type for key
	 */

	static std::vector<std::tuple<AntID, Time, Time>> GetMetaDataKeyRanges(
	    const Experiment &e, const std::string &key, const Value &value
	);

	/**
	 * Gets the tag close-up in the experiment
	 *
	 * @param e the Experiment to query for
	 * @param progrssCallback a callback for the progress of the operation
	 * @param fixCorruptedData a boolean that will make data
	 *         corruption error silently ignored. Could lead to the
	 *         loss of some tag close-up
	 *
	 * @return a tuple of a vector of string, TagID, and an Eigen::Matrix
	 */
	static std::
	    tuple<std::vector<std::string>, std::vector<TagID>, Eigen::MatrixXd>
	    GetTagCloseUps(
	        const Experiment       &e,
	        ProgressReporter::Ptr &&progress,
	        bool                    fixCorruptedData = false
	    );
};

} // namespace myrmidon
} // namespace fort
