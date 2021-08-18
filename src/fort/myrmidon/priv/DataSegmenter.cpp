#include "DataSegmenter.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

DataSegmenter::BuildingTrajectory::BuildingTrajectory(const IdentifiedFrame & frame,
													  const PositionedAntConstRef & ant)
	: Trajectory(std::make_shared<AntTrajectory>())
	, Last(frame.FrameTime)
	, DataPoints({0.0,ant(0,1),ant(0,2),ant(0,3),ant(0,4)})
	, ForceKeep(false) {
	Trajectory->Ant = ant(0,0);
	Trajectory->Start = frame.FrameTime;
	Trajectory->Space = frame.Space;
}

size_t DataSegmenter::BuildingTrajectory::Size() const {
	return DataPoints.size()/5;
}

void DataSegmenter::BuildingTrajectory::Append(const IdentifiedFrame & frame,
                                               const PositionedAntConstRef & ant) {
	Last = frame.FrameTime;
	double t = frame.FrameTime.Sub(Trajectory->Start).Seconds();
	DataPoints.insert(DataPoints.end(),
	                  {t,ant(0,1),ant(0,2),ant(0,3),ant(0,4)});

	// MaxEnd is an over-estimation as it may be incremented after the
	// moment the Interaction should be Terminated.
	for ( BuildingInteraction* i : this->Interactions ) {
		if ( i->Trajectories.first.get() == this ) {
			i->MaxEnd.first += 1;
		}
		if ( i->Trajectories.second.get() == this ) {
			i->MaxEnd.second += 1;
		}
	}

}


AntTrajectory::Ptr DataSegmenter::BuildingTrajectory::Terminate() {
	if ( Size() < 2 ) {
		return AntTrajectory::Ptr();
	}
	Trajectory->Positions = Eigen::Map<const Eigen::Matrix<double,Eigen::Dynamic,5,Eigen::RowMajor>>(&DataPoints[0],Size(),5);
	return Trajectory;
}

DataSegmenter::BuildingInteraction::BuildingInteraction(const Collision & collision,
														const Time & curTime,
														std::pair<BuildingTrajectory::Ptr,BuildingTrajectory::Ptr> trajectories)
	: IDs(collision.IDs)
	, Start(curTime)
	, Last(curTime)
	, Trajectories(trajectories) {
	Trajectories.first->Interactions.insert(this);
	Trajectories.second->Interactions.insert(this);
	SegmentStarts = {trajectories.first->Size()-1,trajectories.second->Size()-1};
	MinEnd = SegmentStarts;
	MaxEnd = SegmentStarts;
	trajectories.first->ForceKeep = true;
	trajectories.second->ForceKeep = true;
	for ( size_t i = 0; i < collision.Types.rows(); ++i ) {
		Types.insert(std::make_pair(collision.Types(i,0),
		                            collision.Types(i,1)));
	}
}

DataSegmenter::BuildingInteraction::~BuildingInteraction() {
	if ( Trajectories.first ) {
		Trajectories.first->Interactions.erase(this);
	}
	if ( Trajectories.second ) {
		Trajectories.second->Interactions.erase(this);
	}
}


void DataSegmenter::BuildingInteraction::Append(const Collision & collision,
												const Time & curTime) {
	Last = curTime;
	for ( size_t i = 0; i < collision.Types.rows(); ++i ) {
		Types.insert(std::make_pair(collision.Types(i,0),
		                            collision.Types(i,1)));
	}
	// MinEnd is a lower estimation, as we may have no collision for
	// some frame in an Interaction.
	MinEnd.first++;
	MinEnd.second++;
}
Time DataSegmenter::BuildingTrajectory::TimeAt(size_t index) const {
	return Trajectory->Start.Add(DataPoints[5*index] * Duration::Second.Nanoseconds());
}

size_t DataSegmenter::BuildingTrajectory::FindIndexFor(const Time & time,
                                                       size_t min,
                                                       size_t max) {
	min = std::min(min,Size()-1);
	max = std::min(max,Size()-1);
	if ( TimeAt(min) >= time ) {
		return min;
	}
	++min;
	while(min < max) {
		auto m = (min+max)/2;
		auto t = TimeAt(m);
		if ( t == time ) {
			return m;
		}
		if ( t < time ) {
			min = m + 1;
		} else {
			max = m -1;
		}
	}
	return min;
}


void DataSegmenter::BuildingInteraction::SummarizeTrajectorySegment(AntTrajectorySegment & s) {
	s.Mean = std::make_unique<Eigen::Vector3d>(Eigen::Vector3d::Zero());
	for ( int i = s.Begin; i < s.End; ++i ) {
		*(s.Mean) += s.Trajectory->Positions.block<1,3>(i,1).transpose() / (s.End - s.Begin);
	}
	s.Trajectory.reset();
	s.Begin = 0;
	s.End = 0;
}


AntInteraction::Ptr DataSegmenter::BuildingInteraction::Terminate(bool summarize) {
	if (Start == Last ) {
		return AntInteraction::Ptr();
	}
	auto res = std::make_shared<AntInteraction>();
	res->IDs = IDs;
	res->Space = Trajectories.first->Trajectory->Space;
	res->Types = InteractionTypes(Types.size(),2);
	size_t i = 0;
	for ( const auto & type : Types ) {
		res->Types(i,0) = type.first;
		res->Types(i,1) = type.second;
		++i;
	}

	res->Trajectories.first = {
							   .Trajectory = Trajectories.first->Trajectory,
							   .Begin = SegmentStarts.first,
							   .End = Trajectories.first->FindIndexFor(Last,MinEnd.first,MaxEnd.first)+1,
	};

	res->Trajectories.second = {
							   .Trajectory = Trajectories.second->Trajectory,
							   .Begin = SegmentStarts.second,
							   .End = Trajectories.second->FindIndexFor(Last,MinEnd.second,MaxEnd.second)+1,
	};

	if ( summarize == true ) {
		SummarizeTrajectorySegment(res->Trajectories.first);
		SummarizeTrajectorySegment(res->Trajectories.second);
	}


	res->Start = Start;
	res->End = Last;

	Trajectories.first->Interactions.erase(this);
	Trajectories.second->Interactions.erase(this);
	Trajectories.first.reset();
	Trajectories.second.reset();
	return res;
}

DataSegmenter::DataSegmenter(const Args & args)
	: d_args(args) {
}

DataSegmenter::~DataSegmenter() {
	for ( auto & [id,interaction] : d_interactions ) {
		auto i = interaction->Terminate(d_args.SummarizeSegment);
		if ( i == nullptr ) {
			continue;
		}
		d_args.StoreInteraction(i);
	}

	for ( auto & [antID,trajectory] : d_trajectories ) {
		if ( d_args.Matcher && d_args.Matcher->Match(antID,0,{}) == false ) {
			continue;
		}
		auto t = trajectory->Terminate();
		if ( t == nullptr ) {
			continue;
		}
		d_args.StoreTrajectory(t);
	}
}

inline bool MonoIDMismatch(const Time & a,
                    const Time & b) {
	if ( a.HasMono() ) {
		if ( b.HasMono() ) {
			return a.MonoID() != b.MonoID();
		}
		return true;
	}
	return b.HasMono() == false;
}

void DataSegmenter::operator()(const myrmidon::Query::CollisionData & data) {

	if ( d_args.Matcher ) {
		d_args.Matcher->SetUp(std::get<0>(data),std::get<1>(data));
	}
	bool hasCollision = std::get<1>(data) != nullptr;

	BuildTrajectories(std::get<0>(data),
					  hasCollision);

	if ( hasCollision ) {
		BuildInteractions(std::get<1>(data));
	}
}

void DataSegmenter::BuildTrajectories(const IdentifiedFrame::Ptr & identified,
                                      bool conserveAllTrajectory) {
	for ( size_t i = 0; i < identified->Positions.rows(); ++i ) {
		AntID antID = identified->Positions(i,0);
		if ( conserveAllTrajectory == false
			 && d_args.Matcher
			 && d_args.Matcher->Match(antID,0,{}) == false ) {
			continue;
		}


		auto fi = d_trajectories.find(antID);
		if ( fi == d_trajectories.end() ) {
			d_trajectories.insert(std::make_pair(antID,
												 std::make_shared<BuildingTrajectory>(*identified,
												                                      identified->Positions.row(i))));
			continue;
		}

		bool maximumGapReached = identified->FrameTime.Sub(fi->second->Last) > d_args.MaximumGap;
		bool spaceChanged =  identified->Space != fi->second->Trajectory->Space;
		if ( MonoIDMismatch(identified->FrameTime,fi->second->Last)
			 || maximumGapReached
			 || spaceChanged ) {


			TerminateTrajectory(fi->second);
			fi->second = std::make_shared<BuildingTrajectory>(*identified,
			                                                  identified->Positions.row(i));
		} else {
			fi->second->Append(*identified,identified->Positions.row(i));
		}
	}

	std::vector<uint32_t> terminated;

	for ( const auto & [antID,trajectory] : d_trajectories ) {
		if ( identified->FrameTime.Sub(trajectory->Last) <= d_args.MaximumGap ) {
			continue;
		}
		terminated.push_back(antID);
		TerminateTrajectory(trajectory);
	}

	for ( const auto & antID : terminated ) {
		d_trajectories.erase(antID);
	}

}

void DataSegmenter::TerminateTrajectory(const BuildingTrajectory::Ptr & trajectory) {
	auto antID = trajectory->Trajectory->Ant;
	std::vector<InteractionID> toRemove;
	for ( const auto & interaction : trajectory->Interactions ) {
		toRemove.push_back(interaction->IDs);
		auto i = interaction->Terminate(d_args.SummarizeSegment);
		if ( i == nullptr ) {
			continue;
		}
		d_args.StoreInteraction(i);
	}

	for ( const auto & IDs : toRemove ) {
		d_interactions.erase(IDs);
	}

	if ( d_args.Matcher
		 && d_args.Matcher->Match(antID,0,{}) == false
		 && trajectory->ForceKeep == false) {
		return;
	}


	auto t = trajectory->Terminate();
	if ( t == nullptr ) {
		return;
	}
	d_args.StoreTrajectory(t);
}

void DataSegmenter::BuildInteractions(const CollisionFrame::Ptr & collisions) {
	for ( const auto & collision : collisions->Collisions ) {
		if ( d_args.Matcher
			 && d_args.Matcher->Match(collision.IDs.first,
									  collision.IDs.second,
									  collision.Types) == false ) {
			continue;
		}

		auto fi = d_interactions.find(collision.IDs);
		if ( fi == d_interactions.end() ) {
			try {
				auto trajectories = std::make_pair(d_trajectories.at(collision.IDs.first),
												   d_trajectories.at(collision.IDs.second));
				d_interactions.insert(std::make_pair(collision.IDs,
				                                     std::make_unique<BuildingInteraction>(collision,
					     collisions->FrameTime,
					     trajectories)));
			} catch ( const std::exception & e ) {
			}
			continue;
		}

		if ( MonoIDMismatch(collisions->FrameTime,fi->second->Last) == true
			 || collisions->FrameTime.Sub(fi->second->Last) > d_args.MaximumGap ) {
			auto i = fi->second->Terminate(d_args.SummarizeSegment);
			if ( i != nullptr ) {
				d_args.StoreInteraction(i);
			}
			try {
				auto trajectories = std::make_pair(d_trajectories.at(collision.IDs.first),
												   d_trajectories.at(collision.IDs.second));
				fi->second = std::make_unique<BuildingInteraction>(collision,
				                                                   collisions->FrameTime,
				                                                   trajectories);
			} catch ( const std::exception & e ) {
				d_interactions.erase(fi);
			}
		} else {
			fi->second->Append(collision,collisions->FrameTime);
		}
	}
	std::vector<InteractionID> terminated;
	for ( auto & [IDs,interaction] : d_interactions ) {
		if ( collisions->FrameTime.Sub(interaction->Last) <= d_args.MaximumGap ) {
			continue;
		}
		terminated.push_back(IDs);
		auto i = interaction->Terminate(d_args.SummarizeSegment);
		if ( i != nullptr ) {
			d_args.StoreInteraction(i);
		}
	}

	for ( const auto & IDs : terminated ) {
		d_interactions.erase(IDs);
	}
}





} // namespace priv
} // namespace myrmidon
} // namespace fort
