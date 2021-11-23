#pragma once

#include <fort/myrmidon/Query.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

class Experiment;

class Query {
public:

	static void ComputeTagStatistics(const Experiment & experiment,
	                                 TagStatistics::ByTagID & result);

	static void IdentifyFrames(const Experiment & experiment,
	                           std::function<void (const IdentifiedFrame::Ptr &)> storeData,
	                           const myrmidon::Query::IdentifyFramesArgs & args);

	static void CollideFrames(const Experiment & experiment,
	                          std::function<void (const CollisionData & data) > storeData,
	                          const myrmidon::Query::CollideFramesArgs & args);

	static void ComputeTrajectories(const Experiment & experiment,
	                                std::function<void (const AntTrajectory::Ptr &)> storeData,
	                                const myrmidon::Query::ComputeAntTrajectoriesArgs & args);


	// computes trajectories and interactions. Bad invariant
	// optimization: interactions will always be saved before
	// trajectories. But there are no test.
	static void ComputeAntInteractions(const Experiment & experiment,
	                                   std::function<void (const AntTrajectory::Ptr &)> storeTrajectory,
	                                   std::function<void (const AntInteraction::Ptr &)> storeInteraction,
	                                   const myrmidon::Query::ComputeAntInteractionsArgs & args);

	static void  FindMovieSegments(const Experiment & experiment,
	                               std::vector<MovieSegmentData> & segments,
	                               SpaceID space,
	                               const Time & start,
	                               const Time & end);
private:

};


} // namespace priv
} // namespace myrmidon
} // namespace fort
