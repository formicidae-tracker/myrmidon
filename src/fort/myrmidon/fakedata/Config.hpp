#pragma once

#include <fort/myrmidon/Types.hpp>
#include <fort/myrmidon/Shapes.hpp>

namespace fort {
namespace myrmidon {

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
	                const std::vector<uint32_t> & types);
};

struct AntData {
	TypedCapsuleList Shape;

	std::vector<Keypoint>        Keypoints;
	std::vector<InteractionData> Interactions;

	Eigen::Vector3d              AntPose;

	void ComputeTagPosition(double & xTag,
	                        double & yTag,
	                        double & tagAngle,
	                        const Eigen::Vector3d & antPosition) const;
};


struct Config {
	Time      Start,End;
	Duration  Framerate,Segment;
	float     Jitter;
	size_t    Width,Height;

	std::map<AntID,AntData> Ants;
	Config();
};

} // namespace myrmidon
} // namespace fort
