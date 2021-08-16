#include "Fakedata.hpp"

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

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace fort {
namespace myrmidon {

Fakedata::Fakedata(const fs::path & basedir) {
	try {
		BuildFakeData(basedir);
	} catch (const std::exception & e) {
		CleanUpFilesystem();
		throw;
	}
}

Fakedata::~Fakedata() {
	//	CleanUpFilesystem();
}



void Fakedata::CleanUpFilesystem() {
	if (d_basedir.empty()) {
		return;
	}
	fs::remove_all(d_basedir);
};

void Fakedata::BuildFakeData(const fs::path & basedir) {
	auto start = Time::Now();
	d_basedir = basedir;
	GenerateFakedata();
	WriteFakedata();
	std::cerr << "Generated data in " << Time::Now().Sub(start) << std::endl;
}


void Fakedata::GenerateFakedata() {
	GeneratedData gen(d_config);
	SaveFullExpectedResult(gen);
	GenerateTruncatedResults();
	GenerateTDDStructure();
}

void Fakedata::SaveFullExpectedResult(const GeneratedData & gen) {
	ExpectedResult full;
	full.Start = Time::SinceEver();
	full.End = Time::Forever();
	full.MaximumGap = Duration::Hour;
	full.Trajectories = gen.Trajectories;
	full.Interactions = gen.Interactions;
	d_results.push_back(full);
	d_frames = gen.Frames;
}

void Fakedata::GenerateTruncatedResults() {
}


void Fakedata::GenerateTDDStructure() {
	d_nestTDDs =
		{
		 {
		  .AbsoluteFilePath = (d_basedir / "nest.0000").string(),
		  .HasFullFrame = false,
		  .HasMovie = true,
		  .HasConfig = true,
		  .Start = d_config.Start,
		  .End = d_config.Start.Add(10*Duration::Second),
		 },
		 {
		  .AbsoluteFilePath = (d_basedir / "nest.0001").string(),
		  .HasFullFrame = true,
		  .HasMovie = false,
		  .HasConfig = true,
		  .Start = d_config.Start.Add(10*Duration::Second),
		  .End = d_config.Start.Add(3*Duration::Minute),
		 },
		 {
		  .AbsoluteFilePath = (d_basedir / "nest.0002").string(),
		  .HasFullFrame = true,
		  .HasMovie = false,
		  .HasConfig = true,
		  .Start = d_config.Start.Add(3*Duration::Minute),
		  .End = d_config.End,
		  },
		};

		d_forageTDDs =
		{
		 {
		  .AbsoluteFilePath = (d_basedir / "forage.0000").string(),
		  .HasFullFrame = true,
		  .HasMovie = false,
		  .HasConfig = true,
		  .Start = d_config.Start,
		  .End = d_config.End,
		 },
		};
}

void Fakedata::WriteFakedata() {
	WriteTDDs();
	for ( const auto & e : d_experiments ) {
		WriteExperimentFile(e);
	}
}


void Fakedata::WriteTDDs() {
	for ( const auto & tddInfo : d_nestTDDs ) {
		WriteTDD(tddInfo,1);
	}
	for ( const auto & tddInfo : d_forageTDDs ) {
		WriteTDD(tddInfo,2);
	}
}

void Fakedata::WriteTDD(const TDDInfo & tddInfo,SpaceID spaceID) {
	fs::create_directories(tddInfo.AbsoluteFilePath);
	if ( tddInfo.HasConfig ) {
		WriteTDDConfig(tddInfo);
	}
	auto drawer = std::make_shared<FrameDrawer>(d_config);

	SegmentedDataWriter::List writers
		= {
		   std::make_shared<HermesFileWriter>(tddInfo.AbsoluteFilePath,d_config),
		   std::make_shared<CloseUpWriter>(tddInfo.AbsoluteFilePath,
		                                   tddInfo.HasFullFrame,
		                                   drawer),
	};
	if ( tddInfo.HasMovie ) {
		writers.push_back(std::make_shared<MovieWriter>(tddInfo.AbsoluteFilePath,d_config,drawer));
	}
	WriteSegmentedData(tddInfo,spaceID,writers);

}

void Fakedata::WriteSegmentedData(const TDDInfo & tddInfo,
                                  SpaceID spaceID,
                                  const SegmentedDataWriter::List & writers ) {
	size_t i = 0;
	uint64_t frameID = 0;
	for ( Time current = tddInfo.Start;
	      current.Before(tddInfo.End);
	      current = current.Add(d_config.Segment) ) {
		auto endTime = std::min(current.Add(d_config.Segment),tddInfo.End);
		auto begin = std::find_if(d_frames.begin(),
		                          d_frames.end(),
		                          [&](const std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr> & it ) {
			                          return it.first->FrameTime >= current;
		                          });
		auto end = std::find_if(d_frames.begin(),
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



void Fakedata::WriteTDDConfig(const TDDInfo & info) {
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
	       << "apriltag:" << std::endl
	       << "  family: " << "36h11" <<  std::endl
	       << "  quad:" << std::endl
	       << "    decimate: 1" << std::endl
	       << "    sigma: 0" << std::endl
	       << "    refine-edges: false" << std::endl
	       << "    min-cluster-pixel: 25" << std::endl
	       << "    max-n-maxima: 10" << std::endl
	       << "    critical-angle-radian: 0.17453299" << std::endl
	       << "    max-line-mean-square-error: 10" << std::endl
	       << "    min-black-white-diff: 75" << std::endl
	       << "    deglitch: false" << std::endl
	       << "highlights: []" << std::endl;
}

void Fakedata::WriteExperimentFile(const ExperimentInfo & info) {
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
		throw std::runtime_error("open('" + info.AbsoluteFilePath + "',O_RDONLY | O_BINARY): " + std::to_string(errno));
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
	for ( const auto & tddInfo : d_forageTDDs ) {
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

} // namespace myrmidon
} // namespace fort
