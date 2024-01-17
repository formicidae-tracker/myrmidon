#pragma once

#include <map>
#include <filesystem>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

#include <fort/time/Time.hpp>

#include <fort/tags/fort-tags.hpp>

#include <fort/myrmidon/utils/FileSystem.hpp>

#include <fort/myrmidon/types/AntInteraction.hpp>
#include <fort/myrmidon/types/AntTrajectory.hpp>
#include <fort/myrmidon/types/TagStatistics.hpp>
#include <fort/myrmidon/Video.hpp>

#include <fort/myrmidon/Matchers.hpp>

#include "Config.hpp"

namespace fs = std::filesystem;

namespace fort {
namespace hermes {
class FrameReadout;
}
namespace myrmidon {

namespace priv {
class TagCloseUp;
}

struct GeneratedData;
class SegmentedDataWriter;
typedef std::shared_ptr<SegmentedDataWriter> SegmentedDataWriterPtr;
typedef std::vector<SegmentedDataWriterPtr> SegmentedDataWriterList;

class FrameDrawer;

class UTestData {
public:

	static void ClearCachedData(const fs::path & tddPath);

	struct SegmentInfo {
		std::string URI;
		uint64_t    FrameID;
		fort::Time  Start,End;
		std::string RelativePath;
	};

	struct TDDInfo {
		fs::path           AbsoluteFilePath;
		fort::tags::Family Family;
		bool               HasFullFrame;
		bool               HasMovie;
		bool               HasConfig;
		bool               IsCorrupted;
		Time               Start, End;
		uint64_t           StartFrame, EndFrame;

		std::multimap<uint64_t, std::pair<fs::path, std::shared_ptr<TagID>>>
		                                                     TagCloseUpFiles;
		std::vector<std::shared_ptr<const priv::TagCloseUp>> TagCloseUps;
		std::vector<SegmentInfo>                             Segments;
	};

	struct ExperimentInfo {
		fs::path    AbsoluteFilePath;
		std::string Version;
	};

	struct ExpectedResult {
		Time          Start,End;
		Duration      MaximumGap;
		Matcher::Ptr  Matches;
		std::vector<AntTrajectory::Ptr> Trajectories;
		std::vector<AntTrajectory::Ptr> InteractionTrajectories;
		std::vector<AntInteraction::Ptr> Interactions;
		std::vector<AntInteraction::Ptr> Summarized() const;
		std::map<SpaceID,std::vector<VideoSegment>> VideoSegments;
	};

	static fs::path TempDirName();

	UTestData(const fs::path & basedir);
	~UTestData();

	const fs::path & Basedir() const;

	const std::vector<TDDInfo> & NestDataDirs() const;

	const std::vector<TDDInfo> & ForagingDataDirs() const;

	const TDDInfo & NoConfigDataDir() const;

	const TDDInfo & NoFamilyDataDir() const;

	const TDDInfo & ARTagDataDir() const;

	const TDDInfo & CorruptedDataDir() const;

	const TDDInfo & WithVideoDataDir() const;

	const ExperimentInfo & CurrentVersionFile() const;
	const ExperimentInfo & V0_1_File() const;

	const std::vector<ExperimentInfo> & ExperimentFiles() const;
	std::vector<ExperimentInfo> OldVersionFiles() const;

	const ExperimentInfo & FutureExperimentFile() const;

	const TagStatistics::ByTagID & ExpectedTagStatistics() const;

	const std::vector<std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr>> & ExpectedFrames() const;

	const std::vector<ExpectedResult> &ExpectedResults() const;

	const fort::myrmidon::Config &Config() const;

	void CleanUpFilesystem();

private:
	typedef std::vector<std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr>> FrameList;

	void BuildFakeData(const fs::path & basedir);

	void GenerateFakedata();
	void GenerateExperimentStructure();

	void SaveFullExpectedResult(const GeneratedData & gen);
	void SetMonotonicTimeToResults();

	void SplitFullResultsWithTDDs();

	void SplitTrajectoryWithTDDs(const AntTrajectory::Ptr & t,
	                             std::vector<AntTrajectory::Ptr> & trajectories,
	                             const std::vector<TDDInfo> & tdds);

	void GenerateMatchedResults();
	void GenerateSegmentedResults();
	void GenerateSegmentedResult(ExpectedResult & result,
	                             const TDDInfo & tddInfo,
	                             SpaceID space);

	void GenerateMovieSegmentData(ExpectedResult & result,
	                              const TDDInfo & tddInfo,
	                              SpaceID space);
	void GenerateTDDStructure();

	void WriteFakedata();
	void WriteTDDs();
	void WriteTDD(TDDInfo & info,SpaceID spaceID);
	void WriteTDDConfig(const TDDInfo & info);


	void WriteSegmentedData(TDDInfo & info,
	                        SpaceID spaceID,
	                        const SegmentedDataWriterList & writers);

	void WriteExperimentFile(const ExperimentInfo & expInfo);


	const std::shared_ptr<FrameDrawer> & DrawerFactory(fort::tags::Family family);


	fs::path d_basedir;
	fort::myrmidon::Config d_config;

	std::vector<TDDInfo> d_nestTDDs,d_foragingTDDs;
	TDDInfo              d_noConfigDir,d_ARTagDir,d_noFamilyDir,d_corruptedDir;
	std::vector<ExperimentInfo> d_experiments;

	std::vector<ExpectedResult> d_results;
	FrameList                   d_frames;
	TagStatistics::ByTagID      d_statistics;

	std::map<fort::tags::Family,std::shared_ptr<FrameDrawer>> d_drawers;
};

} // namespace myrmidon
} // namespace fort
