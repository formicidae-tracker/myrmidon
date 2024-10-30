#include "QueryRunner.hpp"

#include <fort/hermes/Error.hpp>
#include <fort/hermes/FileContext.hpp>
#include <iomanip>
#include <ios>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>

#include <tbb/concurrent_queue.h>
#include <tbb/flow_graph.h>

#include <fort/myrmidon/utils/Defer.hpp>

#include "CollisionSolver.hpp"
#include "Experiment.hpp"
#include "Identifier.hpp"
#include "RawFrame.hpp"
#include "Space.hpp"
#include "TrackingDataDirectory.hpp"
#include "fort/myrmidon/priv/ForwardDeclaration.hpp"
#include "fort/myrmidon/priv/TrackingDataDirectoryError.hpp"
#include "fort/myrmidon/types/Collision.hpp"
#include "fort/myrmidon/types/Reporter.hpp"

#include <fort/myrmidon/myrmidon-config.h>

namespace fort {
namespace myrmidon {
namespace priv {

class DataLoader {
public:
	DataLoader(const Experiment &experiment, const QueryRunner::Args &args)
	    : d_reporter{args.Progress} {
		BuildRanges(experiment, args.Start, args.End);
		d_continue.store(true);

		setProgressBounds(experiment, args);
		computeDuration(experiment, args);
	}

	~DataLoader() {
		if (d_lost == 0 || d_reporter == nullptr) {
			return;
		}
		std::ostringstream oss;
		double percent = d_lost.Seconds() / d_duration.Seconds() * 100.0;
		oss << "data corruption during query representing "
		    << std::setprecision(2) << std::fixed << percent
		    << "% of total query time";
		d_reporter->ReportError(oss.str());
	}

	void setProgressBounds(
	    const Experiment &experiment, const QueryRunner::Args &args
	) {
		if (args.Progress == nullptr) {
			return;
		}

		if (args.Start.IsInfinite() == false &&
		    args.End.IsInfinite() == false) {
			args.Progress->SetBound(args.Start, args.End);
			return;
		}

		auto start =
		    args.Start.IsInfinite() ? fort::Time::Forever() : args.Start;
		auto end   = args.End.IsInfinite() ? fort::Time::SinceEver() : args.End;
		bool empty = true;
		for (const auto &[spaceID, space] : experiment.Spaces()) {
			const auto &tdds = space->TrackingDataDirectories();
			if (tdds.empty()) {
				continue;
			}
			empty = false;
			if (args.Start.IsInfinite()) {
				start = std::min(start, tdds.front()->Start());
			}
			if (args.End.IsInfinite()) {
				end = std::max(end, tdds.back()->End());
			}
		}

		if (empty == false) {
			args.Progress->SetBound(start, end);
		}
	}

	void Stop() {
		d_continue.store(false);
	}

	QueryRunner::RawData operator()() {
		if (d_continue.load() == false) {
			return {.Space = 0, .Frame = nullptr, .ID = 0};
		}
		SpaceID next(0);
		Time    nextTime = Time::SinceEver();
		for (auto &[spaceID, spaceIter] : d_spaceIterators) {
			if (spaceIter.Done()) {
				continue;
			}

			fort::Time dataTime;
			try {
				dataTime = (*spaceIter)->Frame().Time();
			} catch (const CorruptedHermesFileIterator &e) {
				recover(spaceID, e);
				return (*this)();
			}

			if (next == 0 || dataTime.Before(nextTime)) {
				nextTime = dataTime;
				next     = spaceID;
			}
		}

		if (next == 0) {
			return {.Space = 0, .Frame = nullptr, .ID = 0};
		}

		auto &dataIter = d_spaceIterators.at(next);
		auto  res      = *(dataIter);
		++dataIter;
		return {.Space = next, .Frame = res, .ID = d_nextID++};
	}

	void recover(SpaceID ID, const CorruptedHermesFileIterator &e) {
		if (e.NextAvailableID().has_value() &&
		    e.NextAvailableID().value() <
		        d_spaceIterators.at(ID).d_current->d_end.Index()) {
			d_spaceIterators.at(ID).d_current->d_iter = e.Next();
		} else {
			d_spaceIterators.at(ID).d_current->d_iter =
			    d_spaceIterators.at(ID).d_current->d_end;
			++(d_spaceIterators.at(ID));
		}

		d_lost = d_lost + e.Lost();

		if (!d_reporter) {
			return;
		}

		double lost = e.Lost().Seconds() / d_duration.Seconds() * 100.0;

		std::ostringstream oss;

		oss << e.what() << "; lost " << e.Lost().Truncate(Duration::Second)
		    << ", " << std::setprecision(2) << std::fixed << lost
		    << "% of total query time of "
		    << d_duration.Truncate(Duration::Second);

		d_reporter->ReportError(oss.str());
	}

	void computeDuration(const Experiment &e, const QueryRunner::Args &args) {
		d_duration = 0;
		for (const auto &[spaceID, space] : e.Spaces()) {
			for (const auto &tdd : space->TrackingDataDirectories()) {
				auto start = std::max(args.Start, tdd->Start());
				auto end   = std::min(args.End, tdd->End().Add(-1));
				if (start.Before(end)) {
					d_duration = d_duration + end.Sub(start);
				}
			}
		}
	}

private:
	class TDDIterator {
	public:
		TDDIterator(
		    TrackingDataDirectory::const_iterator &begin,
		    TrackingDataDirectory::const_iterator &end
		)
		    : d_iter(std::move(begin))
		    , d_end(std::move(end)) {}

		bool Done() const {
			return d_iter.Index() >= d_end.Index();
		}

		const RawFrameConstPtr &operator*() {
			return *d_iter;
		}

		TDDIterator &operator++() {
			if (Done()) {
				return *this;
			}
			++d_iter;
			return *this;
		}

	private:
		friend class DataLoader;
		TrackingDataDirectory::const_iterator d_iter, d_end;
	};

	class SpaceIterator {
	public:
		SpaceIterator(std::vector<std::pair<
		                  TrackingDataDirectory::const_iterator,
		                  TrackingDataDirectory::const_iterator>> &ranges) {
			for (auto &range : ranges) {
				d_tddIterators.push_back(TDDIterator{
				    range.first,
				    range.second,
				});
			}
			d_current = d_tddIterators.begin();
			while (!Done() && d_current->Done()) {
				++d_current;
			}
		}

		bool Done() const {
			return d_current == d_tddIterators.end();
		}

		const RawFrameConstPtr &operator*() {
			return **d_current;
		}

		SpaceIterator &operator++() {
			if (Done()) {
				return *this;
			}
			++(*d_current);
			while (!Done() && d_current->Done()) {
				++d_current;
			}
			return *this;
		}

		void SkipCurrentTDD() {
			if (d_current != d_tddIterators.end()) {
				++d_current;
			}
		}

	private:
		friend class DataLoader;
		std::vector<TDDIterator>           d_tddIterators;
		std::vector<TDDIterator>::iterator d_current;
	};

	void BuildRanges(
	    const Experiment &experiment, const Time &start, const Time &end
	) {

		for (const auto &[spaceID, space] : experiment.Spaces()) {
			auto ranges = TrackingDataDirectory::IteratorRanges(
			    space->TrackingDataDirectories(),
			    start,
			    end
			);

			d_spaceIterators.insert(
			    std::make_pair(spaceID, SpaceIterator(ranges))
			);
		}
	}

	std::map<SpaceID, SpaceIterator> d_spaceIterators;
	std::atomic<bool>                d_continue;

	ErrorReporter *d_reporter;

	Duration d_duration, d_lost;
	uint64_t d_nextID = 0;
};

void QueryRunner::RunSingleThread(
    const Experiment &experiment, const Args &args, Finalizer finalizer
) {

	DataLoader loader(experiment, args);
	auto       compute = QueryRunner::computeData(experiment, args);
	// we simply run in a single thread
	fort::Time current = args.Start;

	for (;;) {
		auto raw = loader();
		if (raw.Space == 0) {
			break;
		}
		auto data = compute(raw);
		finalizer(data);

		auto time = std::get<1>(data)->FrameTime;
		if (args.Progress != nullptr && current.Before(time)) {
			current = time;
			args.Progress->Update(time);
		}
	}
}

void QueryRunner::RunMultithread(
    const Experiment &experiment, const Args &args, Finalizer finalizer
) {
	// we use a queue to retrieve all data in the main thread
	tbb::concurrent_bounded_queue<OrderedCollisionData> queue;

	auto loader = std::make_shared<DataLoader>(experiment, args);

	tbb::flow::graph g;

	tbb::flow::input_node<RawData> input{
	    g,
	    [loader](tbb::flow_control &fc) {
		    auto res = (*loader)();
		    if (res.Space == 0) {
			    fc.stop();
		    }
		    return res;
	    },
	};

	tbb::flow::limiter_node<RawData> limiter{
	    g,
	    4 * std::thread::hardware_concurrency(),
	};
	tbb::flow::make_edge(input, limiter);

	tbb::flow::function_node<RawData, OrderedCollisionData> compute{
	    g,
	    tbb::flow::unlimited,
	    QueryRunner::computeData(experiment, args),
	};
	tbb::flow::make_edge(limiter, compute);

	tbb::flow::sequencer_node<OrderedCollisionData> ordering{
	    g,
	    [](const OrderedCollisionData &data) { return std::get<0>(data); },
	};
	tbb::flow::make_edge(compute, ordering);

	tbb::flow::function_node<OrderedCollisionData, tbb::flow::continue_msg>
	    finalize{
	        g,
	        tbb::flow::serial,
	        [&queue](const OrderedCollisionData &data) {
		        queue.push(data);
		        return tbb::flow::continue_msg{};
	        },
	    };

	tbb::flow::make_edge(ordering, finalize);
#ifdef MYRMIDON_TBB_HAVE_DECREMENTER
	tbb::flow::make_edge(finalize, limiter.decrementer());
#else
	tbb::flow::make_edge(finalize, limiter.decrement);
#endif

	// we spawn a child process that will feed and close the queue
	auto process = [&]() {
		input.activate();
		g.wait_for_all();
		queue.push(std::make_tuple(0, nullptr, nullptr));
	};

	std::thread go(process);

	fort::Time current = args.Start;
	// we consume the queue in the current thread
	for (;;) {
		OrderedCollisionData v;
		queue.pop(v);
		if (std::get<1>(v) == nullptr && std::get<2>(v) == nullptr) {
			break;
		}
		auto time = std::get<1>(v)->FrameTime;
		if (args.Progress != nullptr && current.Before(time)) {
			current = time;
			args.Progress->Update(time);
		}

		try {
			finalizer(v);
		} catch (...) {
			loader->Stop();
			go.join();
			throw;
		}
	}

	// we wait for our thread to finish, should be the case as it is the one
	// closing the queue
	go.join();
}

QueryRunner::Runner QueryRunner::RunnerFor(bool multithread) {
	if (multithread == false) {
		return RunSingleThread;
	}
	return RunMultithread;
}

QueryRunner::Computer QueryRunner::computeData(
    const Experiment &experiment, const QueryRunner::Args &args
) {
	auto identifier = Identifier::Compile(experiment.Identifier());
	if (args.Collide == false && args.Localize == false) {
		return [identifier](const RawData &raw) {
			// TODO optimize memory allocation here
			auto identified = std::make_shared<IdentifiedFrame>();
			raw.Frame->IdentifyFrom(*identified, *identifier, raw.Space);
			return std::make_tuple(raw.ID, identified, nullptr);
		};
	}
	auto collider =
	    experiment.CompileCollisionSolver(args.CollisionsIgnoreZones);
	if (args.Collide == false) {
		return [identifier, collider](const RawData &raw) {
			// TODO optimize memory allocation here
			auto identified = std::make_shared<IdentifiedFrame>();
			raw.Frame->IdentifyFrom(*identified, *identifier, raw.Space);
			auto zoner = collider->ZonerFor(*identified);
			for (size_t i = 0; i < identified->Positions.rows(); ++i) {
				zoner->LocateAnt(identified->Positions.row(i));
			}
			return std::make_tuple(raw.ID, identified, nullptr);
		};
	}

	return [identifier, collider](const RawData &raw) {
		// TODO optimize memory allocation here
		auto identified = std::make_shared<IdentifiedFrame>();
		raw.Frame->IdentifyFrom(*identified, *identifier, raw.Space);
		// TODO optimize memory allocation here
		auto collided = std::make_shared<CollisionFrame>();
		collider->ComputeCollisions(*collided, *identified);
		return std::make_tuple(raw.ID, identified, collided);
	};
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
