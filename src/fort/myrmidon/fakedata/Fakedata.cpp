#include "Fakedata.hpp"

#include <fort/myrmidon/Shapes.hpp>

#include <random>

namespace fort {
namespace myrmidon {

Fakedata::Fakedata(const fs::path & basedir) {
	try {
		BuildFakeData(basedir);
	} catch (const std::exception & e) {
		CleanUpFilesystem();
		throw;
	}
}

Fakedata::~Fakedata() {
	CleanUpFilesystem();
}


void Fakedata::BuildFakeData(const fs::path & basedir) {
	auto start = Time::Now();
	GenerateFakedata();
	d_basedir = basedir;
	WriteFakedata();
	std::cerr << "Generated data in " << Time::Now().Sub(start) << std::endl;
}

void Fakedata::CleanUpFilesystem() {
	if (d_basedir.empty()) {
		return;
	}
	fs::remove_all(d_basedir);
};


struct Keypoint {
	SpaceID Space;
	double  X,Y,Angle;
	Time    At;
};

struct InteractionData {
	AntID Other;
	Time Start,End;
	InteractionTypes Types;
	InteractionData(AntID other,
	                const Time & start,
	                const Time & end,
	                const std::vector<uint32_t> & types)
		: Other(other)
		, Start(start)
		, End(end) {
		Types.resize(types.size()/2,2);
		for ( size_t i = 0; i < types.size()/2; ++i) {
			Types(i,0) = types[2*i];
			Types(i,1) = types[2*i+1];
		}
	}
};

struct AntData {
	TypedCapsuleList Shape;

	std::vector<Keypoint>        Keypoints;
	std::vector<InteractionData> Interactions;

};

struct Generator {
	Time      Start,End;
	Duration  Framerate;
	float     Jitter;



	std::map<AntID,AntData> Ants;

	std::vector<Time> NestTicks,ForageTicks;
	std::vector<std::pair<SpaceID,Time>> Ticks;

	std::vector<AntTrajectory::Ptr> Trajectories;
	std::vector<AntInteraction::Ptr> Interactions;

	Generator() {
		Start = Time::Parse("2019-11-02T22:03:21.002+01:00");
		End = Start.Add(5 * Duration::Minute);
		Framerate = 250 * Duration::Millisecond;
		Jitter = 0.1;

		Ants =
			{
			 {1,{ // 1 is a static Queen
			     .Shape = {
			               {1,std::make_shared<Capsule>(Eigen::Vector2d(-50,0),
			                                            Eigen::Vector2d(0,0),
			                                            40.0,
			                                            40.0)},
			               {2,std::make_shared<Capsule>(Eigen::Vector2d(50,0),
			                                            Eigen::Vector2d(0,0),
			                                            40.0,
			                                            40.0)},
			               },
			     .Keypoints = {
			                   {1,50,50,0.0,Start},
			                   {1,49,51,0.0,End},
			                   },
			     .Interactions = {},
				 }},
			 {2,{ // 2 is kind of a Nurse
			     .Shape = {
			               {1,std::make_shared<Capsule>(Eigen::Vector2d(-20,0),
			                                            Eigen::Vector2d(0,0),
			                                            15.0,
			                                            15.0)},
			               {2,std::make_shared<Capsule>(Eigen::Vector2d(20,0),
			                                            Eigen::Vector2d(0,0),
			                                            15.0,
			                                            15.0)},
			               },
			     .Keypoints = {
			                   {1,950,50,M_PI,Start},
			                   {1,200,50,M_PI,Start.Add(1*Duration::Minute).Add(-1)},
			                   {1,50,50,0.0,Start.Add(1*Duration::Minute)},
			                   {1,50,50,0.0,Start.Add(1*Duration::Minute+10*Duration::Second)},
			                   {1,50,200,M_PI/2,Start.Add(1*Duration::Minute+10*Duration::Second).Add(1)},
			                   {1,50,950,M_PI/2,Start.Add(2*Duration::Minute)},
			                   {1,50,950,-M_PI/2,Start.Add(2*Duration::Minute+1*Duration::Second)},
			                   {1,50,200,-M_PI/2,Start.Add(3*Duration::Minute).Add(-1)},
			                   {1,50,50,M_PI,Start.Add(3*Duration::Minute)},
			                   {1,50,50,M_PI,Start.Add(4*Duration::Minute)},
			                   {1,200,200,M_PI/4,Start.Add(4*Duration::Minute).Add(1)},
			                   {1,900,900,M_PI/4,End},
			                   },
			     .Interactions = {
			                      InteractionData(1,
			                                      Start.Add(1*Duration::Minute),
			                                      Start.Add(1*Duration::Minute+10*Duration::Second),
			                                      {1,1,2,2}),
			                      InteractionData(1,
			                                      Start.Add(3*Duration::Minute),
			                                      Start.Add(4*Duration::Minute),
			                                      {1,2,2,1}),
			                      },
				 }},
			 {3,{ // 3 is a kind of forager
			     .Shape = {
			               {1,std::make_shared<Capsule>(Eigen::Vector2d(-20,0),
			                                            Eigen::Vector2d(0,0),
			                                            15.0,
			                                            15.0)},
			               {2,std::make_shared<Capsule>(Eigen::Vector2d(20,0),
			                                            Eigen::Vector2d(0,0),
			                                            15.0,
			                                            15.0)},
			               },
			     .Keypoints = {
			                   {1,50,500,0.0,Start},
			                   {1,1000,500,0.0,Start.Add(1*Duration::Minute)},
			                   {2,0,500,0.0,Start.Add(1*Duration::Minute+30*Duration::Second)},
			                   {2,500,500,0.0,Start.Add(2*Duration::Minute)},
			                   {2,500,500,M_PI,Start.Add(2*Duration::Minute+1*Duration::Second)},
			                   {2,0,500,M_PI,Start.Add(3*Duration::Minute)},
			                   {1,1000,500,M_PI,Start.Add(3*Duration::Minute+30*Duration::Second)},
			                   {1,1000,500,M_PI/2,Start.Add(3*Duration::Minute+31*Duration::Second)},
			                   {1,1000,1000,M_PI/2,End},
			                   },
			     .Interactions = {},
				 }},
			};

		BuildFrameTicks();
		BuildTrajectories();
		BuildInteractions();
	}

	std::vector<Time> DrawFrameTicks() const {
		std::random_device rd{};
		std::mt19937 gen{rd()};

		std::normal_distribution<> d{0.0,Framerate.Microseconds()*Jitter};

		std::vector<Time> res;
		for ( Time current = Start; current.Before(End); ) {
			res.push_back(current);
			auto increment = Framerate + std::clamp(int64_t(d(gen)),-15000L,15000L)*Duration::Microsecond;
			current = current.Add(increment);
		}
		return res;
	}

	void BuildFrameTicks() {
		NestTicks = DrawFrameTicks();
		ForageTicks = DrawFrameTicks();

		Ticks.clear();
		Ticks.reserve(NestTicks.size()+ForageTicks.size());
		for ( const auto & t : NestTicks ) {
			Ticks.push_back({1,t});
		}

		for ( const auto & t : ForageTicks ) {
			Ticks.push_back({2,t});
		}

		std::sort(Ticks.begin(),Ticks.end(),
		          [](const std::pair<SpaceID,Time> & a,
		             const std::pair<SpaceID,Time> & b) -> bool {
			          if (a.second == b.second ) {
				          return a.first < b.first;
			          }
			          return a.second < b.second;
		          });
	}

	void BuildTrajectories() {
		Trajectories.clear();
		for ( const auto & [antID,ant] : Ants ) {
			BuildTrajectoryFor(antID,ant);
		}
		std::sort(Trajectories.begin(),
		          Trajectories.end(),
		          [](const AntTrajectory::Ptr & a,
		             const AntTrajectory::Ptr & b) -> bool {
			          auto aEnd = a->End();
			          auto bEnd = b->End();
			          if ( aEnd == bEnd ) {
				          return a->Ant < b->Ant;
			          }
			          return aEnd < bEnd;
		          });
	}


	void BuildTrajectoryFor(AntID antID, const AntData & ant) {
		auto prevKey = ant.Keypoints.begin();
		auto nextKey = prevKey+1;

		AntTrajectory::Ptr current;
		size_t points = 0;
		for ( const auto & [spaceID,t] : Ticks) {
			if ( t < prevKey->At ) {
				continue;
			}
			//increment keypoints if needed
			while ( nextKey != ant.Keypoints.end() && (t > nextKey->At || nextKey->Space != prevKey->Space) ) {
				++nextKey;
				++prevKey;
			}
			if ( nextKey == ant.Keypoints.end() ) {
				break;
			}

			if ( current && current->Space != prevKey->Space ) {
				current->Positions.conservativeResize(points,5);
				Trajectories.push_back(current);
				current.reset();
			}
			if ( spaceID != prevKey->Space ) {
				continue;
			}

			// now we are in the right space, ant t is bounded by prevKey and nextKey

			//create trajectory as needed
			if ( !current ) {
				current = std::make_shared<AntTrajectory>();
				current->Ant = antID;
				current->Space = prevKey->Space;
				current->Start = t;
				current->Positions = Eigen::Matrix<double,Eigen::Dynamic,5>(Ticks.size(),5);
				points = 0;
			}

			// Add interpolation to current Trajectory
			auto prevPosition = Eigen::Vector3d(prevKey->X,prevKey->Y,prevKey->Angle);
			auto nextPosition = Eigen::Vector3d(nextKey->X,nextKey->Y,nextKey->Angle);
			double ratio =  (t.Sub(prevKey->At)).Seconds() / ( nextKey->At.Sub(prevKey->At)).Seconds();
			current->Positions(points,0) = (t.Sub(current->Start)).Seconds();
			current->Positions(points,4) = 0;
			current->Positions.block<1,3>(points,1) = (prevPosition + ratio * (nextPosition - prevPosition)).transpose();
			++points;
		}

		if ( current ) {
			current->Positions.conservativeResize(points,5);
			Trajectories.push_back(current);
		}
	}


	void BuildInteractions() {
		Interactions.clear();
		for( const auto & [antID,ant] : Ants ) {
			BuildInteractionsFor(antID,ant);
		}
		std::sort(Interactions.begin(),
		          Interactions.end(),
		          [](const AntInteraction::Ptr & a,
		             const AntInteraction::Ptr & b) {
			          return a->End < b->End;
		          });
		// for ( const auto & i : Interactions ) {
		// 	auto & a = i->Trajectories.first;
		// 	auto & b = i->Trajectories.second;

		// 	std::cerr << "Interaction between "
		// 	          << i->IDs.first
		// 	          << " and "
		// 	          << i->IDs.second
		// 	          << std::endl
		// 	          << " + from "
		// 	          << i->Start
		// 	          << std::endl
		// 	          << " + a st "
		// 	          << a.Trajectory->Start.Add(a.Trajectory->Positions(a.Begin,0) * Duration::Second.Nanoseconds())
		// 	          << std::endl
		// 	          << " + b st "
		// 	          << b.Trajectory->Start.Add(b.Trajectory->Positions(b.Begin,0) * Duration::Second.Nanoseconds())
		// 	          << std::endl
		// 	          << " +   to "
		// 	          << i->End
		// 	          << std::endl
		// 	          << " + a ed "
		// 	          << a.Trajectory->Start.Add(a.Trajectory->Positions(a.End-1,0) * Duration::Second.Nanoseconds())
		// 	          << std::endl
		// 	          << " + b ed "
		// 	          << b.Trajectory->Start.Add(b.Trajectory->Positions(b.End-1,0) * Duration::Second.Nanoseconds())
		// 	          << std::endl

		// 	          << std::endl;
		// }
	}

	void BuildInteractionsFor(AntID antID,const AntData & ant) {
		for ( const auto & i : ant.Interactions ) {
			auto [aSegment,aStart,aEnd] = FindTrajectorySegment(i.Other,i.Start,i.End);
			auto [bSegment,bStart,bEnd] = FindTrajectorySegment(antID,i.Start,i.End);
			auto res = std::make_shared<AntInteraction>();
			res->IDs = {i.Other,antID};
			res->Types = i.Types;
			res->Space = bSegment.Trajectory->Space;
			res->Start = std::min(aStart,bStart);
			res->End = std::max(aEnd,bEnd);
			res->Trajectories.first = std::move(aSegment);
			res->Trajectories.second = std::move(bSegment);
			Interactions.push_back(res);
		}
	}

	std::tuple<AntTrajectorySegment,Time,Time> FindTrajectorySegment(AntID antID,
	                                                                 const Time & start,
	                                                                 const Time & end) {
		auto fi = std::find_if(Trajectories.begin(),
		                       Trajectories.end(),
		                       [&](const AntTrajectory::Ptr & t) {
			                       return t->Ant == antID
				                       && t->Start <= start
				                       && t->End() >= end;
		                       });
		if ( fi == Trajectories.end() ) {
			throw std::runtime_error("could not find any suitable trajectory");
		}
		AntTrajectorySegment res;
		res.Trajectory = *fi;
		Duration offsetStart,offsetEnd;
		for(res.Begin = 0 ;res.Begin < res.Trajectory->Positions.rows();++res.Begin) {
			offsetStart = res.Trajectory->Positions(res.Begin,0) * Duration::Second.Nanoseconds();
			if ( res.Trajectory->Start.Add(offsetStart) >= start ) {
				break;
			}
		}
		for ( res.End = res.Begin; res.End < res.Trajectory->Positions.rows(); ++res.End) {
			offsetEnd = res.Trajectory->Positions(res.End,0) * Duration::Second.Nanoseconds();
			if ( res.Trajectory->Start.Add(offsetEnd) > end ) {
				break;
			}
		}
		Time resStart = res.Trajectory->Start.Add(offsetStart);
		Time resEnd = res.Trajectory->Start.Add(res.Trajectory->Positions(res.End-1,0) * Duration::Second.Nanoseconds());
		return std::make_tuple(std::move(res),resStart,resEnd);
	}

};

void Fakedata::GenerateFakedata() {
	Generator gen;

}

void Fakedata::WriteFakedata() {
}

} // namespace myrmidon
} // namespace fort
