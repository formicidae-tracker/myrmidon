#pragma once

#include "Config.hpp"

#include <fort/hermes/FrameReadout.pb.h>
#include <fort/myrmidon/utils/FileSystem.hpp>


namespace fort {
namespace myrmidon {

struct GeneratedData {
	std::vector<Time> NestTicks,ForagingTicks;
	std::vector<std::pair<SpaceID,Time>> Ticks;

	std::vector<AntTrajectory::Ptr> Trajectories;
	std::vector<AntInteraction::Ptr> Interactions;

	std::vector<std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr>> Frames;

	TagStatistics::ByTagID Statistics;

	static std::vector<Time> DrawFrameTicks(const Config & config);

	GeneratedData(const Config & config,
	              const fs::path & basepath);

	void GenerateFrameTicks(const Config & config,
	                        const fs::path & basepath);

	void AssignTicks(const std::vector<Time> & ticks,
	                 SpaceID spaceID,
	                 const std::vector<TDDData> & TDDs,
	                 const fs::path & basepath);


	void GenerateTrajectories(const Config & config);
	void GenerateInteractions(const Config & config);
	void GenerateFrames(const Config & config);
	void GenerateTagStatistics(const Config & config);

	void GenerateTrajectoriesFor(AntID antID, const AntData & ant);
	void GenerateInteractionsFor(AntID antID, const AntData & ant);
	void GenerateTagStatisticsFor(uint32_t tagID,const AntData & ant);

	std::vector<AntTrajectorySegment>
	FindTrajectorySegments(AntID antID,
	                       const Time & start,
	                       const Time & end);
};

} // namespace myrmidon
} // namespace fort
