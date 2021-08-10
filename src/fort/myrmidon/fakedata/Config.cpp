#include "Config.hpp"



namespace fort {
namespace myrmidon {


InteractionData::InteractionData(AntID other,
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



Config::Config() {
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
}


} // namespace myrmidon
} // namespace fort
