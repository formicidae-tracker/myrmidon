#include "Fakedata.hpp"

#include "GeneratedData.hpp"
#include <fort/hermes/Header.pb.h>

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/io/gzip_stream.h>

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
		  .End = d_config.Start.Add(20*Duration::Second),
		 },
		 {
		  .AbsoluteFilePath = (d_basedir / "nest.0001").string(),
		  .HasFullFrame = true,
		  .HasMovie = false,
		  .HasConfig = true,
		  .Start = d_config.Start.Add(20*Duration::Second),
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
	WriteHermesFiles(tddInfo,spaceID);
}

void Fakedata::ForeachSegment(const TDDInfo & tddInfo,
                              std::function<void(size_t,FrameList::const_iterator,FrameList::const_iterator,bool)> apply) {
	size_t i = 0;
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
		apply(i,begin,end,endTime == tddInfo.End);
		++i;
	}

}

void Fakedata::WriteHermesFiles(const TDDInfo & tddInfo,
                                SpaceID spaceID) {
	uint64_t frameID = 1;
	ForeachSegment(tddInfo,
	               [&](size_t index,
	                   FrameList::const_iterator begin,
	                   FrameList::const_iterator end,
	                   bool last) {
		               WriteHermesFile(index,
		                               tddInfo.AbsoluteFilePath,
		                               spaceID,
		                               frameID,
		                               begin,
		                               end,
		                               last);
	               });

}

std::string HermesFileName(size_t i ) {
	std::ostringstream os;
	os << "tracking." << std::setw(4) << std::setfill('0') << i << ".hermes";
	return os.str();
}


void Fakedata::WriteHermesFile(size_t index,
                               const std::string & basepath,
                               SpaceID spaceID,
                               uint64_t & frameID,
                               FrameList::const_iterator begin,
                               FrameList::const_iterator end,
                               bool last) {

	hermes::Header header;
	auto v = header.mutable_version();
	v->set_vmajor(0);
	v->set_vminor(1);
	header.set_type(hermes::Header::Type::Header_Type_File);
	header.set_width(d_config.Width);
	header.set_height(d_config.Height);
	if ( index > 0 ) {
		header.set_previous(HermesFileName(index-1));
	}
	auto filename = fs::path(basepath) / HermesFileName(index);
	int fd = open( filename.c_str(),
	           O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
	           0644);

	if ( fd <= 0 ) {
		throw std::runtime_error("open('"
		                         + filename.string()
		                         + "',O_RDONLY | O_BINARY)");
	}

	auto file = std::make_unique<google::protobuf::io::FileOutputStream>(fd);
	file->SetCloseOnDelete(true);
	auto gzipped = std::make_unique<google::protobuf::io::GzipOutputStream>(file.get());
	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(header,gzipped.get())) {
		throw std::runtime_error("could not write hermes header");
	}
	fort::hermes::FileLine lineRO,lineFooter;
	for( auto iter = begin; iter != end; ++iter) {
		if ( iter->first->Space != spaceID ) {
			continue;
		}
		auto ro = lineRO.mutable_readout();
		FillReadout(ro,frameID,iter->first);
		++frameID;
		if ( !google::protobuf::util::SerializeDelimitedToZeroCopyStream(lineRO,gzipped.get()) ) {
			throw std::runtime_error("could not write readout");
		}
	}
	auto footer = lineFooter.mutable_footer();
	if ( last == false ) {
		footer->set_next(HermesFileName(index+1));
	}
	if ( !google::protobuf::util::SerializeDelimitedToZeroCopyStream(lineFooter, gzipped.get()) ) {
		throw std::runtime_error("Could not write footer");
	}

}


void Fakedata::FillReadout(hermes::FrameReadout * ro,
                           uint64_t frameID,
                           const IdentifiedFrame::Ptr & identified) {
	ro->Clear();
	ro->set_timestamp(identified->FrameTime.Sub(d_config.Start).Microseconds());
	identified->FrameTime.ToTimestamp(ro->mutable_time());
	ro->set_frameid(frameID);
	ro->set_quads(identified->Positions.rows());
	for ( size_t i = 0; i < identified->Positions.rows(); ++i ) {
		AntID antID = identified->Positions(i,0);
		auto t = ro->add_tags();
		double x,y,angle;

		d_config.Ants.at(antID).ComputeTagPosition(x,y,angle,identified->Positions.block<1,3>(i,1).transpose());
		t->set_x(x);
		t->set_y(y);
		t->set_theta(angle);
		t->set_id(antID-1);
	}
}


} // namespace myrmidon
} // namespace fort
