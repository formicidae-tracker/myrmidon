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
class SegmentedDataWriter;
typedef std::shared_ptr<SegmentedDataWriter> SegmentedDataWriterPtr;
typedef std::vector<SegmentedDataWriterPtr> SegmentedDataWriterList;

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
	void WriteTDDConfig(const TDDInfo & info);



	SegmentedDataWriterPtr HermesWriter(const TDDInfo &);
	SegmentedDataWriterPtr MovieWriter(const TDDInfo &);
	SegmentedDataWriterPtr CloseUpWriter(const TDDInfo & );

	void WriteSegmentedData(const TDDInfo & info,
	                        SpaceID spaceID,
	                        const SegmentedDataWriterList & writers);


	fs::path d_basedir;
	Config   d_config;

	std::vector<TDDInfo> d_nestTDDs,d_forageTDDs;

	std::vector<ExpectedResult> d_results;
	FrameList                   d_frames;
};

} // namespace myrmidon
} // namespace fort
