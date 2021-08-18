#include "UTestData.hpp"

#include <fstream>

#include "GeneratedData.hpp"
#include "HermesFileWriter.hpp"
#include "MovieWriter.hpp"
#include "CloseUpWriter.hpp"
#include "FrameDrawer.hpp"

#include <opencv2/core.hpp>

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/io/gzip_stream.h>

#include <fort/myrmidon/ExperimentFile.pb.h>
#include <fcntl.h>

#include <fort/myrmidon/priv/proto/TagStatisticsCache.hpp>
#include <fort/myrmidon/priv/proto/TDDCache.hpp>
#include <fort/myrmidon/priv/proto/TagCloseUpCache.hpp>

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace fort {
namespace myrmidon {

UTestData::UTestData(const fs::path & basedir) {
	try {
		BuildFakeData(basedir);
	} catch (const std::exception & e) {
		CleanUpFilesystem();
		throw;
	}
}

UTestData::~UTestData() {
	//	CleanUpFilesystem();
}

const fs::path & UTestData::Basedir() const {
	return d_basedir;
}


const std::vector<UTestData::TDDInfo> & UTestData::NestDataDirs() const {
	return d_nestTDDs;
}

const std::vector<UTestData::TDDInfo> & UTestData::ForagingDataDirs() const {
	return d_foragingTDDs;
}

const UTestData::TDDInfo & UTestData::NoConfigDataDir() const {
	return d_noConfigDir;
}

const UTestData::TDDInfo & UTestData::NoFamilyDataDir() const {
	return d_noFamilyDir;
}


const UTestData::TDDInfo & UTestData::ARTagDataDir() const {
	return d_ARTagDir;
}

const UTestData::TDDInfo & UTestData::WithVideoDataDir() const {
	return d_nestTDDs.front();
}


const UTestData::ExperimentInfo & UTestData::CurrentVersionFile() const {
	return d_experiments[2];
}

const UTestData::ExperimentInfo & UTestData::V0_1_File() const {
	return d_experiments[0];
}

const std::vector<UTestData::ExperimentInfo> & UTestData::ExperimentFiles() const {
	return d_experiments;
}

std::vector<UTestData::ExperimentInfo> UTestData::OldVersionFiles() const {
	return {d_experiments[0],d_experiments[1]};
}

const UTestData::ExperimentInfo & UTestData::FutureExperimentFile() const {
	return d_experiments.back();
}

const TagStatistics::ByTagID & UTestData::ExpectedTagStatistics() const {
	return d_statistics;
}

const std::vector<std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr>> &
UTestData::ExpectedFrames() const {
	return d_frames;
}

const std::vector<UTestData::ExpectedResult> & UTestData::ExpectedResults() const {
	return d_results;
}


void UTestData::CleanUpFilesystem() {
	if (d_basedir.empty()) {
		return;
	}
	fs::remove_all(d_basedir);
};

void UTestData::BuildFakeData(const fs::path & basedir) {
	auto start = Time::Now();
	d_basedir = basedir;
	GenerateFakedata();

	WriteFakedata();


	std::cerr << "Generated data in " << Time::Now().Sub(start) << std::endl;
}


void UTestData::GenerateFakedata() {
	GeneratedData gen(d_config,d_basedir);
	SaveFullExpectedResult(gen);
	GenerateTDDStructure();
	GenerateExperimentStructure();
	GenerateTruncatedResults();
}

void UTestData::SaveFullExpectedResult(const GeneratedData & gen) {
	ExpectedResult full;
	full.Start = Time::SinceEver();
	full.End = Time::Forever();
	full.MaximumGap = Duration::Hour;
	full.Trajectories = gen.Trajectories;
	full.Interactions = gen.Interactions;
	d_results.push_back(full);
	d_frames = gen.Frames;
	d_statistics = gen.Statistics;
}

void UTestData::SplitFullResultsWithTDDs() {
	for ( size_t i = 0; i < d_results.front().Trajectories.size(); ++i ) {
		const auto & t = d_results.front().Trajectories[i];
		SplitTrajectoryWithTDDs(t,d_results.front().Trajectories,t->Space == 1 ? d_nestTDDs : d_foragingTDDs);
		if ( i > 100 ) {
			throw std::logic_error("hey garcon");
		}
	}

	std::sort(d_results.front().Trajectories.begin(),
	          d_results.front().Trajectories.end(),
	          [](const AntTrajectory::Ptr & a,
	             const AntTrajectory::Ptr & b) -> bool {
		          auto aEnd = a->End();
		          auto bEnd = b->End();
		          if ( aEnd == bEnd ) {
			          return a->Ant < b->Ant;
		          }
		          return aEnd < bEnd;
	          });

	std::sort(d_results.front().Interactions.begin(),
	          d_results.front().Interactions.end(),
	          [](const AntInteraction::Ptr & a,
	             const AntInteraction::Ptr & b) {
		          return a->End < b->End;
	          });
}


void UTestData::SplitTrajectoryWithTDDs(const AntTrajectory::Ptr & t,
                                        std::vector<AntTrajectory::Ptr> & trajectories,
                                        const std::vector<TDDInfo> & tdds) {
	for ( const auto & tddInfo : tdds ) {
		if ( t->Start >= tddInfo.End
		     || tddInfo.Start > t->End()
		     || tddInfo.End >= t->End() ) {
			continue;
		}
		auto nt = std::make_shared<AntTrajectory>();
		nt->Ant = t->Ant;
		nt->Space = t->Space;
		size_t idx = 0;
		for(; idx < t->Positions.rows(); ++idx ) {
			nt->Start = t->Start.Add(t->Positions(idx,0) * Duration::Second.Nanoseconds());
			if ( nt->Start >= tddInfo.End ) {
				break;
			}
		}
		double offset = nt->Start.Sub(t->Start).Seconds();
		nt->Positions.resize(t->Positions.rows()-idx,5);
		nt->Positions = t->Positions.block(idx,0,t->Positions.rows()-idx,5);
		nt->Positions.col(0).array() -= offset;
		t->Positions.conservativeResize(idx,5);
		trajectories.push_back(nt);
		return;
	}
}

void UTestData::GenerateTruncatedResults() {
}


void UTestData::GenerateTDDStructure() {
	for ( const auto & tdd : d_config.NestTDDs ) {
		d_nestTDDs.push_back({
		                      .AbsoluteFilePath = d_basedir / tdd.RelativeFilePath,
		                      .Family = fort::tags::Family::Tag36h11,
		                      .HasFullFrame = tdd.HasFullFrame,
		                      .HasMovie = tdd.HasMovie,
		                      .HasConfig = tdd.HasConfig,
		                      .Start = tdd.Start,
		                      .End = tdd.End,
			});
	}

	for ( const auto & tdd : d_config.ForagingTDDs ) {
		d_foragingTDDs.push_back({
		                          .AbsoluteFilePath = d_basedir / tdd.RelativeFilePath,
		                          .Family = fort::tags::Family::Tag36h11,
		                          .HasFullFrame = tdd.HasFullFrame,
		                          .HasMovie = tdd.HasMovie,
		                          .HasConfig = tdd.HasConfig,
		                          .Start = tdd.Start,
		                          .End = tdd.End,
			});
	}


	d_noConfigDir =
		{
		 .AbsoluteFilePath =  d_basedir / "no-config.0000",
		 .Family = fort::tags::Family::Tag36h11,
		 .HasFullFrame = false,
		 .HasMovie = false,
		 .HasConfig = false,
		 .Start = d_config.Start,
		 .End = d_config.Start.Add(10*Duration::Second),
		};
	d_noFamilyDir =
		{
		 .AbsoluteFilePath =  d_basedir / "no-family.0000",
		 .Family = fort::tags::Family::Undefined,
		 .HasFullFrame = false,
		 .HasMovie = false,
		 .HasConfig = true,
		 .Start = d_config.Start,
		 .End = d_config.Start.Add(10*Duration::Second),
		};

	d_ARTagDir =
		{
		 .AbsoluteFilePath =  d_basedir / "ARTag.0000",
		 .Family = fort::tags::Family::Tag36ARTag,
		 .HasFullFrame = false,
		 .HasMovie = false,
		 .HasConfig = true,
		 .Start = d_config.Start,
		 .End = d_config.Start.Add(10*Duration::Second),
		};
}

void UTestData::GenerateExperimentStructure() {
	d_experiments =
		{
		 {
		  .AbsoluteFilePath = d_basedir / "test-v0.1.0.myrmidon",
		  .Version = semver::version("0.1.0"),
		 },
		 {
		  .AbsoluteFilePath = d_basedir / "test-v0.2.0.myrmidon",
		  .Version = semver::version("0.2.0"),
		 },
		 {
		  .AbsoluteFilePath = d_basedir / "test.myrmidon",
		  .Version = semver::version("0.3.0"),
		 },
		 {
		  .AbsoluteFilePath = d_basedir / "test-future.myrmidon",
		  .Version = semver::version("42.42.0"),
		 },
		};
}

void UTestData::WriteFakedata() {
	fs::create_directories(d_basedir);
	WriteTDDs();
	for ( const auto & e : d_experiments ) {
		WriteExperimentFile(e);
	}
}


void UTestData::WriteTDDs() {
	for ( auto & tddInfo : d_nestTDDs ) {
		WriteTDD(tddInfo,1);
	}
	for ( auto & tddInfo : d_foragingTDDs ) {
		WriteTDD(tddInfo,2);
	}
	WriteTDD(d_noConfigDir,2);
	WriteTDD(d_ARTagDir,2);
	WriteTDD(d_noFamilyDir,2);
}


class SegmentInfoWriter : public SegmentedDataWriter {
public:
	SegmentInfoWriter(UTestData::TDDInfo & tddInfo)
		: d_tddInfo(tddInfo)
		, d_start(Time::SinceEver()) {

	}

	void Prepare(size_t index) override {
		d_currentIndex = index;
		d_saved = false;

	}
	void WriteFrom(const IdentifiedFrame & data,
	               uint64_t frameID) override {
		if ( d_start.IsSinceEver() ) {
			d_start = data.FrameTime;
			d_startFrame = frameID;
		}
		d_endFrame = frameID;
		d_end = data.FrameTime;
		if ( d_saved ) {
			return;
		}
		d_saved = true;

		std::ostringstream relpath;
		relpath << "tracking." << std::setw(4) << std::setfill('0') << d_currentIndex << ".hermes";
		d_tddInfo.Segments.push_back({
		                              .URI = d_tddInfo.AbsoluteFilePath.filename() / "frames" / std::to_string(frameID),
		                              .FrameID = frameID,
		                              .Time = data.FrameTime,
		                              .RelativePath = relpath.str(),
			});
	}

	void Finalize(size_t index,bool last) override {
		if ( last == false ) {
			return;
		}

		d_tddInfo.Start = d_start;
		d_tddInfo.End = d_end.Add(1);
		d_tddInfo.StartFrame = d_startFrame;
		d_tddInfo.EndFrame = d_endFrame;
	}

private:
	UTestData::TDDInfo & d_tddInfo;
	bool     d_saved;
	size_t   d_currentIndex;
	Time     d_start,d_end;
	uint64_t d_startFrame,d_endFrame;
};

void UTestData::WriteTDD(TDDInfo & tddInfo,SpaceID spaceID) {
	fs::create_directories(tddInfo.AbsoluteFilePath / "ants");
	if ( tddInfo.HasConfig ) {
		WriteTDDConfig(tddInfo);
	}
	SegmentedDataWriter::List writers
		= {
		   std::make_shared<HermesFileWriter>(tddInfo.AbsoluteFilePath,d_config),
	};

	if ( tddInfo.Family != fort::tags::Family::Undefined ) {
		auto drawer = DrawerFactory(tddInfo.Family);
		writers.push_back(std::make_shared<CloseUpWriter>(tddInfo,
		                                                  drawer));

		if ( tddInfo.HasMovie ) {
			writers.push_back(std::make_shared<MovieWriter>(tddInfo.AbsoluteFilePath,d_config,drawer));
		}
	}

	// since segmentInfoWriter is modifying data, it should be the last one
	writers.push_back(std::make_shared<SegmentInfoWriter>(tddInfo));

	WriteSegmentedData(tddInfo,spaceID,writers);

}

void UTestData::WriteSegmentedData(TDDInfo & tddInfo,
                                   SpaceID spaceID,
                                   const SegmentedDataWriter::List & writers) {
	auto monoID = priv::TrackingDataDirectory::GetUID(tddInfo.AbsoluteFilePath);
	size_t i = 0;
	uint64_t frameID(0);

	for ( Time current = tddInfo.Start;
	      current.Before(tddInfo.End);
	      current = current.Add(d_config.Segment) ) {
		auto endTime = std::min(current.Add(d_config.Segment),tddInfo.End);
		auto begin = std::find_if(d_frames.begin(),
		                          d_frames.end(),
		                          [&](const std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr> & it ) {
			                          return it.first->FrameTime >= current;
		                          });
		auto end = std::find_if(begin,
		                        d_frames.end(),
		                        [&](const std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr> & it ) {
			                        return it.first->FrameTime >= endTime;
		                        });
		std::for_each(writers.begin(),writers.end(),
		              [&](const SegmentedDataWriter::Ptr & w) {
			              w->Prepare(i);
		              });
		for(auto iter = begin; iter != end ; ++iter ) {
			if ( iter->first->Space != spaceID ) {
				continue;
			}
			++frameID;
			std::for_each(writers.begin(),writers.end(),
			              [&](const SegmentedDataWriter::Ptr & w) {
				              if ( iter->first->FrameTime.HasMono() == false ) {
					              iter->first->FrameTime =
						              Time::FromTimestampAndMonotonic(iter->first->FrameTime.ToTimestamp(),
						                                              iter->first->FrameTime.Sub(d_config.Start).Nanoseconds()+2000,
						                                              monoID);
					              iter->second->FrameTime = iter->first->FrameTime;
				              }
				              w->WriteFrom(*iter->first,frameID);
			              });
		}
		std::for_each(writers.begin(),writers.end(),
		              [&](const SegmentedDataWriter::Ptr & w) {
			              w->Finalize(i,endTime == tddInfo.End);
		              });
		++i;
	}
}



void UTestData::WriteTDDConfig(const TDDInfo & info) {
	fs::path tddPath(info.AbsoluteFilePath);
	std::ofstream config((tddPath / "leto-final-config.yml").c_str());
	config << "experiment: " << tddPath.stem() << std::endl
	       << "legacy-mode: false" << std::endl
	       << "new-ant-roi: 300" << std::endl
	       << "new-ant-renew-period: 1m" << std::endl
	       << "stream:" << std::endl
	       << "  host:" << std::endl
	       << "  bitrate: 2000" << std::endl
	       << "  bitrate-max-ratio: 1.5" << std::endl
	       << "  quality: fast" << std::endl
	       << "  tuning: film" << std::endl
	       << "camera:" << std::endl
	       << "  strobe-delay: 0s" << std::endl
	       << "  strobe-duration: 1.5ms" << std::endl
	       << "  fps: " << int(std::round(Duration::Second.Seconds()/d_config.Framerate.Seconds())) << std::endl
	       << "  stub-path: \"\"" << std::endl
	       << "apriltag:" << std::endl;
	if ( info.Family != fort::tags::Family::Undefined ) {
		config << "  family: " << fort::tags::GetFamilyName(info.Family) <<  std::endl
		       << "  quad:" << std::endl
		       << "    decimate: 1" << std::endl
		       << "    sigma: 0" << std::endl
		       << "    refine-edges: false" << std::endl
		       << "    min-cluster-pixel: 25" << std::endl
		       << "    max-n-maxima: 10" << std::endl
		       << "    critical-angle-radian: 0.17453299" << std::endl
		       << "    max-line-mean-square-error: 10" << std::endl
		       << "    min-black-white-diff: 75" << std::endl
		       << "    deglitch: false" << std::endl;
	}
	config << "highlights: []" << std::endl;
}

void UTestData::WriteExperimentFile(const ExperimentInfo & info) {
	pb::Experiment e;

	e.set_author("myrmidon-tests");
	e.set_name("myrmidon test data");
	e.set_comment("automatically generated data");

	auto mt = e.add_custommeasurementtypes();
	mt->set_id(1);
	mt->set_name("head-tail");

	auto st = e.add_antshapetypes();
	st->set_id(1);
	st->set_name("head");
	st = e.add_antshapetypes();
	st->set_id(2);
	st->set_name("body");

	pb::FileHeader header;

	header.set_majorversion(info.Version.major);
	header.set_minorversion(info.Version.minor);
	pb::FileLine l;

	int fd = open(info.AbsoluteFilePath.c_str(),O_CREAT | O_TRUNC | O_RDWR | O_BINARY,0644 );
	if ( fd <= 0 ) {
		throw std::runtime_error("open('" + info.AbsoluteFilePath.string() + "',O_RDONLY | O_BINARY): " + std::to_string(errno));
	}
	auto file = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped = std::make_shared<google::protobuf::io::GzipOutputStream>(file.get());

	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(header, gunziped.get()) ) {
		throw std::runtime_error("could not write header message");
	}

	l.set_allocated_experiment(&e);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
		throw std::runtime_error("could not write experiment data");
	}
	l.release_experiment();

	pb::Space s;
	s.set_id(1);
	s.set_name("nest-area");
	for ( const auto & tddInfo : d_nestTDDs ) {
		s.add_trackingdatadirectories(fs::path(tddInfo.AbsoluteFilePath).filename());
	}

	l.set_allocated_space(&s);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
		throw std::runtime_error("could not write space data");
	}
	l.release_space();

	s.set_id(2);
	s.set_name("forage-area");
	s.clear_trackingdatadirectories();
	for ( const auto & tddInfo : d_foragingTDDs ) {
		s.add_trackingdatadirectories(fs::path(tddInfo.AbsoluteFilePath).filename());
	}

	l.set_allocated_space(&s);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
		throw std::runtime_error("could not write space data");
	}
	l.release_space();


	for ( const auto & [antID,ant] : d_config.Ants) {
		fort::myrmidon::pb::AntDescription a;
		a.set_id(antID);
		a.mutable_color()->set_r(255);
		auto identification = a.add_identifications();
		identification->set_id(antID-1);
		auto pose = identification->mutable_userdefinedpose();
		pose->mutable_position()->set_x(ant.AntPose.x());
		pose->mutable_position()->set_y(ant.AntPose.y());
		pose->set_angle(ant.AntPose.z());

		for ( const auto & [typeID,capsule] : ant.Shape ) {
			auto sh = a.add_shape();
			sh->set_type(typeID);
			sh->mutable_capsule()->mutable_c1()->set_x(capsule->C1().x());
			sh->mutable_capsule()->mutable_c1()->set_y(capsule->C1().y());
			sh->mutable_capsule()->mutable_c2()->set_x(capsule->C2().x());
			sh->mutable_capsule()->mutable_c2()->set_y(capsule->C2().y());
			sh->mutable_capsule()->set_r1(capsule->R1());
			sh->mutable_capsule()->set_r2(capsule->R2());
		}


		l.set_allocated_antdescription(&a);
		if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(l, gunziped.get()) ) {
			throw std::runtime_error("could not write ant data " + std::to_string(antID));
		}
		l.release_antdescription();
	}
}

void UTestData::ClearCachedData(const fs::path & tddPath) {
	fs::remove_all(tddPath / priv::proto::TDDCache::CACHE_FILENAME);
	fs::remove_all(tddPath / priv::proto::TagStatisticsCache::CACHE_PATH);
	fs::remove_all(tddPath / priv::proto::TagCloseUpCache::CACHE_PATH);
	fs::remove_all(tddPath / "ants/computed");

}

const std::shared_ptr<FrameDrawer> & UTestData::DrawerFactory(fort::tags::Family family) {
	if ( d_drawers.count(family) == 0 ) {
		auto d = std::make_shared<FrameDrawer>(family,d_config);
		d_drawers.insert({family,d});
	}
	return d_drawers.at(family);
}

void UTestData::SetMonotonicTimeToResults() {
	auto findTime
		= [&](const fort::Time & t, SpaceID spaceID) {
			  return std::find_if(d_frames.begin(),
			                      d_frames.end(),
			                      [&](const std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr> & it) {
				                      return it.first->Space == spaceID && it.first->FrameTime >= t;
			                      })->first->FrameTime;
		  };

	for ( const auto & t : d_results.front().Trajectories ) {
		t->Start = findTime(t->Start,t->Space);
	}
	for ( const auto & i : d_results.front().Interactions ) {
		i->Start = findTime(i->Start,i->Space);
		i->End = findTime(i->End,i->Space);
	}
}


} // namespace myrmidon
} // namespace fort