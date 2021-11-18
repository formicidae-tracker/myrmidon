#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <map>

#include <fort/time/Time.hpp>

#include "Typedefs.hpp"

namespace fort {
namespace myrmidon {

/**
 * Reports information about a tracking data directory.
 */
struct TrackingDataDirectoryInfo {
	/**
	 * The URI used in the GUI to designate the tracking data directory
	 */
	std::string URI;
	/**
	 * The absolute filepath on the directory on the system
	 */
	std::string AbsoluteFilePath;
	/**
	 * The number of Frames in this directory
	 */
	uint64_t    Frames;
	/**
	 * The first frame Time
	 */
	Time        Start;
	/**
	 * The last frame Time
	 */
	Time        End;
};

/**
 * Reports global tracking data stats for a Space
 */
struct SpaceDataInfo {
	/**
	 * The URI used to designate the Space
	 */
	std::string URI;
	/**
	 * The name of the Space
	 */
	std::string Name;
	/**
	 * The number of frame in the Space
	 */
	uint64_t    Frames;
	/**
	 * The first Time present in the Space
	 */
	Time        Start;
	/**
	 * The last Time present in the Space
	 */
	Time        End;
	/**
	 * Infos for all tracking data directories, ordered in Time
	 */
	std::vector<TrackingDataDirectoryInfo> TrackingDataDirectories;
};

/**
 * Reports global tracking data stats for an Experiment
 */
struct ExperimentDataInfo {
	/**
	 * The number of tracked frame in the Experiment
	 */
	uint64_t Frames;
	/**
	 * The Time of the first tracked frame
	 */
	Time     Start;
	/**
	 * the Time of the last tracked frame
	 */
	Time     End;

	/**
	 * Data infos for all Space
	 */
	std::map<SpaceID,SpaceDataInfo> Spaces;
};

} // namespace myrmidon
} // namespace fort
