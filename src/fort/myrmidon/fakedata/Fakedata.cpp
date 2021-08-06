#include "Fakedata.hpp"

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


struct Waypoint {
	SpaceID Space;
	double  X,Y,Angle;
	Time    At;
};

struct AntData {
	TypedCapsuleList Shape;

	std::vector<Waypoint> Waypoints;
};

struct Generator {
	Time      Start,End;
	Duration  Framerate;
	float     Jitter;

	std::map<AntID,AntData> Ants;

	std::vector<std::pair<SpaceID,Time>> FrameTimes;

	Generator() {
		Start = Time::Parse("2019-11-02T22:03:21.002+01:00");
		End = Start.Add(5 * Duration::Minute);
		Framerate = 250 * Duration::Millisecond;
		Jitter = 0.1;

		Ants =
			{
			 {1,{ // 1 is a static Queen
			     .Shape = {},
			     .Waypoints = {
			                   {1,50,50,0.0,Start},
			                   {1,49,51,M_PI,End}
			                   }
				 }
			 },
			 {2,{ // 2 is kind of a Nurse
			     .Shape = {},
			     .Waypoints = {
			                   {1,900,50,M_PI,Start},
			                   {1,60,50,M_PI,Start.Add(1*Duration::Minute)},
			                   {1,60,50,M_PI,Start.Add(1*Duration::Minute+10*Duration::Second)},
			                   {1,60,50,M_PI/2,Start.Add(1*Duration::Minute+11*Duration::Second)},
			                   {1,50,900,M_PI/2,Start.Add(2*Duration::Minute)},
			                   {1,50,900,-M_PI/2,Start.Add(2*Duration::Minute+1*Duration::Second)},
			                   {1,60,50,-M_PI/2,Start.Add(3*Duration::Minute)},
			                   {1,60,50,-M_PI/2,Start.Add(4*Duration::Minute)},
			                   {1,60,50,M_PI/4,Start.Add(4*Duration::Minute+Duration::Second)},
			                   {1,900,900,M_PI/4,End},
			                   }
				 }
			 },
			 {3,{ // 3 is a kind of forager
			     .Shape = {},
			     .Waypoints = {
			                   {1,50,500,0.0,Start},
			                   {1,1000,500,0.0,Start.Add(1*Duration::Minute)},
			                   {2,0,500,0.0,Start.Add(1*Duration::Minute+30*Duration::Second)},
			                   {2,500,500,0.0,Start.Add(2*Duration::Minute)},
			                   {2,500,500,M_PI,Start.Add(2*Duration::Minute+1*Duration::Second)},
			                   {2,0,500,M_PI,Start.Add(3*Duration::Minute)},
			                   {1,1000,500,M_PI,Start.Add(3*Duration::Minute+30*Duration::Second)},
			                   {1,1000,500,M_PI/2,Start.Add(3*Duration::Minute+31*Duration::Second)},
			                   {1,1000,1000,M_PI/2,End},
			                   }
				 }
			 },
			};

		BuildFrameTimes();
		BuildTrajectories();
	}

	std::vector<Time> DrawFrameTime() const {
		std::random_device rd{};
		std::mt19937 gen{rd()};

		std::normal_distribution<> d{0.0,Framerate.Microseconds()*Jitter};

		std::vector<Time> res;
		for ( Time current = Start; current.Before(End); ) {
			res.push_back(current);
			auto increment = Framerate + std::clamp(int64_t(d(gen)),-5000L,5000L)*Duration::Microsecond;
			current = current.Add(increment);
		}
		return res;
	}

	void BuildFrameTimes() {
		FrameTimes.clear();
		for ( const auto & t : DrawFrameTime() ) {
			FrameTimes.push_back({1,t});
		}
		for ( const auto & t : DrawFrameTime() ) {
			FrameTimes.push_back({2,t});
		}
		std::sort(FrameTimes.begin(),
		          FrameTimes.end(),
		          [](const std::pair<SpaceID,Time> & a,
		             const std::pair<SpaceID,Time> & b) -> bool {
			          if ( a.second == b.second ) {
				          return a.first < b.first;
			          }
			          return a.second < b.second;
		          });
	}
};

void Fakedata::GenerateFakedata() {
	Generator gen;

}

void Fakedata::WriteFakedata() {
}

} // namespace myrmidon
} // namespace fort
