#include "QueryRunner.hpp"

#include <thread>

#include <tbb/concurrent_queue.h>
#include <tbb/pipeline.h>

#include <fort/myrmidon/utils/Defer.hpp>

#include "TrackingDataDirectory.hpp"
#include "RawFrame.hpp"
#include "Experiment.hpp"
#include "Space.hpp"
#include "Identifier.hpp"
#include "CollisionSolver.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


class DataLoader {
public:
	DataLoader(const Experiment & experiment,
			   const QueryRunner::Args & args) {
		BuildRanges(experiment,args.Start,args.End);
		d_continue.store(true);
	}

	void Stop() {
		d_continue.store(false);
	}

	QueryRunner::RawData operator()() {
		if ( d_continue.load() == false ) {
			return std::make_pair(0,RawFrame::ConstPtr());
		}
		static int i = 0;
		SpaceID next(0);
		Time nextTime  = Time::SinceEver();
		for (  auto & [spaceID,spaceIter] : d_spaceIterators ) {
			if ( spaceIter.Done() ) {
				continue;
			}
			auto dataTime = (*spaceIter)->Frame().Time();
			if ( next == 0 || dataTime.Before(nextTime) ) {
				nextTime = dataTime;
				next = spaceID;
			}
		}

		if ( next == 0 ) {
			return std::make_pair(0,RawFrame::ConstPtr());
		}

		auto & dataIter = d_spaceIterators.at(next);
		auto res = *(dataIter);
		++dataIter;
		return std::make_pair(next,res);
	}

private:
	class TDDIterator {
	public:
		TDDIterator(TrackingDataDirectory::const_iterator & begin,
		            TrackingDataDirectory::const_iterator & end)
			: d_iter(std::move(begin))
			, d_end(std::move(end)) {
		}

		bool Done() const {
			return d_iter == d_end;
		}

		const RawFrameConstPtr & operator*() {
			return *d_iter;
		}

		TDDIterator & operator++() {
			if ( Done() ) {
				return *this;
			}
			++d_iter;
			return *this;
		}

	private:
		TrackingDataDirectory::const_iterator d_iter,d_end;
	};

	class SpaceIterator {
	public:
		SpaceIterator(std::vector<std::pair<TrackingDataDirectory::const_iterator,
		              TrackingDataDirectory::const_iterator>> & ranges ) {
			for ( auto & range : ranges ) {
				d_tddIterators.push_back(TDDIterator(range.first,range.second));
			}
			d_current = d_tddIterators.begin();
			while( !Done() && d_current->Done() ) {
				++d_current;
			}
		}

		bool Done() const {
			return d_current == d_tddIterators.end();
		}

		const RawFrameConstPtr & operator*() {
			return **d_current;
		}

		SpaceIterator & operator++() {
			if ( Done() ) {
				return *this;
			}
			++(*d_current);
			while ( !Done() && d_current->Done() ) {
				++d_current;
			}
			return *this;
		}

	private:
		std::vector<TDDIterator>            d_tddIterators;
		std::vector<TDDIterator>::iterator  d_current;
	};


	void BuildRanges(const Experiment & experiment,
	                 const Time & start,
	                 const Time & end) {
		for ( const auto & [spaceID,space] : experiment.Spaces() ) {
			auto ranges =TrackingDataDirectory::IteratorRanges(space->TrackingDataDirectories(),
			                                                   start,
			                                                   end);
			d_spaceIterators.insert(std::make_pair(spaceID,SpaceIterator(ranges)));
		}
	}

	std::map<SpaceID,SpaceIterator> d_spaceIterators;
	std::atomic<bool>       d_continue;
};


void QueryRunner::RunMultithread(const Experiment & experiment,
								 const Args & args,
								 Finalizer finalizer) {

	auto loader = std::make_shared<DataLoader>(experiment,args);

	tbb::filter_t<void,RawData>
		loadData(tbb::filter::serial_in_order,
		         [loader](tbb::flow_control & fc) -> RawData {
			         auto res = (*loader)();
			         if ( std::get<0>(res) == 0 ) {
				         fc.stop();
			         }
			         return res;
		         });

	tbb::filter_t<RawData,CollisionData>
		computeData(tbb::filter::parallel,
					QueryRunner::computeData(experiment,args));

	tbb::filter_t<CollisionData,void>
		finalizeData(tbb::filter::serial_in_order,
					 finalizer);

	tbb::parallel_pipeline(std::thread::hardware_concurrency()*2,
						   loadData & computeData & finalizeData);

}

void QueryRunner::RunSingleThread(const Experiment & experiment,
								  const Args & args,
								  Finalizer finalizer) {

	DataLoader loader(experiment,args);
	auto compute = QueryRunner::computeData(experiment,args);
	// we simply run in a single thread
	for (;;) {
		auto raw = loader();
		if ( std::get<0>(raw) == 0 ) {
			break;
		}
		auto data  = compute(raw);
		finalizer(data);
	}
}

void QueryRunner::RunMultithreadFinalizeInCurrent(const Experiment & experiment,
												  const Args & args,
												  Finalizer finalizer) {
	// we use a queue to retrieve all data in the main thread
	tbb::concurrent_bounded_queue<myrmidon::CollisionData> queue;

	auto loader = std::make_shared<DataLoader>(experiment,args);

	tbb::filter_t<void,RawData>
		loadData(tbb::filter::serial_in_order,
		         [loader](tbb::flow_control & fc) -> RawData {
			         auto res = (*loader)();
			         if ( std::get<0>(res) == 0 ) {
				         fc.stop();
			         }
			         return res;
		         });

	tbb::filter_t<RawData,CollisionData>
		computeData(tbb::filter::parallel,
					QueryRunner::computeData(experiment,args));

	tbb::filter_t<CollisionData,void>
		finalizeData(tbb::filter::serial_in_order,
		             [&queue](const myrmidon::CollisionData & data){
			             queue.push(data);
		             });

	// we spawn a child process that will feed and close the queue
	auto process
		= [&]() {
			  tbb::parallel_pipeline(std::thread::hardware_concurrency()*2,
			                         loadData & computeData & finalizeData);
			  queue.push(std::make_pair(nullptr,nullptr));
		  };

	std::thread go(process);

	// we consume the queue in the current thread
	for (;;) {
		CollisionData v;
		queue.pop(v);
		if ( v.first == nullptr && v.second == nullptr ) {
			break;
		}
		try {
			finalizer(v);
		} catch(...) {
			loader->Stop();
			go.join();
			throw;
		}
	}

	// we wait for our thread to finish, should be the case as it is the one closing the queue
	go.join();
}


QueryRunner::Runner QueryRunner::RunnerFor(bool multithread,bool finalizerInCurrentThread) {
	if ( multithread == false ) {
		return RunSingleThread;
	}
	if ( finalizerInCurrentThread == false ) {
		return RunMultithread;
	}
	return RunMultithreadFinalizeInCurrent;
}


std::function<CollisionData(const QueryRunner::RawData &)>
QueryRunner::computeData(const Experiment & experiment,
						 const QueryRunner::Args & args) {
	auto identifier = Identifier::Compile(experiment.Identifier());
	if ( args.Collide == false && args.Localize == false ) {
		return [identifier](const RawData & raw) {
			       // TODO optimize memory allocation here
			       auto identified = std::make_shared<IdentifiedFrame>();
			       std::get<1>(raw)->IdentifyFrom(*identified,*identifier,std::get<0>(raw));
				   return std::make_pair(identified,nullptr);
			   };
	}
	auto collider = experiment.CompileCollisionSolver(args.CollisionsIgnoreZones);
	if ( args.Collide == false ) {
		return [identifier,collider] (const RawData & raw) {
			       // TODO optimize memory allocation here
			       auto identified = std::make_shared<IdentifiedFrame>();
			       std::get<1>(raw)->IdentifyFrom(*identified,*identifier,std::get<0>(raw));
				   auto zoner = collider->ZonerFor(*identified);
				   for ( size_t i = 0; i < identified->Positions.rows(); ++i ) {
					   zoner->LocateAnt(identified->Positions.row(i));
				   }
				   return std::make_pair(identified,nullptr);
			   };
	}

	return [identifier,collider] (const RawData & raw) {
		       // TODO optimize memory allocation here
		       auto identified = std::make_shared<IdentifiedFrame>();
		       std::get<1>(raw)->IdentifyFrom(*identified,*identifier,std::get<0>(raw));
		       // TODO optimize memory allocation here
		       auto collided = std::make_shared<CollisionFrame>();
		       collider->ComputeCollisions(*collided,*identified);
		       return std::make_pair(identified,collided);
	       };


}

} // namespace priv
} // namespace myrmidon
} // namespace fort
