#pragma once

#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/myrmidon/types/FixableError.hpp>
#include <fort/myrmidon/types/Typedefs.hpp>
#include <fort/myrmidon/utils/FileSystem.hpp>
#include <fort/time/Time.hpp>
#include <optional>

namespace fort {
namespace myrmidon {
namespace priv {

class CorruptedHermesFileError : public FixableError {
public:
	CorruptedHermesFileError(
	    const std::string &reason, const std::string &file, uint64_t until
	);
	virtual ~CorruptedHermesFileError();

	std::string FixDescription() const noexcept override;

	void Fix() override;

private:
	fs::path d_file;
	uint64_t d_until;
};

class CorruptedHermesFileIterator : public std::exception {
public:
	CorruptedHermesFileIterator(
	    const fs::path            &filepath,
	    FrameID                    lastValid,
	    const Time                &lastTime,
	    std::optional<FrameID>     next,
	    TrackingDataDirectory::Ptr tdd
	) noexcept;

	const char *what() const noexcept override;

	TrackingDataDirectory::const_iterator Next() const;

	std::optional<FrameID> NextAvailableID() const;

	Duration Lost() const;

private:
	std::string                d_what;
	fort::Time                 d_lastValid;
	TrackingDataDirectory::Ptr d_tdd;
	std::optional<FrameID>     d_next;
};

class NoKnownAcquisitionTimeFor : public FixableError {
public:
	NoKnownAcquisitionTimeFor(
	    const std::string &reason, const fs::path &filepath
	);
	virtual ~NoKnownAcquisitionTimeFor();
	std::string FixDescription() const noexcept override;

	void Fix() override;

private:
	fs::path d_filepath, d_disabledPath;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
