
#include "Query.hpp"

#include <thread>

#include <tbb/concurrent_queue.h>
#include <tbb/parallel_for.h>

#include <fort/myrmidon/Matchers.hpp>
#include <fort/myrmidon/Video.hpp>

#include "CollisionSolver.hpp"
#include "DataSegmenter.hpp"
#include "Experiment.hpp"
#include "Identifier.hpp"
#include "QueryRunner.hpp"
#include "RawFrame.hpp"
#include "Space.hpp"
#include "TagCloseUp.hpp"
#include "TagStatistics.hpp"
#include "TrackingDataDirectory.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

void Query::ProcessLoaders(
    const std::vector<TrackingDataDirectory::Loader> &loaders,
    ProgressReporter::Ptr                           &&progress,
    bool                                              fixCorruptedData
) {
	if (loaders.empty() == true) {
		return;
	}
	tbb::concurrent_bounded_queue<std::shared_ptr<FixableError::Ptr>> queue;

	std::atomic<bool> stop;
	stop.store(false);

	class StopIteration {};

	std::thread go([&]() {
		try {
			tbb::parallel_for(
			    tbb::blocked_range<size_t>(0, loaders.size()),
			    [&](const tbb::blocked_range<size_t> &range) {
				    for (size_t idx = range.begin(); idx != range.end();
				         ++idx) {
					    FixableError::Ptr e;
					    if (stop.load() == true) {
						    throw StopIteration();
					    }
					    e = loaders[idx]();
					    queue.push(
					        std::make_shared<FixableError::Ptr>(std::move(e))
					    );
				    }
			    }
			);
		} catch (const StopIteration &) {
			return;
		}
		queue.push(nullptr);
	});

	FixableErrorList errors;
	int              i = 0;

	if (progress) {
		progress->SetTotal(loaders.size());
	}

	for (;;) {
		std::shared_ptr<FixableError::Ptr> error;
		queue.pop(error);
		if (error == nullptr) {
			break;
		}
		if (*error) {
			errors.push_back(std::move(*error));
		}
		try {
			if (progress) {
				progress->Update(++i);
			}
		} catch (const std::exception &e) {
			stop.store(true);
			go.join();
			throw;
		}
	}
	go.join();

	if (errors.empty() == true) {
		return;
	}
	if (fixCorruptedData == false) {
		throw FixableErrors(std::move(errors));
	}
	for (auto &e : errors) {
		e->Fix();
	}
}

static void EnsureTagStatisticsAreComputed(
    const Experiment       &experiment,
    ProgressReporter::Ptr &&progress,
    bool                    fixCorruptedData
) {
	std::vector<TrackingDataDirectory::Loader> loaders;
	for (const auto &[URI, tdd] : experiment.TrackingDataDirectories()) {
		if (tdd->TagStatisticsComputed() == true) {
			continue;
		}
		auto localLoaders = tdd->PrepareTagStatisticsLoaders();
		loaders.insert(loaders.end(), localLoaders.begin(), localLoaders.end());
	}

	Query::ProcessLoaders(loaders, std::move(progress), fixCorruptedData);
}

void Query::ComputeTagStatistics(
    const Experiment       &experiment,
    TagStatistics::ByTagID &result,
    ProgressReporter::Ptr &&progress,
    bool                    fixCorruptedData
) {
	EnsureTagStatisticsAreComputed(
	    experiment,
	    std::move(progress),
	    fixCorruptedData
	);

	std::vector<TagStatistics::ByTagID> allSpaceResult;

	typedef std::vector<TagStatisticsHelper::Loader> StatisticLoaderList;
	for (const auto &[spaceID, space] : experiment.Spaces()) {
		std::vector<TagStatisticsHelper::Timed> spaceResults;
		for (const auto &tdd : space->TrackingDataDirectories()) {
			spaceResults.push_back(tdd->TagStatistics());
		}
		allSpaceResult.push_back(TagStatisticsHelper::MergeTimed(
		                             spaceResults.begin(),
		                             spaceResults.end()
		)
		                             .TagStats);
	}

	result = TagStatisticsHelper::MergeSpaced(
	    allSpaceResult.begin(),
	    allSpaceResult.end()
	);
}

void Query::IdentifyFrames(
    const Experiment                                 &experiment,
    std::function<void(const IdentifiedFrame::Ptr &)> storeDataFunctor,
    const myrmidon::Query::IdentifyFramesArgs        &args
) {

	auto runner = QueryRunner::RunnerFor(args.SingleThreaded == false);

	runner(
	    experiment,
	    {
	        .Start                 = args.Start,
	        .End                   = args.End,
	        .Localize              = args.ComputeZones,
	        .Collide               = false,
	        .CollisionsIgnoreZones = false,
	        .Progress              = args.Progress.get(),
	    },
	    [=](const priv::QueryRunner::OrderedCollisionData &data) {
		    storeDataFunctor(std::get<1>(data));
	    }
	);
}

void Query::CollideFrames(
    const Experiment                          &experiment,
    std::function<void(const CollisionData &)> storeDataFunctor,
    const myrmidon::Query::CollideFramesArgs  &args
) {

	auto runner = QueryRunner::RunnerFor(args.SingleThreaded == false);
	runner(
	    experiment,
	    {
	        .Start                 = args.Start,
	        .End                   = args.End,
	        .Localize              = false,
	        .Collide               = true,
	        .CollisionsIgnoreZones = args.CollisionsIgnoreZones,
	        .Progress              = args.Progress.get(),
	    },
	    [=](const priv::QueryRunner::OrderedCollisionData &data) {
		    storeDataFunctor(
		        std::make_pair(std::get<1>(data), std::get<2>(data))
		    );
	    }
	);
}

void Query::ComputeTrajectories(
    const Experiment                                  &experiment,
    std::function<void(const AntTrajectory::Ptr &)>    storeDataFunctor,
    const myrmidon::Query::ComputeAntTrajectoriesArgs &args
) {
	auto runner = QueryRunner::RunnerFor(args.SingleThreaded == false);
	auto sargs  = DataSegmenter::Args{
	     .StoreTrajectory             = storeDataFunctor,
	     .StoreInteraction            = [](const AntInteraction::Ptr &) {},
	     .MaximumGap                  = args.MaximumGap,
	     .SummarizeSegment            = false,
	     .SegmentOnMatcherValueChange = args.SegmentOnMatcherValueChange,
    };
	if (args.Matcher) {
		sargs.Matcher = args.Matcher->ToPrivate();
		sargs.Matcher->SetUpOnce(experiment.Identifier()->Ants());
	}

	auto segmenter = DataSegmenter(sargs);
	runner(
	    experiment,
	    {
	        .Start                 = args.Start,
	        .End                   = args.End,
	        .Localize              = args.ComputeZones,
	        .Collide               = false,
	        .CollisionsIgnoreZones = false,
	        .Progress              = args.Progress.get(),
	    },
	    [&segmenter](const priv::QueryRunner::OrderedCollisionData &data) {
		    segmenter(std::make_pair(std::get<1>(data), std::get<2>(data)));
	    }
	);
}

void Query::ComputeAntInteractions(
    const Experiment                                  &experiment,
    std::function<void(const AntTrajectory::Ptr &)>    storeTrajectory,
    std::function<void(const AntInteraction::Ptr &)>   storeInteraction,
    const myrmidon::Query::ComputeAntInteractionsArgs &args
) {

	auto runner = QueryRunner::RunnerFor(args.SingleThreaded == false);
	DataSegmenter::Args sargs;
	sargs.MaximumGap                  = args.MaximumGap;
	sargs.SegmentOnMatcherValueChange = args.SegmentOnMatcherValueChange;
	if (args.Matcher) {
		sargs.Matcher = args.Matcher->ToPrivate();
		sargs.Matcher->SetUpOnce(experiment.Identifier()->Ants());
	}
	if (args.ReportFullTrajectories == true) {
		sargs.SummarizeSegment = false;
		sargs.StoreTrajectory  = storeTrajectory;
		sargs.StoreInteraction = storeInteraction;
	} else {
		sargs.SummarizeSegment = true;
		sargs.StoreTrajectory  = [](const AntTrajectory::Ptr &) {};
		sargs.StoreInteraction = storeInteraction;
	}
	auto segmenter = DataSegmenter(sargs);

	runner(
	    experiment,
	    {
	        .Start                 = args.Start,
	        .End                   = args.End,
	        .Localize              = false,
	        .Collide               = true,
	        .CollisionsIgnoreZones = args.CollisionsIgnoreZones,
	        .Progress              = args.Progress.get(),
	    },
	    [&segmenter](const auto &data) {
		    segmenter({std::get<1>(data), std::get<2>(data)});
	    }
	);
}

void Query::FindVideoSegments(
    const Experiment          &experiment,
    std::vector<VideoSegment> &segments,
    SpaceID                    space,
    const Time                &start,
    const Time                &end
) {

	segments.clear();

	if (experiment.Spaces().count(space) == 0) {

		return;
	}
	auto ranges = TrackingDataDirectory::IteratorRanges(
	    experiment.Spaces().at(space)->TrackingDataDirectories(),
	    start,
	    end
	);

	for (auto &[iter, end] : ranges) {
		MovieSegment::ConstPtr segment;
		MovieFrameID           nextMatch(0), movieID(0);
		for (; iter != end; ++iter) {
			const auto &frame      = *iter;
			auto        trackingID = frame->Frame().FrameID();
			if (segment == nullptr || movieID > segment->EndMovieFrame() ||
			    segment->EndFrame() < trackingID) {
				try {
					segment = iter.LockParent()
					              ->MovieSegments()
					              .Find(trackingID)
					              .second;
					movieID = segment->ToMovieFrameID(trackingID);
				} catch (const std::exception &) {
					// no movie for this time
					break;
				}
				segments.push_back(
				    {.Space            = space,
				     .AbsoluteFilePath = segment->AbsoluteFilePath(),
				     .Begin            = uint32_t(movieID)}
				);
			}
			try {
				nextMatch = segment->ToMovieFrameID(trackingID);
			} catch (const std::exception &) {
				// no more data
				break;
			}
			segments.back().End = uint32_t(nextMatch + 1);
			auto &data          = segments.back().Data;
			for (; movieID <= std::min(segment->EndMovieFrame(), nextMatch);
			     ++movieID) {
				if (movieID != nextMatch) {
					continue;
				}
				data.push_back(
				    {.Position = uint32_t(movieID),
				     .Time     = frame->Frame().Time()}
				);
			}
		}
	}
}

static void EnsureTagCloseUpsAreLoaded(
    const Experiment &e, ProgressReporter::Ptr &&progress, bool fixCorruptedData
) {
	std::vector<TrackingDataDirectory::Loader> loaders;
	for (const auto &[uri, tdd] : e.TrackingDataDirectories()) {
		if (tdd->TagCloseUpsComputed() == true) {
			continue;
		}
		auto localLoaders = tdd->PrepareTagCloseUpsLoaders();
		loaders.insert(loaders.end(), localLoaders.begin(), localLoaders.end());
	}

	Query::ProcessLoaders(loaders, std::move(progress), fixCorruptedData);
}

std::tuple<std::vector<std::string>, std::vector<TagID>, Eigen::MatrixXd>
Query::GetTagCloseUps(
    const Experiment &e, ProgressReporter::Ptr &&progress, bool fixCorruptedData
) {

	EnsureTagCloseUpsAreLoaded(e, std::move(progress), fixCorruptedData);

	std::vector<std::string> paths;
	std::vector<TagID>       IDs;
	Eigen::MatrixXd          positions;
	std::size_t              i = -1;
	for (const auto &[URI, tdd] : e.TrackingDataDirectories()) {
		const auto &closeUps = tdd->TagCloseUps();
		paths.reserve(paths.size() + closeUps.size());
		IDs.reserve(paths.size() + closeUps.size());
		positions.conservativeResize(positions.rows() + closeUps.size(), 11);
		for (const auto &cu : closeUps) {
			++i;
			paths.push_back(cu->AbsoluteFilePath());
			IDs.push_back(cu->TagValue());
			positions.block<1, 2>(i, 0) = cu->TagPosition().transpose();
			positions(i, 2)             = cu->TagAngle();
			for (size_t j = 0; j < 4; ++j) {
				positions.block<1, 2>(i, 2 * j + 3) =
				    cu->Corners()[j].transpose();
			}
		}
	}
	return {paths, IDs, positions};
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
