#include "TagCloseUpCache.hpp"


#include "IOUtils.hpp"


namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

// Version 1: settings in the cache file.
// Version 2: a single detection per close-up
// Version 3: full close up detections
const uint32_t TagCloseUpCache::CACHE_VERSION = 3;

const std::string TagCloseUpCache::CACHE_PATH = "tag-close-up.cache";

std::vector<TagCloseUp::ConstPtr> TagCloseUpCache::Load(
    const fs::path                        &tddAbsoluteFilePath,
    const fs::path                        &closeUpSubdir,
    std::function<FrameReference(FrameID)> resolver
) {
	std::vector<TagCloseUp::ConstPtr> res;
	ReadWriter::Read(
	    tddAbsoluteFilePath / closeUpSubdir / CACHE_PATH,
	    [&res](const pb::TagCloseUpCacheHeader &pb) {
		    if (pb.version() != CACHE_VERSION) {
			    throw cpptrace::runtime_error(
			        "Mismatched cache version " + std::to_string(pb.version()) +
			        " (expected:" + std::to_string(CACHE_VERSION)
			    );
		    }
	    },
	    [&](const pb::TagCloseUp &line) {
		    auto tcu = proto::IOUtils::LoadTagCloseUp(
		        line,
		        tddAbsoluteFilePath / closeUpSubdir,
		        resolver
		    );
		    res.push_back(tcu);
	    }
	);
	return res;
}

void TagCloseUpCache::Save(
    const fs::path                          &tddAbsoluteFilePath,
    const fs::path                          &closeUpSubdir,
    const std::vector<TagCloseUp::ConstPtr> &tagCloseUps
) {
	pb::TagCloseUpCacheHeader h;
	h.set_version(CACHE_VERSION);
	std::vector<ReadWriter::LineWriter> lines;

	for (const auto &tcu : tagCloseUps) {
		lines.push_back([tcu = std::ref(tcu),
		                 &tddAbsoluteFilePath,
		                 &closeUpSubdir](pb::TagCloseUp &line) {
			proto::IOUtils::SaveTagCloseUp(
			    &line,
			    *tcu.get(),
			    tddAbsoluteFilePath / closeUpSubdir
			);
		});
	}

	ReadWriter::Write(
	    tddAbsoluteFilePath / closeUpSubdir / CACHE_PATH,
	    h,
	    lines
	);
}

} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
