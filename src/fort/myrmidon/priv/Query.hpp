#pragma once

#include <fort/myrmidon/Query.hpp>
#include "TrackingDataDirectory.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class Experiment;

class Query {
public:
	static void ComputeTagStatistics(
	    const Experiment       &experiment,
	    TagStatistics::ByTagID &result,
	    ProgressReporter::Ptr &&progress,
	    bool                    fixCorruptedData
	);

	static void IdentifyFrames(
	    const Experiment	                             &experiment,
	    std::function<void(const IdentifiedFrame::Ptr &)> storeData,
	    const myrmidon::Query::IdentifyFramesArgs        &args
	);

	static void CollideFrames(
	    const Experiment	                          &experiment,
	    std::function<void(const CollisionData &data)> storeData,
	    const myrmidon::Query::CollideFramesArgs      &args
	);

	static void ComputeTrajectories(
	    const Experiment	                              &experiment,
	    std::function<void(const AntTrajectory::Ptr &)>    storeData,
	    const myrmidon::Query::ComputeAntTrajectoriesArgs &args
	);

	// computes trajectories and interactions. Bad invariant
	// optimization: interactions will always be saved before
	// trajectories. But there are no test.
	static void ComputeAntInteractions(
	    const Experiment	                              &experiment,
	    std::function<void(const AntTrajectory::Ptr &)>    storeTrajectory,
	    std::function<void(const AntInteraction::Ptr &)>   storeInteraction,
	    const myrmidon::Query::ComputeAntInteractionsArgs &args
	);

	static void FindVideoSegments(
	    const Experiment          &experiment,
	    std::vector<VideoSegment> &segments,
	    SpaceID                    space,
	    const Time                &start,
	    const Time                &end
	);

	static std::
	    tuple<std::vector<std::string>, std::vector<TagID>, Eigen::MatrixXd>
	    GetTagCloseUps(
	        const Experiment       &e,
	        ProgressReporter::Ptr &&progressCallback,
	        bool                    fixCorruptedData
	    );

	static void ProcessLoaders(
	    const std::vector<TrackingDataDirectory::Loader> &loaders,
	    ProgressReporter::Ptr                           &&progress,
	    bool                                              fixCorruptedData
	);

private:
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
