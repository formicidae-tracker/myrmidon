#pragma once

#include <cstdint>
#include <fort/video/Types.hpp>
#include <map>
#include <string>
#include <vector>

#include <Eigen/Core>

#include <fort/time/Time.hpp>

#include <fort/myrmidon/Shapes.hpp>
#include <fort/myrmidon/types/Collision.hpp>
#include <fort/myrmidon/types/Typedefs.hpp>

namespace fort {
namespace myrmidon {

struct TDDData {
	std::string RelativeFilePath;
	bool        HasFullFrame, HasMovie, HasConfig;
	Time        Start, End;
};

struct Keypoint {
	SpaceID Space;
	double  X, Y, Angle;
	Time    At;
};

struct InteractionData {
	AntID            Other;
	Time             Start, End;
	SpaceID          Space;
	InteractionTypes Types;
	InteractionData(
	    AntID                        other,
	    SpaceID                      spaceID,
	    const Time                  &start,
	    const Time                  &end,
	    const std::vector<uint32_t> &types
	);
};

struct AntData {
	TypedCapsuleList Shape;

	std::vector<Keypoint>        Keypoints;
	std::vector<InteractionData> Interactions;

	Eigen::Vector3d AntPose;
	size_t          AntSize, TagSize;
	uint8_t         Color;
	bool            IsQueen;

	void ComputeTagPosition(
	    double	            &xTag,
	    double	            &yTag,
	    double	            &tagAngle,
	    const Eigen::Vector3d &antPosition
	) const;
};

struct Config {
	Time              Start, End;
	video::Ratio<int> Framerate;
	Duration          Segment;
	float             Jitter;
	size_t            Width, Height;

	std::vector<TDDData> NestTDDs, ForagingTDDs;

	std::map<AntID, AntData> Ants;
	Config();
};

} // namespace myrmidon
} // namespace fort
