#pragma once

#include <fort/myrmidon/Types.hpp>
#include <fort/myrmidon/Matchers.hpp>
#include <fort/myrmidon/utils/FileSystem.hpp>
#include <semver.hpp>

#include "Config.hpp"

namespace fort {
namespace hermes {
class FrameReadout;
}
namespace myrmidon {

struct GeneratedData;

class Fakedata {
public:
	static fs::path GenerateTempDir();

	static void ClearCacheData(const std::string & tddPath);


	struct TDDInfo {
		std::string AbsoluteFilePath;
		bool        HasFullFrame;
		bool        HasMovie;
		bool        HasConfig;
		Time        Start,End;
	};

	struct ExperimentInfo {
		std::string AbsoluteFilePath;
		semver::version Version;
	};

	struct ExpectedResult {
		Time          Start,End;
		Duration      MaximumGap;
		Matcher::Ptr  Matches;
		std::vector<AntTrajectory::Ptr> Trajectories;
		std::vector<AntInteraction::Ptr> Interactions;
	};

	Fakedata(const fs::path & basedir);
	~Fakedata();


	std::vector<TDDInfo> NestDataDirs() const;

	std::vector<TDDInfo> ForagingDataDirs() const;

	TDDInfo WithVideoDataDir() const;


	ExperimentInfo ExperimentFile() const;
	std::vector<ExperimentInfo> OldVersionFiles() const;
	ExperimentInfo FutureExperimentFile() const;



	const std::vector<std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr>> & PunctualResult() const;

	const std::vector<ExpectedResult> & QueryResult() const;
private:
	typedef std::vector<std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr>> FrameList;

	void BuildFakeData(const fs::path & basedir);
	void CleanUpFilesystem();

	void GenerateFakedata();


	void SaveFullExpectedResult(const GeneratedData & gen);
	void GenerateTruncatedResults();
	void GenerateTDDStructure();

	void WriteFakedata();
	void WriteTDDs();
	void WriteTDD(const TDDInfo & info,SpaceID spaceID);
	void WriteHermesFiles(const TDDInfo & info,
	                      SpaceID spaceID);

	void ForeachSegment(const TDDInfo & tddInfo,
	                    std::function<void(size_t,FrameList::const_iterator,FrameList::const_iterator,bool)> apply);

	void WriteHermesFile(size_t index,
	                     const std::string & basepath,
	                     SpaceID spaceID,
	                     uint64_t & frameID,
	                     FrameList::const_iterator begin,
	                     FrameList::const_iterator end,
	                     bool last);

	void FillReadout(hermes::FrameReadout * ro,
	                 uint64_t frameID,
	                 const IdentifiedFrame::Ptr & identified);


	fs::path d_basedir;
	Config   d_config;

	std::vector<TDDInfo> d_nestTDDs,d_forageTDDs;

	std::vector<ExpectedResult> d_results;
	FrameList                   d_frames;
};

} // namespace myrmidon
} // namespace fort
