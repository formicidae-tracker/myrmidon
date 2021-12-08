#include "TrackingDataDirectoryError.hpp"

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
		+ " and to be the last of the sequence";
}

void CorruptedHermesFileError::Fix() {
	//TODO: implement
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
