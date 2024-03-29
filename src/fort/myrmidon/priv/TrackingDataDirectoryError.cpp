#include "TrackingDataDirectoryError.hpp"

#include <iomanip>

#include <fort/hermes/Header.pb.h>
#include <fort/hermes/FrameReadout.pb.h>


#include <fort/myrmidon/priv/proto/FileReadWriter.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

CorruptedHermesFileError::CorruptedHermesFileError(const std::string & reason,
                                                   const std::string & file,
                                                   uint64_t until)
	: FixableError(reason)
	, d_file(file)
	, d_until(until) {
	if ( d_file.is_absolute() == false ) {
		throw std::invalid_argument("needed an absolute filepath");
	}
}
CorruptedHermesFileError::~CorruptedHermesFileError() {
}

std::string CorruptedHermesFileError::FixDescription() const noexcept {
	if ( d_until == std::numeric_limits<uint64_t>::max() ) {
		return "rewrite '"
			+ d_file.string()
			+ "' to be the last of the sequence";
	}
	return "rewrite '"
		+ d_file.string()
		+ "' up to frame "
		+ std::to_string(d_until)
		+ " and to continue if possible to next segment";
}

void CorruptedHermesFileError::Fix() {
	typedef proto::FileReadWriter<hermes::Header,hermes::FileLine> RW;
	fort::hermes::Header header;
	std::vector<RW::LineWriter> lineWriters;

	uint64_t last = 0;
	try {
		RW::Read(d_file,
		         [&header] ( const hermes::Header & h ) {
			         header.CheckTypeAndMergeFrom(h);
		         },
		         [&last,&lineWriters,this]( const hermes::FileLine & line) {
			         if ( line.has_readout() == false || line.readout().frameid() > d_until) {
				         return;
			         }
			         hermes::FrameReadout ro;
			         ro.CheckTypeAndMergeFrom(line.readout());
			         lineWriters.push_back([ro](hermes::FileLine & wline) {
				                               wline.mutable_readout()->CheckTypeAndMergeFrom(ro);
			                               });
			         if ( line.readout().has_time() ) {
				         last = line.readout().frameid();
			         }
		         });
	} catch ( std::exception & e ) {
	}

	if ( d_until != std::numeric_limits<uint64_t>::max()
	     && last < d_until ) {
		throw std::runtime_error("could not read '"
		                         + d_file.string()
		                         + "' until expected frame "
		                         + std::to_string(d_until));
	}

	auto number = std::atoi(d_file.stem().extension().string().substr(1).c_str());
	std::ostringstream oss;
	oss << d_file.stem().stem().string() << "." << std::setfill('0') << std::setw(4) << (number+1) << ".hermes";;
	auto next = d_file.parent_path()/oss.str();

	if ( d_until != std::numeric_limits<uint64_t>::max()
	     || fs::exists(next) == false ) {
		lineWriters.push_back([](hermes::FileLine & line) {
			                      // create an empty footer for the last message
			                      line.mutable_footer();
		                      });
	} else {
		lineWriters.push_back([&next](hermes::FileLine & line) {
			                      line.mutable_footer()->set_next(next.string());
		                      });
	}

	auto backupName =  d_file.parent_path() / (d_file.filename().string() + ".bak");
	fs::rename(d_file,backupName);
	RW::Write(d_file,
	          header,
	          lineWriters);
}



NoKnownAcquisitionTimeFor::NoKnownAcquisitionTimeFor(const std::string & reason,
                                                     const fs::path & filepath)
	: FixableError(reason)
	, d_filepath(filepath) {
	if ( d_filepath.is_absolute() == false ) {
		throw std::invalid_argument("needed an absolute filepath");
	}
	d_disabledPath = d_filepath.parent_path() / (d_filepath.stem().string() + ".dis" + d_filepath.extension().string());
}

NoKnownAcquisitionTimeFor::~NoKnownAcquisitionTimeFor() {
}

std::string NoKnownAcquisitionTimeFor::FixDescription() const noexcept {
	return "rename '" + d_filepath.string() + "' to '" + d_disabledPath.string() + "'";
}

void NoKnownAcquisitionTimeFor::Fix() {
	fs::rename(d_filepath,d_disabledPath);
}

}  // priv
}  // myrmidon
}  // fort
