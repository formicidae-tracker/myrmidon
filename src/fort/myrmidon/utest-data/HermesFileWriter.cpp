#include "HermesFileWriter.hpp"


//#include <sys/types.h>
#include <fcntl.h>
//#include <unistd.h>
//#include <sys/stat.h>

#include <google/protobuf/util/delimited_message_util.h>
#include <google/protobuf/io/gzip_stream.h>


#ifndef O_BINARY
#define O_BINARY 0
#endif


namespace fort {
namespace myrmidon {


HermesFileWriter::HermesFileWriter(const fs::path & basepath,
                                   const Config & config)
	: d_basepath(basepath)
	, d_config(config) {
}

HermesFileWriter::~HermesFileWriter() {}

std::string HermesFileWriter::HermesFileName(size_t i ) {
	std::ostringstream os;
	os << "tracking." << std::setw(4) << std::setfill('0') << i << ".hermes";
	return os.str();
}

void HermesFileWriter::Prepare(size_t index) {
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

	auto filename = fs::path(d_basepath) / HermesFileName(index);
	int fd = open( filename.c_str(),
	               O_CREAT | O_TRUNC | O_RDWR | O_BINARY,
	               0644);

	if ( fd <= 0 ) {
		throw std::runtime_error("open('"
		                         + filename.string()
		                         + "',O_RDONLY | O_BINARY)");
	}

	d_file = std::make_unique<google::protobuf::io::FileOutputStream>(fd);
	d_file->SetCloseOnDelete(true);
	d_gzipped = std::make_unique<google::protobuf::io::GzipOutputStream>(d_file.get());

	if (!google::protobuf::util::SerializeDelimitedToZeroCopyStream(header,d_gzipped.get())) {
		throw std::runtime_error("could not write hermes header");
	}
	d_line.Clear();
}

void HermesFileWriter::WriteFrom(const IdentifiedFrame & data,
                                 uint64_t frameID) {
	auto ro = d_line.mutable_readout();
	FillReadout(ro,frameID,data);
	if ( !google::protobuf::util::SerializeDelimitedToZeroCopyStream(d_line,d_gzipped.get()) ) {
		throw std::runtime_error("could not write readout");
	}
}

void HermesFileWriter::Finalize(size_t index,bool last) {
	d_line.Clear();
	auto footer = d_line.mutable_footer();
	if ( last == false ) {
		footer->set_next(HermesFileName(index+1));
	}
	if ( !google::protobuf::util::SerializeDelimitedToZeroCopyStream(d_line, d_gzipped.get()) ) {
		throw std::runtime_error("Could not write footer");
	}
	d_gzipped.reset();
	d_file.reset();
}

void HermesFileWriter::FillReadout(hermes::FrameReadout * ro,
                                   uint64_t frameID,
                                   const IdentifiedFrame & identified) {
	ro->Clear();
	ro->set_timestamp(identified.FrameTime.MonotonicValue()/1000);
	identified.FrameTime.ToTimestamp(ro->mutable_time());
	ro->set_frameid(frameID);
	ro->set_quads(identified.Positions.rows());
	for ( size_t i = 0; i < identified.Positions.rows(); ++i ) {
		AntID antID = identified.Positions(i,0);
		auto t = ro->add_tags();
		double x,y,angle;

		d_config.Ants.at(antID).ComputeTagPosition(x,y,angle,identified.Positions.block<1,3>(i,1).transpose());
		t->set_x(x);
		t->set_y(y);
		t->set_theta(angle);
		t->set_id(antID-1);
	}
}

} // namespace myrmidon
} // namespace fort
