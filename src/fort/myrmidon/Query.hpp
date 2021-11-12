#pragma once

#include "Types.hpp"

#include "Experiment.hpp"
#include "Matchers.hpp"

namespace fort {
namespace myrmidon {


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

	// Data returned by <CollideFrames>.
	typedef std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr> CollisionData;

	/**
	 * Computes all measurement for an Ant
	 * @param experiment the Experiment to query for
	 * @param antID the desired Ant
	 * @param typeID the desired measurement type
	 * @return a ComputedMeasurement::List of the Measurement for the Ant
	 */
	static ComputedMeasurement::List ComputeMeasurementFor(const Experiment & experiment,
	                                                       AntID antID,
	                                                       MeasurementTypeID typeID);

	/**
	 * Gets tracking data statistics about the Experiment
	 *
	 * @return an ExperimentDataInfo structure of informations
	 */
	static ExperimentDataInfo GetDataInformations(const Experiment & experiment);


	/**
	 * Arguments for any Query
	 */
	struct QueryArgs {
		//! First Time to consider (default: Time::SinceEver())
		Time Start;
		//! Last  Time to consider (default: Time::Forever())
		Time End;
		//! Uses a single thread for computation (default: false)
		bool SingleThreaded;
		//! Forces result allocation from the calling thread (default: false)
		bool AllocationInCurrentThread;

		//! Builds default arguments
		QueryArgs();
	};


	/**
	 * Computes TagStatistics for an experiment
	 * @param experiment the Experiment to query for
	 * @return the TagStatistics indexed by TagID
	 */
	static TagStatistics::ByTagID ComputeTagStatistics(const Experiment & experiment);

	/**
	 * Arguments for IdentifyFrames
	 *
	 * Arguments for IdentifyFrames() and IdentifyFramesFunctor().
	 */
	struct IdentifyFramesArgs : public QueryArgs {
		//! enables zone computation without collision detection
		bool ComputeZones;

		// Builds default arguments
		IdentifyFramesArgs();
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
	static void IdentifyFramesFunctor(const Experiment & experiment,
	                                  std::function<void (const IdentifiedFrame::Ptr &)> storeData,
	                                  const IdentifyFramesArgs & args = IdentifyFramesArgs());


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
	static void IdentifyFrames(const Experiment & experiment,
	                           std::vector<IdentifiedFrame::Ptr> & result,
	                           const IdentifyFramesArgs & args = IdentifyFramesArgs() );

	/**
	 * Arguments for CollideFrames
	 *
	 * Arguments for CollideFrames() and CollideFramesFunctor().
	 */
	struct CollideFramesArgs : public QueryArgs {
		//! Collision detection happens over different zones (default: false).
		bool CollisionsIgnoreZones;

		// Builds default arguments
		CollideFramesArgs();
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
	static void CollideFramesFunctor(const Experiment & experiment,
	                                 std::function<void (const CollisionData & data)> storeData,
	                                 const CollideFramesArgs & args = CollideFramesArgs());


	/**
	 * Finds Collision in tracking frames
	 * @param experiment the Experiment to query for
	 * @param result the resulting IdentifiedFrame and CollisionFrame
	 * @param args the QueryArgs to use for this query
	 *
	 * Finds Collision between ants in frames, data will be reported
	 * ordered by time. Zones for each Ant will also be computed.
	 */
	static void CollideFrames(const Experiment & experiment,
	                          std::vector<CollisionData> & result,
	                          const CollideFramesArgs & args = CollideFramesArgs());


	/**
	 * Arguments for ComputeAntTrajectories
	 *
	 * Arguments for ComputeAntTrajectories() and
	 * ComputeAntTrajectoriesFunctor().
	 */
	struct ComputeAntTrajectoriesArgs : public QueryArgs {
		//! Maximum Duration before considering the trajectory be two
		//! different parts (default: 1s)
		Duration MaximumGap;
		//! Matcher to reduce the query to an Ant subset (default: to
		//! nullptr, i.e. anything).
		myrmidon::Matcher::Ptr Matcher;
		//! Computes the zone of each Ant (default: false)
		bool ComputeZones;

		// Builds default arguments
		ComputeAntTrajectoriesArgs();
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
	static void ComputeAntTrajectoriesFunctor(const Experiment & experiment,
	                                          std::function<void (const AntTrajectory::Ptr &)> storeTrajectory,
	                                          const ComputeAntTrajectoriesArgs & args = ComputeAntTrajectoriesArgs());



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
	static void ComputeAntTrajectories(const Experiment & experiment,
	                                   std::vector<AntTrajectory::Ptr> & trajectories,
	                                   const ComputeAntTrajectoriesArgs & args = ComputeAntTrajectoriesArgs());

	/**
	 * Arguments for ComputeAntInteractions
	 *
	 * Arguments for ComputeAntInteractions() and
	 * ComputeAntInteractionsFunctor().
	 */
	struct ComputeAntInteractionsArgs : public QueryArgs{
		//! Maximum Duration before considering the trajectory be two
		//! different parts (default: 1s)
		Duration MaximumGap;
		//! Matcher to reduce the query to an Ant subset (default: to
		//! nullptr, i.e. anything).
		myrmidon::Matcher::Ptr Matcher;

		//! Reports full trajectories. If false only mean trajectory
		//! during interactions will be reported, otherwise trajectory
		//! will be computed like ComputeAntTrajectories() and
		//! AntInteraction wil points to sub-segment (default: true).
		bool ReportFullTrajectories;

		//! Collisions, and therefore interactions, happens over
		//! different zones.
		bool CollisionsIgnoreZones;

		//! Builds default arguments
		ComputeAntInteractionsArgs();
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
	static void ComputeAntInteractionsFunctor(const Experiment & experiment,
	                                          std::function<void ( const AntTrajectory::Ptr&)> storeTrajectory,
	                                          std::function<void ( const AntInteraction::Ptr&)> storeInteraction,
	                                          const ComputeAntInteractionsArgs & args = ComputeAntInteractionsArgs());



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
	static void ComputeAntInteractions(const Experiment & experiment,
	                                   std::vector<AntTrajectory::Ptr> & trajectories,
	                                   std::vector<AntInteraction::Ptr> & interactions,
	                                   const ComputeAntInteractionsArgs & args = ComputeAntInteractionsArgs());


};


} // namespace myrmidon
} // namespace fort
