#pragma once

#include "Config.hpp"

#include <fort/hermes/FrameReadout.pb.h>


namespace fort {
namespace myrmidon {

struct GeneratedData {
	std::vector<Time> NestTicks,ForageTicks;
	std::vector<std::pair<SpaceID,Time>> Ticks;

	std::vector<AntTrajectory::Ptr> Trajectories;
	std::vector<AntInteraction::Ptr> Interactions;

	std::vector<std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr>> Frames;

	static std::vector<Time> DrawFrameTicks(const Config & config);

	GeneratedData(const Config & config);

	void GenerateFrameTicks(const Config & config);
	void GenerateTrajectories(const Config & config);
	void GenerateInteractions(const Config & config);
	void GenerateFrames(const Config & config);


	void GenerateTrajectoriesFor(AntID antID, const AntData & ant);
	void GenerateInteractionsFor(AntID antID, const AntData & ant);

	std::tuple<AntTrajectorySegment,Time,Time>
	FindTrajectorySegment(AntID antID,
	                      const Time & start,
	                      const Time & end);
};

} // namespace myrmidon
} // namespace fort
