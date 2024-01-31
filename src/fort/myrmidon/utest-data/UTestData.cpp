#include "UTestData.hpp"

#include <fstream>

#include "CloseUpWriter.hpp"
#include "FrameDrawer.hpp"
#include "GeneratedData.hpp"
#include "HermesFileWriter.hpp"
#include "MovieWriter.hpp"

#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/util/delimited_message_util.h>

#include <fcntl.h>
#include <fort/myrmidon/ExperimentFile.pb.h>

#include <fort/myrmidon/priv/DataSegmenter.hpp>
#include <fort/myrmidon/priv/proto/TDDCache.hpp>
#include <fort/myrmidon/priv/proto/TagCloseUpCache.hpp>
#include <fort/myrmidon/priv/proto/TagStatisticsCache.hpp>

#include <semver.hpp>

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace fort {
namespace myrmidon {

fs::path UTestData::TempDirName() {
	std::ostringstream oss;
	oss << "fort-myrmidon-utestdata-" << getpid();
	return fs::temp_directory_path() / oss.str();
}

UTestData::UTestData(const fs::path &basedir) {
	try {
		BuildFakeData(basedir);
	} catch (const std::exception &e) {
		CleanUpFilesystem();
		throw;
	}
}

UTestData::~UTestData() {}

const fs::path &UTestData::Basedir() const {
	return d_basedir;
}

const std::vector<UTestData::TDDInfo> &UTestData::NestDataDirs() const {
	return d_nestTDDs;
}

const std::vector<UTestData::TDDInfo> &UTestData::ForagingDataDirs() const {
	return d_foragingTDDs;
}

const UTestData::TDDInfo &UTestData::CorruptedDataDir() const {
	return d_corruptedDir;
}

const UTestData::TDDInfo &UTestData::NoConfigDataDir() const {
	return d_noConfigDir;
}

const UTestData::TDDInfo &UTestData::NoFamilyDataDir() const {
	return d_noFamilyDir;
}

const UTestData::TDDInfo &UTestData::ARTagDataDir() const {
	return d_ARTagDir;
}

const UTestData::TDDInfo &UTestData::WithVideoDataDir() const {
	return d_nestTDDs.front();
}

const UTestData::ExperimentInfo &UTestData::CurrentVersionFile() const {
	return d_experiments[2];
}

const UTestData::ExperimentInfo &UTestData::V0_1_File() const {
	return d_experiments[0];
}

const std::vector<UTestData::ExperimentInfo> &
UTestData::ExperimentFiles() const {
	return d_experiments;
}

std::vector<UTestData::ExperimentInfo> UTestData::OldVersionFiles() const {
	return {d_experiments[0], d_experiments[1]};
}

const UTestData::ExperimentInfo &UTestData::FutureExperimentFile() const {
	return d_experiments.back();
}

const TagStatistics::ByTagID &UTestData::ExpectedTagStatistics() const {
	return d_statistics;
}

const std::vector<std::pair<IdentifiedFrame::Ptr, CollisionFrame::Ptr>> &
UTestData::ExpectedFrames() const {
	return d_frames;
}

const std::vector<UTestData::ExpectedResult> &
UTestData::ExpectedResults() const {
	return d_results;
}

void UTestData::CleanUpFilesystem() {
	if (d_basedir.empty()) {
		return;
	}
#ifndef NDEBUG
	std::cerr << "Cleanup files in " << d_basedir << std::endl;
#endif
	fs::remove_all(d_basedir);
	d_basedir = "";
};

void UTestData::BuildFakeData(const fs::path &basedir) {
	d_basedir = basedir;

#ifndef NDEBUG
	auto start = Time::Now();
	std::cerr << std::endl
	          << "Generating UTestData in " << d_basedir << std::endl;
#endif

	GenerateFakedata();

	WriteFakedata();

	GenerateSegmentedResults();
	GenerateMatchedResults();

#ifndef NDEBUG
	std::cerr << "Generated data in " << Time::Now().Sub(start) << std::endl;
#endif
}

void UTestData::GenerateFakedata() {
	GeneratedData gen(d_config, d_basedir);
	SaveFullExpectedResult(gen);
	GenerateTDDStructure();
	GenerateExperimentStructure();
}

void UTestData::SaveFullExpectedResult(const GeneratedData &gen) {
	ExpectedResult full;
	full.Start                   = Time::SinceEver();
	full.End                     = Time::Forever();
	full.MaximumGap              = 10 * Duration::Second;
	full.Trajectories            = gen.Trajectories;
	full.Interactions            = gen.Interactions;
	full.InteractionTrajectories = gen.Trajectories;
	d_results.push_back(full);
	d_frames     = gen.Frames;
	d_statistics = gen.Statistics;
}

void UTestData::SplitFullResultsWithTDDs() {
	for (size_t i = 0; i < d_results.front().Trajectories.size(); ++i) {
		const auto &t = d_results.front().Trajectories[i];
		SplitTrajectoryWithTDDs(
		    t,
		    d_results.front().Trajectories,
		    t->Space == 1 ? d_nestTDDs : d_foragingTDDs
		);
		if (i > 100) {
			throw std::logic_error("hey garcon");
		}
	}

	std::sort(
	    d_results.front().Trajectories.begin(),
	    d_results.front().Trajectories.end(),
	    [](const AntTrajectory::Ptr &a, const AntTrajectory::Ptr &b) -> bool {
		    auto aEnd = a->End();
		    auto bEnd = b->End();
		    if (aEnd == bEnd) {
			    return a->Ant < b->Ant;
		    }
		    return aEnd < bEnd;
	    }
	);

	std::sort(
	    d_results.front().Interactions.begin(),
	    d_results.front().Interactions.end(),
	    [](const AntInteraction::Ptr &a, const AntInteraction::Ptr &b) {
		    return a->End < b->End;
	    }
	);
}

void UTestData::SplitTrajectoryWithTDDs(
    const AntTrajectory::Ptr        &t,
    std::vector<AntTrajectory::Ptr> &trajectories,
    const std::vector<TDDInfo>      &tdds
) {
	for (const auto &tddInfo : tdds) {
		if (t->Start >= tddInfo.End || tddInfo.Start > t->End() ||
		    tddInfo.End >= t->End()) {
			continue;
		}
		auto nt    = std::make_shared<AntTrajectory>();
		nt->Ant    = t->Ant;
		nt->Space  = t->Space;
		size_t idx = 0;
		for (; idx < t->Positions.rows(); ++idx) {
			nt->Start = t->Start.Add(
			    t->Positions(idx, 0) * Duration::Second.Nanoseconds()
			);
			if (nt->Start >= tddInfo.End) {
				break;
			}
		}
		double offset = nt->Start.Sub(t->Start).Seconds();
		nt->Positions.resize(t->Positions.rows() - idx, 5);
		nt->Positions =
		    t->Positions.block(idx, 0, t->Positions.rows() - idx, 5);
		nt->Positions.col(0).array() -= offset;
		t->Positions.conservativeResize(idx, 5);
		trajectories.push_back(nt);
		return;
	}
}

void UTestData::GenerateMatchedResults() {
	d_results.push_back(d_results.front());
	auto &queenOnly   = d_results.back();
	queenOnly.Matches = fort::myrmidon::Matcher::AntMetaData("isQueen", true);
	queenOnly.VideoSegments.clear();
	queenOnly.Trajectories.erase(
	    std::remove_if(
	        queenOnly.Trajectories.begin(),
	        queenOnly.Trajectories.end(),
	        [](const AntTrajectory::Ptr &t) { return t->Ant != 1; }
	    ),
	    queenOnly.Trajectories.end()
	);
	std::set<AntTrajectory::Ptr> toKeep;
	queenOnly.Interactions.erase(
	    std::remove_if(
	        queenOnly.Interactions.begin(),
	        queenOnly.Interactions.end(),
	        [&](const AntInteraction::Ptr &i) {
		        if (i->IDs.first == 1) {
			        toKeep.insert(std::get<0>(i->Trajectories).first.Trajectory
			        );
			        toKeep.insert(std::get<0>(i->Trajectories).second.Trajectory
			        );
			        return false;
		        }
		        return true;
	        }
	    ),
	    queenOnly.Interactions.end()
	);

	queenOnly.InteractionTrajectories.erase(
	    std::remove_if(
	        queenOnly.InteractionTrajectories.begin(),
	        queenOnly.InteractionTrajectories.end(),
	        [&](const AntTrajectory::Ptr &t) {
		        return toKeep.count(t) == 0 && t->Ant != 1;
	        }
	    ),
	    queenOnly.InteractionTrajectories.end()
	);
}

void UTestData::GenerateTDDStructure() {
	for (const auto &tdd : d_config.NestTDDs) {
		d_nestTDDs.push_back({
		    .AbsoluteFilePath = d_basedir / tdd.RelativeFilePath,
		    .Family           = fort::tags::Family::Tag36h11,
		    .HasFullFrame     = tdd.HasFullFrame,
		    .HasMovie         = tdd.HasMovie,
		    .HasConfig        = tdd.HasConfig,
		    .IsCorrupted      = false,
		    .Start            = tdd.Start,
		    .End              = tdd.End,
		});
	}

	for (const auto &tdd : d_config.ForagingTDDs) {
		d_foragingTDDs.push_back({
		    .AbsoluteFilePath = d_basedir / tdd.RelativeFilePath,
		    .Family           = fort::tags::Family::Tag36h11,
		    .HasFullFrame     = tdd.HasFullFrame,
		    .HasMovie         = tdd.HasMovie,
		    .HasConfig        = tdd.HasConfig,
		    .IsCorrupted      = false,
		    .Start            = tdd.Start,
		    .End              = tdd.End,
		});
	}

	d_noConfigDir = {
	    .AbsoluteFilePath = d_basedir / "no-config.0000",
	    .Family           = fort::tags::Family::Tag36h11,
	    .HasFullFrame     = false,
	    .HasMovie         = false,
	    .HasConfig        = false,
	    .IsCorrupted      = false,
	    .Start            = d_config.Start,
	    .End              = d_config.Start.Add(10 * Duration::Second),
	};
	d_noFamilyDir = {
	    .AbsoluteFilePath = d_basedir / "no-family.0000",
	    .Family           = fort::tags::Family::Undefined,
	    .HasFullFrame     = false,
	    .HasMovie         = false,
	    .HasConfig        = true,
	    .IsCorrupted      = false,
	    .Start            = d_config.Start,
	    .End              = d_config.Start.Add(10 * Duration::Second),

	};

	d_corruptedDir = {
	    .AbsoluteFilePath = d_basedir / "corrupted.0000",
	    .Family           = fort::tags::Family::Tag36h11,
	    .HasFullFrame     = true,
	    .HasMovie         = true,
	    .HasConfig        = true,
	    .IsCorrupted      = true,
	    .Start            = d_config.Start,
	    .End              = d_config.Start.Add(9 * Duration::Second),
	};

	d_ARTagDir = {
	    .AbsoluteFilePath = d_basedir / "ARTag.0000",
	    .Family           = fort::tags::Family::Tag36ARTag,
	    .HasFullFrame     = false,
	    .HasMovie         = false,
	    .HasConfig        = true,
	    .IsCorrupted      = false,
	    .Start            = d_config.Start,
	    .End              = d_config.Start.Add(10 * Duration::Second),
	};
}

void UTestData::GenerateExperimentStructure() {
	d_experiments = {
	    {
	        .AbsoluteFilePath = d_basedir / "test-v0.1.0.myrmidon",

	        .Version = "0.1.0",
	    },
	    {
	        .AbsoluteFilePath = d_basedir / "test-v0.2.0.myrmidon",
	        .Version          = "0.2.0",
	    },
	    {
	        .AbsoluteFilePath = d_basedir / "test.myrmidon",
	        .Version          = "0.3.0",
	    },
	    {
	        .AbsoluteFilePath = d_basedir / "test-future.myrmidon",
	        .Version          = "42.42.0",
	    },
	};
}

void UTestData::WriteFakedata() {
	fs::create_directories(d_basedir);
	WriteTDDs();
	for (const auto &e : d_experiments) {
		WriteExperimentFile(e);
	}
}

void UTestData::WriteTDDs() {
	for (auto &tddInfo : d_nestTDDs) {
		WriteTDD(tddInfo, 1);
	}
	for (auto &tddInfo : d_foragingTDDs) {
		WriteTDD(tddInfo, 2);
	}
	WriteTDD(d_noConfigDir, 2);
	WriteTDD(d_ARTagDir, 2);
	WriteTDD(d_noFamilyDir, 2);
	WriteTDD(d_corruptedDir, 1);
}

class SegmentInfoWriter : public SegmentedDataWriter {
public:
	SegmentInfoWriter(UTestData::TDDInfo &tddInfo)
	    : d_tddInfo(tddInfo)
	    , d_start(Time::SinceEver()) {}

	void Prepare(size_t index) override {
		d_currentIndex = index;
		d_saved        = false;
	}

	void WriteFrom(const IdentifiedFrame &data, uint64_t frameID) override {
		if (d_start.IsSinceEver()) {
			d_start      = data.FrameTime;
			d_startFrame = frameID;
		}
		d_endFrame = frameID;
		d_end      = data.FrameTime;
		if (d_saved) {
			d_tddInfo.Segments.back().End = data.FrameTime.Add(1);
			return;
		}
		d_saved = true;

		std::ostringstream relpath;
		relpath << "tracking." << std::setw(4) << std::setfill('0')
		        << d_currentIndex << ".hermes";
		d_tddInfo.Segments.push_back({
		    .URI = d_tddInfo.AbsoluteFilePath.filename() / "frames" /
		           std::to_string(frameID),
		    .FrameID      = frameID,
		    .Start        = data.FrameTime,
		    .End          = data.FrameTime.Add(1),
		    .RelativePath = relpath.str(),
		});
	}

	void Finalize(size_t index, bool last) override {
		if (last == false) {
			return;
		}

		d_tddInfo.Start      = d_start;
		d_tddInfo.End        = d_end.Add(1);
		d_tddInfo.StartFrame = d_startFrame;
		d_tddInfo.EndFrame   = d_endFrame;
	}

private:
	UTestData::TDDInfo &d_tddInfo;
	bool                d_saved;
	size_t              d_currentIndex;
	Time                d_start, d_end;
	uint64_t            d_startFrame, d_endFrame;
};

void TruncateFile(const std::filesystem::path &filepath, int bytes) {
#ifndef NDEBUG
	std::cerr << "Truncating " << bytes << " bytes of " << filepath
	          << std::endl;
#endif // NDEBUG

	FILE *file = fopen(filepath.c_str(), "r+");
	if (file == nullptr) {
		throw std::runtime_error(
		    "open('" + filepath.string() +
		    "',O_CREAT | O_TRUNC | O_RDWR | O_BINARY): " + std::to_string(errno)
		);
	}
	if (fseeko(file, -bytes, SEEK_END) != 0) {
		throw std::runtime_error(
		    "fseeko('" + filepath.string() + "'," + std::to_string(-bytes) +
		    ",SEEK_END): " + std::to_string(errno)
		);
	}
	auto offset = ftello(file);
	std::cerr << "File is " << offset + bytes << " bytes " << std::endl;

	if (ftruncate(fileno(file), offset) != 0) {
		throw std::runtime_error(
		    "ftruncate('" + filepath.string() + "'," + std::to_string(offset) +
		    "): " + std::to_string(errno)
		);
	}

	fclose(file);
}

void UTestData::WriteTDD(TDDInfo &tddInfo, SpaceID spaceID) {
	fs::create_directories(tddInfo.AbsoluteFilePath / "ants");
	if (tddInfo.HasConfig) {
		WriteTDDConfig(tddInfo);
	}

	SegmentedDataWriter::List writers = {
	    std::make_shared<HermesFileWriter>(tddInfo.AbsoluteFilePath, d_config),
	};

	if (tddInfo.Family != fort::tags::Family::Undefined) {
		auto drawer = DrawerFactory(tddInfo.Family);
		writers.push_back(std::make_shared<CloseUpWriter>(tddInfo, drawer));

		if (tddInfo.HasMovie) {
			writers.push_back(std::make_shared<MovieWriter>(
			    tddInfo.AbsoluteFilePath,
			    d_config,
			    drawer
			));
		}
	}

	// since segmentInfoWriter is modifying data, it should be the last one
	writers.push_back(std::make_shared<SegmentInfoWriter>(tddInfo));

	WriteSegmentedData(tddInfo, spaceID, writers);

	if (tddInfo.IsCorrupted == true) {
		TruncateFile(
		    tddInfo.AbsoluteFilePath /
		        std::prev(tddInfo.Segments.end(), 2)->RelativePath,
		    50
		);
	}
}

void UTestData::WriteSegmentedData(
    TDDInfo &tddInfo, SpaceID spaceID, const SegmentedDataWriter::List &writers
) {
	auto monoID = priv::TrackingDataDirectory::GetUID(tddInfo.AbsoluteFilePath);
	size_t   i  = 0;
	uint64_t frameID(0);

	Duration increment =
	    tddInfo.IsCorrupted ? (tddInfo.End.Sub(tddInfo.Start).Nanoseconds() / 3)
	                        : d_config.Segment;

	for (Time current = tddInfo.Start; current.Before(tddInfo.End);
	     current      = current.Add(increment)) {
		auto endTime = std::min(current.Add(increment), tddInfo.End);
		auto begin   = std::find_if(
            d_frames.begin(),
            d_frames.end(),
            [&](const std::pair<IdentifiedFrame::Ptr, CollisionFrame::Ptr> &it
            ) { return it.first->FrameTime >= current; }
        );
		auto end = std::find_if(
		    begin,
		    d_frames.end(),
		    [&](const std::pair<IdentifiedFrame::Ptr, CollisionFrame::Ptr> &it
		    ) { return it.first->FrameTime >= endTime; }
		);
		std::for_each(
		    writers.begin(),
		    writers.end(),
		    [&](const SegmentedDataWriter::Ptr &w) { w->Prepare(i); }
		);
		for (auto iter = begin; iter != end; ++iter) {
			if (iter->first->Space != spaceID) {
				continue;
			}
			++frameID;
			std::for_each(
			    writers.begin(),
			    writers.end(),
			    [&](const SegmentedDataWriter::Ptr &w) {
				    if (iter->first->FrameTime.HasMono() == false) {
					    iter->first->FrameTime =
					        Time::FromTimestampAndMonotonic(
					            iter->first->FrameTime.ToTimestamp(),
					            iter->first->FrameTime.Sub(d_config.Start)
					                    .Nanoseconds() +
					                2000,
					            monoID
					        );
					    iter->second->FrameTime = iter->first->FrameTime;
				    }
				    w->WriteFrom(*iter->first, frameID);
			    }
			);
		}
		std::for_each(
		    writers.begin(),
		    writers.end(),
		    [&](const SegmentedDataWriter::Ptr &w) {
			    w->Finalize(i, endTime == tddInfo.End);
		    }
		);
		++i;
	}
}

void UTestData::WriteTDDConfig(const TDDInfo &info) {
	fs::path      tddPath(info.AbsoluteFilePath);
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
	       << "  fps: "
	       << int(std::round(
	              double(d_config.Framerate.Num) / d_config.Framerate.Den
	          ))
	       << std::endl
	       << "  stub-path: \"\"" << std::endl
	       << "apriltag:" << std::endl;
	if (info.Family != fort::tags::Family::Undefined) {
		config << "  family: " << fort::tags::GetFamilyName(info.Family)
		       << std::endl
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

void UTestData::WriteExperimentFile(const ExperimentInfo &info) {
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

	auto md = e.add_antmetadata();
	md->set_name("isQueen");
	auto dv = md->mutable_defaultvalue();
	dv->set_type(pb::AntStaticValue::BOOL);
	dv->set_boolvalue(false);

	pb::FileHeader header;

	semver::version version(info.Version);

	header.set_majorversion(version.major);
	header.set_minorversion(version.minor);
	pb::FileLine l;

	int fd = open(
	    info.AbsoluteFilePath.c_str(),
	    O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
	    0644
	);
	if (fd <= 0) {
		throw std::runtime_error(
		    "open('" + info.AbsoluteFilePath.string() +
		    "',O_RDONLY | O_BINARY): " + std::to_string(errno)
		);
	}
	auto file = std::make_shared<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gunziped =
	    std::make_shared<google::protobuf::io::GzipOutputStream>(file.get());

	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(
	        header,
	        gunziped.get()
	    )) {
		throw std::runtime_error("could not write header message");
	}

	l.set_allocated_experiment(&e);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(
	        l,
	        gunziped.get()
	    )) {
		throw std::runtime_error("could not write experiment data");
	}
	l.release_experiment();

	pb::Space s;
	s.set_id(1);
	s.set_name("nest-area");
	for (const auto &tddInfo : d_nestTDDs) {
		s.add_trackingdatadirectories(
		    fs::path(tddInfo.AbsoluteFilePath).filename()
		);
	}

	l.set_allocated_space(&s);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(
	        l,
	        gunziped.get()
	    )) {
		throw std::runtime_error("could not write space data");
	}
	l.release_space();

	s.set_id(2);
	s.set_name("forage-area");
	s.clear_trackingdatadirectories();
	for (const auto &tddInfo : d_foragingTDDs) {
		s.add_trackingdatadirectories(
		    fs::path(tddInfo.AbsoluteFilePath).filename()
		);
	}

	l.set_allocated_space(&s);
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(
	        l,
	        gunziped.get()
	    )) {
		throw std::runtime_error("could not write space data");
	}
	l.release_space();

	for (const auto &[antID, ant] : d_config.Ants) {
		fort::myrmidon::pb::AntDescription a;
		if (ant.IsQueen == true) {
			auto nv = a.add_namedvalues();
			nv->set_name("isQueen");
			nv->mutable_value()->set_type(pb::AntStaticValue::BOOL);
			nv->mutable_value()->set_boolvalue(true);
		}
		a.set_id(antID);
		a.mutable_color()->set_r(255);
		auto identification = a.add_identifications();
		identification->set_id(antID - 1);
		auto pose = identification->mutable_userdefinedpose();
		pose->mutable_position()->set_x(ant.AntPose.x());
		pose->mutable_position()->set_y(ant.AntPose.y());
		pose->set_angle(ant.AntPose.z());

		for (const auto &[typeID, capsule] : ant.Shape) {
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
		if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(
		        l,
		        gunziped.get()
		    )) {
			throw std::runtime_error(
			    "could not write ant data " + std::to_string(antID)
			);
		}
		l.release_antdescription();
	}
}

void UTestData::ClearCachedData(const fs::path &tddPath) {
	fs::remove_all(tddPath / priv::proto::TDDCache::CACHE_FILENAME);
	fs::remove_all(tddPath / priv::proto::TagStatisticsCache::CACHE_PATH);
	fs::remove_all(tddPath / priv::proto::TagCloseUpCache::CACHE_PATH);
	fs::remove_all(tddPath / "ants/computed");
}

const std::shared_ptr<FrameDrawer> &
UTestData::DrawerFactory(fort::tags::Family family) {
	if (d_drawers.count(family) == 0) {
		auto d = std::make_shared<FrameDrawer>(family, d_config);
		d_drawers.insert({family, d});
	}
	return d_drawers.at(family);
}

void UTestData::SetMonotonicTimeToResults() {
	auto findTime = [&](const fort::Time &t, SpaceID spaceID) {
		return std::find_if(
		           d_frames.begin(),
		           d_frames.end(),
		           [&](const std::
		                   pair<IdentifiedFrame::Ptr, CollisionFrame::Ptr> &it
		           ) {
			           return it.first->Space == spaceID &&
			                  it.first->FrameTime >= t;
		           }
		)->first->FrameTime;
	};

	for (const auto &t : d_results.front().Trajectories) {
		t->Start = findTime(t->Start, t->Space);
	}
	for (const auto &i : d_results.front().Interactions) {
		i->Start = findTime(i->Start, i->Space);
		i->End   = findTime(i->End, i->Space);
	}
}

std::vector<AntInteraction::Ptr> UTestData::ExpectedResult::Summarized() const {
	std::vector<AntInteraction::Ptr> res;
	for (const auto &i : Interactions) {
		auto ii          = std::make_shared<AntInteraction>(*i);
		ii->Trajectories = std::make_pair(
		    priv::DataSegmenter::SummarizeTrajectorySegment(
		        std::get<0>(i->Trajectories).first
		    ),
		    priv::DataSegmenter::SummarizeTrajectorySegment(
		        std::get<0>(i->Trajectories).second
		    )
		);
		res.push_back(ii);
	}
	return res;
}

void UTestData::GenerateSegmentedResults() {
	for (const auto &tddInfo : d_nestTDDs) {
		GenerateSegmentedResult(d_results.front(), tddInfo, 1);
	}

	for (const auto &tddInfo : d_foragingTDDs) {
		GenerateSegmentedResult(d_results.front(), tddInfo, 2);
	}
}

void UTestData::GenerateSegmentedResult(
    ExpectedResult &result, const TDDInfo &info, SpaceID spaceID
) {
	if (info.HasMovie == true) {
		GenerateMovieSegmentData(result, info, spaceID);
	}
	// Other segmented data to generate
}

std::string MovieSegmentName(size_t index) {
	std::ostringstream oss;
	oss << "stream." << std::setw(4) << std::setfill('0') << index << ".mp4";
	return oss.str();
}

void MatchMovieData(
    VideoSegment                                                      &segment,
    std::vector<std::pair<IdentifiedFrame::Ptr, CollisionFrame::Ptr>> &frames,
    UTestData::ExpectedResult                                         &result,
    const Time                                                        &start,
    const Time                                                        &end
) {
	for (const auto &[identified, collided] : frames) {
		if (identified->Space != segment.Space ||
		    start > identified->FrameTime || identified->FrameTime >= end) {
			continue;
		}
		segment.Data.push_back({
		    .Position   = uint32_t(segment.Data.size()),
		    .Time       = identified->FrameTime,
		    .Identified = identified,
		    .Collided   = collided,
		});
	}
	segment.End = segment.Data.size();

	for (auto &d : segment.Data) {
		std::copy_if(
		    result.Trajectories.begin(),
		    result.Trajectories.end(),
		    std::back_inserter(d.Trajectories),
		    [&](const AntTrajectory::Ptr &t) -> bool {
			    return t->Space == segment.Space && t->Start <= d.Time &&
			           d.Time <= t->End();
		    }
		);

		std::copy_if(
		    result.Interactions.begin(),
		    result.Interactions.end(),
		    std::back_inserter(d.Interactions),
		    [&](const AntInteraction::Ptr &i) -> bool {
			    return segment.Space == i->Space && i->Start <= d.Time &&
			           d.Time <= i->End;
		    }
		);
	}
}

void UTestData::GenerateMovieSegmentData(
    ExpectedResult &result, const TDDInfo &info, SpaceID spaceID
) {
	auto &videoSegments = result.VideoSegments[spaceID];
	videoSegments.clear();
	videoSegments.reserve(info.Segments.size());

	size_t index = -1;
	for (const auto &segment : info.Segments) {
		++index;

		videoSegments.push_back(
		    {.Space = spaceID,
		     .AbsoluteFilePath =
		         (info.AbsoluteFilePath / MovieSegmentName(index)).string(),
		     .Begin = 0}
		);
		MatchMovieData(
		    videoSegments.back(),
		    d_frames,
		    result,
		    info.Start,
		    info.End
		);
	}
#ifndef NDEBUG
	for (const auto &s : videoSegments) {
		std::cerr << "VideoSegment{ Space = " << s.Space
		          << " , AbsoluteFilePath = " << s.AbsoluteFilePath
		          << " , Begin = " << s.Begin << " , End = " << s.End
		          << " , Data.size() = " << s.Data.size() << "}" << std::endl;
	}
#endif // NDEBUG
}

const Config &UTestData::Config() const {
	return d_config;
}

} // namespace myrmidon
} // namespace fort
