
#include "Query.hpp"

#include <thread>

#include <tbb/parallel_for.h>
#include <tbb/pipeline.h>

#include <fort/myrmidon/Matchers.hpp>
#include <fort/myrmidon/MovieSegmentData.hpp>

#include "TagStatistics.hpp"
#include "TrackingDataDirectory.hpp"
#include "Identifier.hpp"
#include "RawFrame.hpp"
#include "CollisionSolver.hpp"
#include "QueryRunner.hpp"
#include "DataSegmenter.hpp"
#include "Space.hpp"
#include "Experiment.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


static void EnsureTagStatisticsAreComputed(const Space & space) {
	std::vector<TrackingDataDirectory::Loader> loaders;
	for ( const auto & tdd : space.TrackingDataDirectories() ) {
		if ( tdd->TagStatisticsComputed() == true ) {
			continue;
		}
		auto localLoaders = tdd->PrepareTagStatisticsLoaders();
		loaders.insert(loaders.end(),localLoaders.begin(),localLoaders.end());
	}
	tbb::parallel_for(tbb::blocked_range<size_t>(0,loaders.size()),
		                  [&loaders](const tbb::blocked_range<size_t> & range) {
			                  for ( size_t idx = range.begin();
			                        idx != range.end();
			                        ++idx ) {
				                  loaders[idx]();
			                  }
		                  });
}

void Query::ComputeTagStatistics(const Experiment & experiment,TagStatistics::ByTagID & result) {
	std::vector<TagStatistics::ByTagID> allSpaceResult;

	typedef std::vector<TagStatisticsHelper::Loader> StatisticLoaderList;
	for ( const auto & [spaceID,space] : experiment.Spaces() ) {
		EnsureTagStatisticsAreComputed(*space);
		std::vector<TagStatisticsHelper::Timed> spaceResults;
		for ( const auto & tdd : space->TrackingDataDirectories() ) {
			spaceResults.push_back(tdd->TagStatistics());
		}
		allSpaceResult.push_back(TagStatisticsHelper::MergeTimed(spaceResults.begin(),spaceResults.end()).TagStats);
	}

	result = TagStatisticsHelper::MergeSpaced(allSpaceResult.begin(),allSpaceResult.end());
}


void Query::IdentifyFrames(const Experiment & experiment,
                           std::function<void (const IdentifiedFrame::Ptr &)> storeDataFunctor,
                           const myrmidon::Query::IdentifyFramesArgs & args) {

	auto runner = QueryRunner::RunnerFor(args.SingleThreaded == false,
										 args.AllocationInCurrentThread);
	runner(experiment,
		   {
			.Start = args.Start,
			.End = args.End,
			.Localize = args.ComputeZones,
			.Collide = false,
			.CollisionsIgnoreZones = false,
		   },
		   [=](const CollisionData & data) {
			   storeDataFunctor(data.first);
		   });
}

void Query::CollideFrames(const Experiment & experiment,
                          std::function<void (const CollisionData &)> storeDataFunctor,
                          const myrmidon::Query::CollideFramesArgs & args) {

	auto runner = QueryRunner::RunnerFor(args.SingleThreaded == false,
										 args.AllocationInCurrentThread);
	runner(experiment,
		   {
			.Start = args.Start,
			.End = args.End,
			.Localize = false,
			.Collide = true,
			.CollisionsIgnoreZones = args.CollisionsIgnoreZones,
		   },
		   [=](const CollisionData & data) {
			   storeDataFunctor(data);
		   });
}

void Query::ComputeTrajectories(const Experiment & experiment,
                                std::function<void (const AntTrajectory::Ptr &)> storeDataFunctor,
                                const myrmidon::Query::ComputeAntTrajectoriesArgs & args) {
	auto runner = QueryRunner::RunnerFor(args.SingleThreaded == false,
										 args.AllocationInCurrentThread);
	auto sargs = DataSegmenter::Args{
									  .StoreTrajectory = storeDataFunctor,
									  .StoreInteraction = [](const AntInteraction::Ptr &) {},
									  .MaximumGap = args.MaximumGap,
									  .SummarizeSegment = false,};
	if ( args.Matcher ) {
		sargs.Matcher = args.Matcher->ToPrivate();
	}

	auto segmenter = DataSegmenter(sargs);
	runner(experiment,
		   {
			.Start = args.Start,
			.End = args.End,
			.Localize = args.ComputeZones,
			.Collide = false,
			.CollisionsIgnoreZones = false,
		   },
		   [&segmenter](const CollisionData & data)  {
			   segmenter(data);
		   });
}

void Query::ComputeAntInteractions(const Experiment & experiment,
                                   std::function<void ( const AntTrajectory::Ptr &) > storeTrajectory,
                                   std::function<void ( const AntInteraction::Ptr &) > storeInteraction,
                                   const myrmidon::Query::ComputeAntInteractionsArgs & args) {

	auto runner = QueryRunner::RunnerFor(args.SingleThreaded == false,
										 args.AllocationInCurrentThread);
	DataSegmenter::Args sargs;
	sargs.MaximumGap = args.MaximumGap;
	if ( args.Matcher ) {
		sargs.Matcher = args.Matcher->ToPrivate();
	}
	if ( args.ReportFullTrajectories == true ) {
		sargs.SummarizeSegment = false;
		sargs.StoreTrajectory = storeTrajectory;
		sargs.StoreInteraction = storeInteraction;
	} else {
		sargs.SummarizeSegment = true;
		sargs.StoreTrajectory = [](const AntTrajectory::Ptr &){};
		sargs.StoreInteraction = storeInteraction;
	}
	auto segmenter = DataSegmenter(sargs);

	runner(experiment,
		   {
			.Start = args.Start,
			.End = args.End,
			.Localize = false,
			.Collide = true,
			.CollisionsIgnoreZones = args.CollisionsIgnoreZones,
		   },
		   [&segmenter](const CollisionData & data)  {
			   segmenter(data);
		   });
}

void Query::FindMovieSegment(const Experiment & experiment,
                             std::vector<MovieSegmentData> & segments,
                             SpaceID space,
                             const Time & start,
                             const Time & end) {

	segments.clear();

	if ( experiment.Spaces().count(space)) {
		return;
	}
	auto ranges = TrackingDataDirectory::IteratorRanges(experiment.Spaces().at(space)->TrackingDataDirectories(),
	                                                    start,end);

	for ( auto & [iter,end] : ranges ) {
		MovieSegment::ConstPtr segment;
		MovieFrameID lastMovieID;
		for ( ; iter != end; ++iter) {
			const auto & frame = *iter;
			auto frameID = frame->Frame().ID();
			if ( segment == nullptr
			     || segment->EndFrame() < frameID ) {
				segment = iter.LockParent()->MovieSegments().Find(frameID).second;
				lastMovieID = segment->ToMovieFrameID(frameID) - 1;
				segments.push_back(MovieSegmentData{.Space = space,
				                                    .AbsoluteFilePath = segment->AbsoluteFilePath });
			}
			while(lastMovieID < segment->ToMovieFrameID(frameID) ) {
				++lastMovieID;
				segments.back().Data.push_back(

			}



		}

	}
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
