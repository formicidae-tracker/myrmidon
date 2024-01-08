#include "Config.hpp"

#include <fort/myrmidon/priv/Isometry2D.hpp>

namespace fort {
namespace myrmidon {


InteractionData::InteractionData(AntID other,
                                 SpaceID spaceID,
                                 const Time & start,
                                 const Time & end,
                                 const std::vector<uint32_t> & types)
	: Other(other)
	, Space(spaceID)
	, Start(start)
	, End(end) {
	Types.resize(types.size()/2,2);
	for ( size_t i = 0; i < types.size()/2; ++i) {
		Types(i,0) = types[2*i];
		Types(i,1) = types[2*i+1];
	}
}

Config::Config() {
	Start     = Time::Parse("2019-11-02T22:03:21.002+01:00");
	End       = Start.Add(5 * Duration::Minute);
	Framerate = {.Num = 4, .Den = 1};
	Segment   = Duration::Minute;
	Jitter    = 0.1;
	Width     = 1000;
	Height    = 1000;
	Ants      = {
	         {1,
	          {
	              // 1 is a static Queen
	              .Shape =
                 {
	                      {1,
	                       std::make_shared<Capsule>(
                          Eigen::Vector2d(-120, 0),
                          Eigen::Vector2d(-50, 0),
                          40.0,
                          40.0
                      )},
	                      {2,
	                       std::make_shared<Capsule>(
                          Eigen::Vector2d(50, 0),
                          Eigen::Vector2d(120, 0),
                          40.0,
                          40.0
                      )},
                 },
	              .Keypoints =
                 {
	                      {1, 100, 100, M_PI / 4, Start},
	                      {1, 99, 101, 0.0, End},
                 },
	              .Interactions = {},
	              .AntPose      = Eigen::Vector3d(60, 10, 0),
	              .AntSize      = 180,
	              .TagSize      = 40,
	              .Color        = 0,
	              .IsQueen      = true,
         }},
	         {2,
	          {
	              // 2 is kind of a Nurse
	              .Shape =
                 {
	                      {1,
	                       std::make_shared<Capsule>(
                          Eigen::Vector2d(-20, 0),
                          Eigen::Vector2d(-65, 0),
                          15.0,
                          15.0
                      )},
	                      {2,
	                       std::make_shared<Capsule>(
                          Eigen::Vector2d(20, 0),
                          Eigen::Vector2d(65, 0),
                          15.0,
                          15.0
                      )},
                 },
	              .Keypoints =
                 {
	                      {1, 800, 100, M_PI, Start},
	                      {1,
	                       300,
	                       100,
	                       M_PI,
	                       Start.Add(1 * Duration::Minute).Add(-1)},
	                      {1, 100, 100, 0.0, Start.Add(1 * Duration::Minute)},
	                      {1,
	                       100,
	                       100,
	                       0.0,
	                       Start.Add(1 * Duration::Minute + 10 * Duration::Second)},
	                      {1,
	                       100,
	                       300,
	                       M_PI / 2,
	                       Start.Add(1 * Duration::Minute + 10 * Duration::Second)
	                           .Add(1)},
	                      {1, 100, 800, M_PI / 2, Start.Add(2 * Duration::Minute)},
	                      {1,
	                       100,
	                       800,
	                       -M_PI / 2,
	                       Start.Add(2 * Duration::Minute + 1 * Duration::Second)},
	                      {1,
	                       100,
	                       300,
	                       -M_PI / 2,
	                       Start.Add(3 * Duration::Minute + 10 * Duration::Second)
	                           .Add(-1)},
	                      {1,
	                       100,
	                       100,
	                       M_PI,
	                       Start.Add(3 * Duration::Minute + 10 * Duration::Second)},
	                      {1,
	                       100,
	                       100,
	                       M_PI,
	                       Start.Add(3 * Duration::Minute + 40 * Duration::Second)},
	                      {1,
	                       300,
	                       300,
	                       M_PI / 4,
	                       Start.Add(3 * Duration::Minute + 40 * Duration::Second)
	                           .Add(1)},
	                      {1, 800, 800, M_PI / 4, End},
                 },
	              .Interactions =
                 {
	                      InteractionData(
                         1,
                         1,
                         Start.Add(1 * Duration::Minute),
                         Start
                             .Add(1 * Duration::Minute + 10 * Duration::Second),
                         {1, 1, 2, 2}
                     ),
	                      InteractionData(
                         1,
                         1,
                         Start
                             .Add(3 * Duration::Minute + 10 * Duration::Second),
                         Start
                             .Add(3 * Duration::Minute + 40 * Duration::Second),
                         {1, 2, 2, 1}
                     ),
                 },
	              .AntPose = Eigen::Vector3d(0, 7, M_PI / 6),
	              .AntSize = 120,
	              .TagSize = 40,
	              .Color   = 30,
	              .IsQueen = false,
         }},
	         {3,
	          {
	              // 3 is a kind of forager
	              .Shape =
                 {
	                      {1,
	                       std::make_shared<Capsule>(
                          Eigen::Vector2d(-20, 0),
                          Eigen::Vector2d(-65, 0),
                          15.0,
                          15.0
                      )},
	                      {2,
	                       std::make_shared<Capsule>(
                          Eigen::Vector2d(20, 0),
                          Eigen::Vector2d(65, 0),
                          15.0,
                          15.0
                      )},
                 },
	              .Keypoints =
                 {
	                      {1, 100, 500, 0.0, Start},
	                      {1, 1000, 500, 0.0, Start.Add(1 * Duration::Minute)},
	                      {2,
	                       40,
	                       500,
	                       0.0,
	                       Start.Add(1 * Duration::Minute + 30 * Duration::Second)},
	                      {2, 500, 500, 0.0, Start.Add(2 * Duration::Minute)},
	                      {2,
	                       500,
	                       500,
	                       M_PI,
	                       Start.Add(2 * Duration::Minute + 1 * Duration::Second)},
	                      {2, 0, 500, M_PI, Start.Add(3 * Duration::Minute)},
	                      {1,
	                       960,
	                       500,
	                       M_PI,
	                       Start.Add(3 * Duration::Minute + 30 * Duration::Second)},
	                      {1,
	                       960,
	                       500,
	                       M_PI / 2,
	                       Start.Add(3 * Duration::Minute + 31 * Duration::Second)},
	                      {1, 960, 800, M_PI / 2, End},
                 },
	              .Interactions = {},
	              .AntPose      = Eigen::Vector3d(-7, -7, M_PI * 9.0 / 10.0),
	              .AntSize      = 120,
	              .TagSize      = 40,
	              .Color        = 30,
	              .IsQueen      = false,
         }},
    };
	NestTDDs = {
	    {
	        .RelativeFilePath = "nest.0000",
	        .HasFullFrame     = false,
	        .HasMovie         = true,
	        .HasConfig        = true,
	        .Start            = Start,
	        .End              = Start.Add(15 * Duration::Second),
	    },
	    {
	        .RelativeFilePath = "nest.0001",
	        .HasFullFrame     = true,
	        .HasMovie         = false,
	        .HasConfig        = true,
	        .Start            = Start.Add(15 * Duration::Second),
	        .End = Start.Add(3 * Duration::Minute + 15 * Duration::Second),
	    },
	    {
	        .RelativeFilePath = "nest.0002",
	        .HasFullFrame     = true,
	        .HasMovie         = false,
	        .HasConfig        = true,
	        .Start = Start.Add(3 * Duration::Minute + 15 * Duration::Second),
	        .End   = End,
	    },
	};
	ForagingTDDs = {
	    {
	        .RelativeFilePath = "foraging.0000",
	        .HasFullFrame     = true,
	        .HasMovie         = false,
	        .HasConfig        = true,
	        .Start            = Start,
	        .End              = End,
	    },
	};
}

void AntData::ComputeTagPosition(double & x,
                                 double & y,
                                 double & tagAngle,
                                 const Eigen::Vector3d & antPosition) const {
	priv::Isometry2Dd antToOrig(antPosition.z(),antPosition.block<2,1>(0,0));
	auto tagToAnt = priv::Isometry2Dd(AntPose.z(),AntPose.block<2,1>(0,0)).inverse();
	auto tagToOrig = antToOrig * tagToAnt;
	x = tagToOrig.translation().x();
	y = tagToOrig.translation().y();
	tagAngle = tagToOrig.angle();
}

} // namespace myrmidon
} // namespace fort
