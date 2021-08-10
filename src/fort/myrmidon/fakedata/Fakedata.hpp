#pragma once

#include <fort/myrmidon/Types.hpp>
#include <fort/myrmidon/Matchers.hpp>
#include <fort/myrmidon/utils/FileSystem.hpp>
#include <semver.hpp>

#include "Config.hpp"

namespace fort {
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


	void BuildFakeData(const fs::path & basedir);
	void CleanUpFilesystem();

	void GenerateFakedata();


	void SaveFullExpectedResult(const GeneratedData & gen);
	void GenerateTruncatedResults();


	void WriteFakedata();


	fs::path d_basedir;
	Config   d_config;

	std::vector<ExpectedResult> d_results;
	std::vector<std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr>> d_frames;
};

} // namespace myrmidon
} // namespace fort
