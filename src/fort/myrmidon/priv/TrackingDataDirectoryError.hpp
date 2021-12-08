#pragma once

#include <fort/myrmidon/types/FixableError.hpp>
#include <fort/myrmidon/utils/FileSystem.hpp>


namespace fort {
namespace myrmidon {
namespace priv {

class CorruptedHermesFileError : public FixableError {
public:
	CorruptedHermesFileError(const std::string & reason,
	                         const std::string & file,
	                         uint64_t until);
	virtual ~CorruptedHermesFileError();

	std::string FixDescription() const noexcept override;

	void Fix() override;
private:
	fs::path d_file;
	uint64_t d_until;
};


class NoKnownAcquisitionTimeFor : public FixableError {
public:
	NoKnownAcquisitionTimeFor(const std::string & reason,
	                          const fs::path & filepath);
	virtual ~NoKnownAcquisitionTimeFor();
	std::string FixDescription() const noexcept override;

	void Fix() override;
private:
	fs::path d_filepath,d_disabledPath;
};


}  // priv
}  // myrmidon
}  // fort
