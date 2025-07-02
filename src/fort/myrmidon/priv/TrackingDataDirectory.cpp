#include <algorithm>
#include <memory>
#include <optional>
extern "C" {
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
}

#include "TrackingDataDirectory.hpp"

#include <cpptrace/cpptrace.hpp>
#include <mutex>
#include <regex>

#include <tbb/parallel_for.h>

#include <yaml-cpp/yaml.h>

#include <fort/hermes/Error.hpp>
#include <fort/hermes/FileContext.hpp>

#include <fort/utils/Defer.hpp>

#include <fort/video/PNG.hpp>
#include <fort/video/Reader.hpp>

#include <fort/myrmidon/priv/proto/TDDCache.hpp>
#include <fort/myrmidon/priv/proto/TagCloseUpCache.hpp>
#include <fort/myrmidon/priv/proto/TagStatisticsCache.hpp>
#include <fort/myrmidon/utils/Checker.hpp>
#include <fort/myrmidon/utils/FileSystem.hpp>
#include <fort/myrmidon/utils/NotYetImplemented.hpp>
#include <fort/myrmidon/utils/ObjectPool.hpp>

#include "RawFrame.hpp"
#include "TagCloseUp.hpp"
#include "TimeUtils.hpp"
#include "TrackingDataDirectoryError.hpp"

#ifdef MYRMIDON_USE_BOOST_FILESYSTEM
#define MYRMIDON_FILE_IS_REGULAR(f) ((f).type() == fs::regular_file)
#else
#define MYRMIDON_FILE_IS_REGULAR(f) ((f).type() == fs::file_type::regular)
#endif

namespace fort {
namespace myrmidon {
namespace priv {

TrackingDataDirectory::~TrackingDataDirectory() {}

TrackingDataDirectory::Ptr TrackingDataDirectory::Create(
    const std::string                 &uri,
    const fs::path                    &absoluteFilePath,
    uint64_t                           startFrame,
    uint64_t                           endFrame,
    const Time                        &startdate,
    const Time                        &enddate,
    const TrackingIndex::Ptr          &si,
    const MovieIndex::Ptr             &movies,
    const FrameReferenceCacheConstPtr &referenceCache
) {

	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);

	std::shared_ptr<TrackingDataDirectory> res(new TrackingDataDirectory(
	    uri,
	    absoluteFilePath,
	    startFrame,
	    endFrame,
	    startdate,
	    enddate,
	    si,
	    movies,
	    referenceCache
	));
	return res;
}

TrackingDataDirectory::TrackingDataDirectory(
    const std::string                 &uri,
    const fs::path                    &absoluteFilePath,
    uint64_t                           startFrame,
    uint64_t                           endFrame,
    const Time                        &startdate,
    const Time                        &enddate,
    const TrackingIndex::Ptr          &si,
    const MovieIndex::Ptr             &movies,
    const FrameReferenceCacheConstPtr &referenceCache
)
    : d_absoluteFilePath(absoluteFilePath)
    , d_URI(uri)
    , d_startFrame(startFrame)
    , d_endFrame(endFrame)
    , d_uid(GetUID(d_absoluteFilePath))
    , d_segments(si)
    , d_movies(movies)
    , d_referencesByFID(referenceCache) {

	d_start = startdate;
	d_end   = enddate;

	if (d_startFrame >= d_endFrame) {
		std::ostringstream os;
		os << "TrackingDataDirectory: startFrame:" << d_startFrame
		   << " >= endDate: " << d_endFrame;
		throw std::invalid_argument(os.str());
	}

	if (startdate.Before(enddate) == false) {
		std::ostringstream os;
		os << "TrackingDataDirectory: startDate:" << startdate
		   << " >= endDate: " << enddate;
		throw std::invalid_argument(os.str());
	}

	for (const auto &[frameID, ref] : *referenceCache) {
		d_frameIDByTime.insert(std::make_pair(ref.Time(), frameID));
	}
}

const std::string &TrackingDataDirectory::URI() const {
	return d_URI;
}

const fs::path &TrackingDataDirectory::AbsoluteFilePath() const {
	return d_absoluteFilePath;
}

uint64_t TrackingDataDirectory::StartFrame() const {
	return d_startFrame;
}

uint64_t TrackingDataDirectory::EndFrame() const {
	return d_endFrame;
}

const Time &TrackingDataDirectory::Start() const {
	return d_start;
}

const Time &TrackingDataDirectory::End() const {
	return d_end;
}

TrackingDataDirectory::UID
TrackingDataDirectory::GetUID(const fs::path &filepath) {
	static std::mutex              mutex;
	static UID                     last = 0;
	static std::map<fs::path, UID> d_UIDs;
	std::lock_guard<std::mutex>    lock(mutex);
	fs::path fpath = fs::weakly_canonical(fs::absolute(filepath));
	auto     fi    = d_UIDs.find(fpath);
	if (fi == d_UIDs.end()) {
		d_UIDs.insert(std::make_pair(fpath, ++last));
		return last;
	}
	return fi->second;
}

const tags::ApriltagOptions &TrackingDataDirectory::DetectionSettings() const {
	return d_detectionSettings;
}

void TrackingDataDirectory::CheckPaths(
    const fs::path &path, const fs::path &experimentRoot
) {
	if (fs::is_directory(experimentRoot) == false) {
		throw std::invalid_argument(
		    "experiment root path " + experimentRoot.string() +
		    " is not a directory"
		);
	}
	if (fs::is_directory(path) == false) {
		throw std::invalid_argument(path.string() + " is not a directory");
	}
}

std::tuple<
    std::vector<fs::path>,
    std::map<uint32_t, std::pair<fs::path, fs::path>>>
TrackingDataDirectory::LookUpFiles(const fs::path &absoluteFilePath) {
	std::vector<fs::path>                             hermesFiles;
	std::map<uint32_t, std::pair<fs::path, fs::path>> moviesPaths;
	auto extractID = [](const fs::path &p) -> uint32_t {
		std::istringstream iss(p.stem().extension().string());
		uint32_t           res;
		iss.ignore(std::numeric_limits<std::streamsize>::max(), '.');
		iss >> res;
		if (!iss) {
			throw std::runtime_error("Could not extract id in " + p.string());
		}
		return res;
	};
	for (auto const &f : fs::directory_iterator(absoluteFilePath)) {
		if (!MYRMIDON_FILE_IS_REGULAR(f.status())) {
			continue;
		}
		auto p = f.path();
		if (p.extension() == ".hermes" &&
		    p.filename().string().substr(0, 9) == "tracking.") {
			hermesFiles.push_back(p);
			continue;
		}

		if (p.extension() == ".mp4" && p.stem().stem() == "stream") {
			moviesPaths[extractID(p)].first = p;
		}

		if (p.extension() == ".txt" &&
		    p.stem().stem() == "stream.frame-matching") {
			moviesPaths[extractID(p)].second = p;
		}
	}

	std::sort(hermesFiles.begin(), hermesFiles.end());
	return {hermesFiles, moviesPaths};
}

MovieSegment::List TrackingDataDirectory::LoadMovieSegments(
    const std::map<uint32_t, std::pair<fs::path, fs::path>> &moviesPaths,
    const std::string                                       &parentURI
) {
	MovieSegment::List movies;
	for (const auto &[id, paths] : moviesPaths) {
		if (!paths.first.empty() && !paths.second.empty()) {
			movies.push_back(
			    MovieSegment::Open(id, paths.first, paths.second, parentURI)
			);
		}
	}

	std::sort(
	    movies.begin(),
	    movies.end(),
	    [](const MovieSegment::Ptr &a, const MovieSegment::Ptr &b) {
		    return a->StartFrame() < b->StartFrame();
	    }
	);
	return movies;
}

void TrackingDataDirectory::BuildFrameReferenceCache(
    const std::string             &URI,
    Time::MonoclockID              monoID,
    const fs::path                &tddPath,
    const TrackingIndex::ConstPtr &trackingIndexer,
    FrameReferenceCache           &cache,
    const ProgressCallback        &progress,
    FixableErrorList              &errors
) {
	struct CacheSegment {
		std::string             AbsoluteFilePath;
		std::map<FrameID, Time> ToFind;
		FixableErrorList        Errors;

		void Load(Time::MonoclockID monoID) {

			fort::hermes::FileContext  fc(AbsoluteFilePath, false);
			fort::hermes::FrameReadout ro;
			bool                       first      = true;
			FrameID                    curFrameID = 0;
			for (auto iter = ToFind.begin(); iter != ToFind.end();) {
				try {
					fc.Read(&ro);
					curFrameID   = ro.frameid();
					Time curTime = TimeFromFrameReadout(ro, monoID);
					if (iter->first == curFrameID) {
						iter->second = curTime;
						++iter;
					}
				} catch (const fort::hermes::EndOfFile &) {
					if (iter != ToFind.end()) {
						throw std::runtime_error(
						    "Frame " + std::to_string(iter->first) +
						    " is outside of file " + AbsoluteFilePath
						);
					}
				} catch (const hermes::UnexpectedEndOfFileSequence &e) {
					auto error = std::make_unique<CorruptedHermesFileError>(
					    "Could not find frame " + std::to_string(iter->first) +
					        " in " + AbsoluteFilePath,
					    this->AbsoluteFilePath,
					    curFrameID
					);
					Errors.push_back(std::move(error));
					return;
				} catch (const std::exception &e) {
					throw std::runtime_error(
					    "[TDD.BuildCache]: Could not find frame " +
					    std::to_string(iter->first) + ": " + e.what()
					);
				}
			}
		}
	};

	std::map<std::string, CacheSegment> toFind;
	std::vector<CacheSegment>           flattened;
	for (const auto &[frameID, neededRef] : cache) {
		const auto &[ref, file] = trackingIndexer->Find(frameID);
		toFind[file].ToFind.insert({frameID, Time()});
		toFind[file].ToFind.insert({ref.FrameID(), Time()});
	}
	flattened.reserve(toFind.size());
	for (auto &[file, segment] : toFind) {
		segment.AbsoluteFilePath = (tddPath / file).string();
		flattened.push_back(std::move(segment));
	}

	std::atomic<int> counts;
	counts.store(0);
	// do the parrallel computations
	progress(0, flattened.size());
	tbb::parallel_for(
	    tbb::blocked_range<size_t>(0, flattened.size()),
	    [&flattened, monoID, &counts, &progress](
	        const tbb::blocked_range<size_t> &range
	    ) {
		    for (size_t idx = range.begin(); idx != range.end(); ++idx) {
			    flattened[idx].Load(monoID);
			    progress(counts.fetch_add(1) + 1, flattened.size());
		    }
	    }
	);
	// merge all
	for (auto &segment : flattened) {
		for (const auto &[frameID, time] : segment.ToFind) {
			cache[frameID] = FrameReference(URI, frameID, time);
		}
		for (auto &e : segment.Errors) {
			errors.push_back(std::move(e));
		}
	}
}

std::tuple<
    TrackingDataDirectory::TimedFrame,
    TrackingDataDirectory::TimedFrame,
    FixableError::Ptr>
TrackingDataDirectory::BuildIndexes(
    const std::string           &URI,
    Time::MonoclockID            monoID,
    const std::vector<fs::path> &hermesFiles,
    const TrackingIndex::Ptr    &trackingIndexer
) {

	uint64_t start, end;
	Time     startDate, endDate;

	fort::hermes::FrameReadout                 ro;
	bool                                       first = true;
	std::shared_ptr<fort::hermes::FileContext> fc;

	FixableError::Ptr error;
	std::string       last = "";
	for (const auto &f : hermesFiles) {
		try {
			// we only read a single file
			fc = std::make_shared<fort::hermes::FileContext>(f.string(), false);
			fc->Read(&ro);
			Time startTime = TimeFromFrameReadout(ro, monoID);

			if (first == true) {
				start     = ro.frameid();
				startDate = startTime;
				first     = false;
			}

			FrameID        curFrameID = ro.frameid();
			FrameReference curReference(URI, curFrameID, startTime);
			trackingIndexer->Insert(
			    curReference,
			    f.filename().generic_string()
			);
			last = f.string();
		} catch (const std::exception &e) {
			if (last.empty()) {
				throw std::runtime_error(
				    "Could not read first frame from " + f.string() + ": " +
				    e.what()
				);
			} else {
				error = std::make_unique<CorruptedHermesFileError>(
				    "could not read first frame from '" + f.string() +
				        "': " + e.what(),
				    last,
				    std::numeric_limits<uint64_t>::max()
				);
				// very important, we only read a single file, we do
				// not try to read the next one: it will most likely
				// fail !
				fc = std::make_shared<fort::hermes::FileContext>(last, false);
				break;
			}
		}
	}

	try {
		for (;;) {
			fc->Read(&ro);
			end     = ro.frameid();
			endDate = TimeFromFrameReadout(ro, monoID);

			// we add 1 nanosecond to transform the valid range from
			//[start;end[ to [start;end] by making it
			//[start;end+1ns[. There are no time existing between end
			// and end+1ns;
			endDate = endDate.Add(1);
		}
	} catch (const fort::hermes::EndOfFile &) {
		// DO nothing, we just reached EOF
	} catch (const fort::hermes::UnexpectedEndOfFileSequence &e) {
		error = std::make_unique<CorruptedHermesFileError>(
		    "could not read last frame from '" + last + "': " + e.what(),
		    last,
		    end
		);
	} catch (const std::exception &e) {
		throw std::runtime_error(
		    "could not extract last frame from " + last + ": " + e.what()
		);
	}

	return std::make_tuple(
	    std::make_pair(start, startDate),
	    std::make_pair(end, endDate),
	    std::move(error)
	);
}

    std::multimap<FrameID, std::pair<fs::path, std::shared_ptr<TagID>>>
	TrackingDataDirectory::ListTagCloseUpFiles(const fs::path &path) {
		std::multimap<FrameID, std::pair<fs::path, std::shared_ptr<TagID>>> res;

		static std::regex singleRx("ant_([0-9]+)_(frame_)?([0-9]+).png");
		static std::regex multiRx("frame_([0-9]+).png");

		for (const auto &de : fs::directory_iterator(path)) {
			auto ext = de.path().extension().string();
			std::transform(
			    ext.begin(),
			    ext.end(),
			    ext.begin(),
			    [](unsigned char c) { return std::tolower(c); }
			);
			if (ext != ".png") {
				continue;
			}

			std::smatch ID;
			std::string filename = de.path().filename().string();
			FrameID     frameID;
			if (std::regex_search(filename, ID, singleRx) && ID.size() > 3) {
				std::istringstream IDS(ID.str(1));
				std::istringstream FrameS(ID.str(3));
				auto               tagID = std::make_shared<TagID>(0);

				IDS >> *(tagID);
				FrameS >> frameID;
				res.insert(
				    std::make_pair(frameID, std::make_pair(de.path(), tagID))
				);
				continue;
			}
			if (std::regex_search(filename, ID, multiRx) && ID.size() > 1) {
				std::istringstream FrameS(ID.str(1));
				FrameS >> frameID;
				res.insert(std::make_pair(
				    frameID,
				    std::make_pair(de.path(), std::shared_ptr<TagID>())
				));
				continue;
			}
		}

		return res;
	}

	std::tuple<TrackingDataDirectory::Ptr, FixableErrorList>
	TrackingDataDirectory::Open(
	    const fs::path         &filepath,
	    const fs::path         &experimentRoot,
	    const ProgressCallback &progress
	) {
		CheckPaths(filepath, experimentRoot);

		auto absoluteFilePath = fs::weakly_canonical(fs::absolute(filepath));
		auto URI = fs::relative(absoluteFilePath, fs::absolute(experimentRoot));

		Ptr              res;
		FixableErrorList errors;
		try {
			res = LoadFromCache(absoluteFilePath, URI.generic_string());
		} catch (const std::exception &e) {
			std::tie(res, errors) =
			    OpenFromFiles(absoluteFilePath, URI.generic_string(), progress);
			if (errors.empty()) {
				try {
					res->SaveToCache();
				} catch (const std::exception &e) {
				}
			}
		}

		res->LoadComputedFromCache();
		res->LoadDetectionSettings();

		return std::make_tuple(res, std::move(errors));
	}

	std::tuple<TrackingDataDirectory::Ptr, FixableErrorList>
	TrackingDataDirectory::OpenFromFiles(
	    const fs::path         &absoluteFilePath,
	    const std::string      &URI,
	    const ProgressCallback &progress
	) {

		auto ti             = std::make_shared<TrackingIndex>();
		auto mi             = std::make_shared<MovieIndex>();
		auto referenceCache = std::make_shared<FrameReferenceCache>();
		FixableErrorList errors;

		auto [hermesFiles, moviesPaths] = LookUpFiles(absoluteFilePath);
		if (hermesFiles.empty()) {
			throw std::invalid_argument(
			    absoluteFilePath.string() +
			    " does not contains any .hermes file"
			);
		}

		Time::MonoclockID monoID = GetUID(absoluteFilePath);

		auto [start, end, error] = BuildIndexes(URI, monoID, hermesFiles, ti);
		auto [startFrame, startDate] = start;
		auto [endFrame, endDate]     = end;

		if (error != nullptr) {
			errors.push_back(std::move(error));
		}

		auto closeUpFiles = ListTagCloseUpFiles(absoluteFilePath / "ants");

		for (const auto &[frameID, s] : closeUpFiles) {
			auto [filepath, filter] = s;
			if (frameID > endFrame) {
				errors.push_back(std::make_unique<NoKnownAcquisitionTimeFor>(
				    "could not access acquisition time for '" +
				        filepath.string() +
				        "': last readable TrackingFrameID is " +
				        std::to_string(endFrame),
				    filepath
				));
			} else {
				referenceCache->insert(
				    std::make_pair(frameID, FrameReference(URI, 0, Time()))
				);
			}
		}

		auto movies = LoadMovieSegments(moviesPaths, URI);
		movies.erase(
		    std::remove_if(
		        movies.begin(),
		        movies.end(),
		        [endFrame](const MovieSegment::Ptr &ms) {
			        return ms->StartFrame() > endFrame;
		        }
		    ),
		    movies.end()
		);
		for (const auto &m : movies) {
			referenceCache->insert(
			    std::make_pair(m->StartFrame(), FrameReference(URI, 0, Time()))
			);
			if (m->EndFrame() <= endFrame) {
				referenceCache->insert(std::make_pair(
				    m->EndFrame(),
				    FrameReference(URI, 0, Time())
				));
			}
		}

		BuildFrameReferenceCache(
		    URI,
		    monoID,
		    absoluteFilePath,
		    ti,
		    *referenceCache,
		    progress,
		    errors
		);
		// caches the last frame
		referenceCache->insert(std::make_pair(
		    endFrame,
		    FrameReference(URI, endFrame, endDate.Add(-1))
		));
		Time emptyTime;

		std::set<FrameID> toErase;
		for (const auto &[frameID, ref] : *referenceCache) {
			if (ref.FrameID() == 0 || ref.Time().Equals(emptyTime)) {
				toErase.insert(frameID);
			}
		}

		for (const auto &m : movies) {
			auto fi = referenceCache->find(m->StartFrame());
			if (fi == referenceCache->cend() ||
			    (fi->second.FrameID() == 0 ||
			     fi->second.Time().Equals(emptyTime))) {

				std::ostringstream oss;
				oss << "could not access acquisition time for frame "
				    << m->StartFrame() << ", starting frame of movie segment '"
				    << m->AbsoluteFilePath()
				    << "', likely due to data corruption.";
				errors.push_back(std::make_unique<NoKnownAcquisitionTimeFor>(
				    oss.str(),
				    m->AbsoluteFilePath()
				));
			} else {
				mi->Insert(fi->second, m);
			}
		}

		for (const auto &[frameID, s] : closeUpFiles) {
			if (toErase.count(frameID) == 0) {
				continue;
			}
			auto [filepath, filter] = s;
			errors.push_back(std::make_unique<NoKnownAcquisitionTimeFor>(
			    "could not access acquisition time for '" + filepath.string() +
			        "', likely due to data corruption",
			    filepath
			));
		}

		for (auto frameID : toErase) {
			referenceCache->erase(frameID);
		}

		return {
		    TrackingDataDirectory::Create(
		        URI,
		        absoluteFilePath,
		        startFrame,
		        endFrame,
		        startDate,
		        endDate,
		        ti,
		        mi,
		        referenceCache
		    ),
		    std::move(errors)};
	}

	const TrackingDataDirectory::TrackingIndex &
	TrackingDataDirectory::TrackingSegments() const {
		return *d_segments;
	}

	TrackingDataDirectory::const_iterator::const_iterator(
	    const TrackingDataDirectory::Ptr &parent,
	    uint64_t                          current
	)
	    : d_parent(parent)
	    , d_current(current) {}

	TrackingDataDirectory::const_iterator &
	TrackingDataDirectory::const_iterator::operator=(const const_iterator &other
	) {
		d_parent  = other.d_parent;
		d_current = other.d_current;
		d_file.reset();
		d_frame.reset();
		return *this;
	}

	TrackingDataDirectory::const_iterator::const_iterator(
	    const const_iterator &other
	)
	    : d_parent{other.d_parent}
	    , d_current{other.d_current} {}

	TrackingDataDirectory::const_iterator::const_iterator(
	    const_iterator && other
	)
	    : d_parent(other.d_parent)
	    , d_current(other.d_current)
	    , d_file(std::move(other.d_file))
	    , d_message(other.d_message)
	    , d_frame(other.d_frame) {}

	TrackingDataDirectory::const_iterator &
	TrackingDataDirectory::const_iterator::operator=(const_iterator &&other) {
		d_parent  = other.d_parent;
		d_current = other.d_current;
		d_file    = std::move(other.d_file);
		d_message = other.d_message;
		d_frame   = (other.d_frame);
		return *this;
	}

	TrackingDataDirectory::const_iterator &
	TrackingDataDirectory::const_iterator::operator++() {
		auto parent = LockParent();
		if (d_current <= parent->d_endFrame) {
			++d_current;
		}
		return *this;
	}

	bool TrackingDataDirectory::const_iterator::operator==(
	    const const_iterator &other
	) const {
		auto parent      = LockParent();
		auto otherParent = other.LockParent();
		return (parent->GetUID() == otherParent->GetUID()) &&
		       (d_current == other.d_current);
	}

	bool TrackingDataDirectory::const_iterator::operator!=(
	    const const_iterator &other
	) const {
		return !(*this == other);
	}

	FrameID TrackingDataDirectory::const_iterator::Index() const {
		return d_current;
	}

	const RawFrameConstPtr TrackingDataDirectory::const_iterator::NULLPTR;

	const RawFrameConstPtr &TrackingDataDirectory::const_iterator::operator*() {
		auto parent = LockParent();
		if (d_current > parent->d_endFrame) {
			return NULLPTR;
		}

		while (!d_frame || d_frame->Frame().FrameID() < d_current) {
			if (!d_file) {
				auto p = parent->d_absoluteFilePath /
				         parent->d_segments->Find(d_current).second;
				d_file = std::unique_ptr<fort::hermes::FileContext>(
				    new fort::hermes::FileContext(p.string())
				);
				d_message.Clear();
			}

			try {
				d_file->Read(&d_message);
				d_frame =
				    RawFrame::Create(parent->d_URI, d_message, parent->d_uid);
			} catch (const fort::hermes::UnexpectedEndOfFileSequence &e) {
				auto lastValidID = d_current;
				auto lastValidTime =
				    d_frame ? d_frame->Frame().Time() : parent->Start();

				d_current = parent->d_endFrame + 1;
				d_frame.reset();

				std::optional<FrameID> next;

				if (e.FileLineContext().Next.has_value()) {
					const auto &segments =
					    parent->TrackingSegments().Segments();

					auto iter = std::find_if(
					    segments.begin(),
					    segments.end(),
					    [next =
					         e.FileLineContext().Next.value().filename().string(
					         )](const auto &s) { return s.second == next; }
					);
					if (iter != segments.end()) {
						next = iter->first.FrameID();
					}
				}

				throw CorruptedHermesFileIterator{
				    e.FileLineContext().Filename,
				    lastValidID,
				    lastValidTime,
				    next,
				    parent,
				};
			} catch (const fort::hermes::EndOfFile &) {
				d_current = parent->d_endFrame + 1;
				d_frame.reset();
				return NULLPTR;
			}
		}

		if (d_frame->Frame().FrameID() > d_current) {
			d_current = d_frame->Frame().FrameID();
		}
		return d_frame;
	}

	TrackingDataDirectory::Ptr
	TrackingDataDirectory::const_iterator::LockParent() const {
		if (auto locked = d_parent.lock()) {
			return locked;
		}
		throw DeletedReference<TrackingDataDirectory>();
	}

	TrackingDataDirectory::const_iterator TrackingDataDirectory::begin() const {
		return const_iterator(
		    std::const_pointer_cast<TrackingDataDirectory>(shared_from_this()),
		    d_startFrame
		);
	}

	TrackingDataDirectory::const_iterator TrackingDataDirectory::end() const {
		return const_iterator(
		    std::const_pointer_cast<TrackingDataDirectory>(shared_from_this()),
		    d_endFrame + 1
		);
	}

	TrackingDataDirectory::const_iterator TrackingDataDirectory::FrameAt(
	    uint64_t frameID
	) const {
		if (frameID < d_startFrame || frameID > d_endFrame) {
			return end();
		}
		return const_iterator(
		    std::const_pointer_cast<TrackingDataDirectory>(shared_from_this()),
		    frameID
		);
	}

	TrackingDataDirectory::const_iterator TrackingDataDirectory::FrameAfter(
	    const Time &t
	) const {
		if (t < Start()) {
			std::ostringstream oss;
			oss << t << " is not in [" << Start() << ",+∞[";
			throw std::out_of_range(oss.str());
		}
		auto iter    = FrameAt(d_segments->Find(t).first.FrameID());
		Time curTime = (*iter)->Frame().Time();
		if (curTime == t) {
			return iter;
		}
		for (; iter != end(); ++iter) {
			curTime = (*iter)->Frame().Time();
			if (curTime >= t) {
				return iter;
			}
		}
		return end();
	}

	FrameReference TrackingDataDirectory::FrameReferenceAt(FrameID frameID)
	    const {
		auto fi = d_referencesByFID->find(frameID);
		if (fi != d_referencesByFID->cend()) {
			return fi->second;
		}
		auto it = FrameAt(frameID);
		if (it == end()) {
			throw std::out_of_range(
			    "Could not find frame " + std::to_string(frameID) + " in [" +
			    std::to_string(d_startFrame) + ";" +
			    std::to_string(d_endFrame) + "]"
			);
		}

		return (*it)->Frame();
	}

	FrameReference TrackingDataDirectory::FrameReferenceAfter(const Time &t)
	    const {
		auto fi = d_frameIDByTime.find(t);
		if (fi != d_frameIDByTime.cend()) {
			return FrameReferenceAt(fi->second);
		}
		auto it = FrameAfter(t);
		if (it == end()) {
			throw std::out_of_range(
			    "Could not find frame after " + t.Format() + " in [" +
			    d_start.Format() + ";" + d_end.Format() + "["
			);
		}
		return (*it)->Frame();
	}

	const TrackingDataDirectory::MovieIndex &
	TrackingDataDirectory::MovieSegments() const {
		return *d_movies;
	}

	const TrackingDataDirectory::FrameReferenceCache &
	TrackingDataDirectory::ReferenceCache() const {
		return *d_referencesByFID;
	}

	TrackingDataDirectory::Ptr TrackingDataDirectory::LoadFromCache(
	    const fs::path    &absoluteFilePath,
	    const std::string &URI
	) {
		return proto::TDDCache::Load(absoluteFilePath, URI);
	}

	void TrackingDataDirectory::SaveToCache() const {
		proto::TDDCache::Save(
		    std::const_pointer_cast<TrackingDataDirectory>(shared_from_this())
		);
	}

	std::shared_ptr<std::map<FrameReference, fs::path>>
	TrackingDataDirectory::EnumerateFullFrames(const fs::path &subpath)
	    const noexcept {
		auto dirpath = AbsoluteFilePath() / subpath;
		if (fs::is_directory(dirpath) == false) {
			return {};
		}

		try {
			auto listing = ListTagCloseUpFiles(dirpath);
			auto res = std::make_shared<std::map<FrameReference, fs::path>>();
			for (const auto &[frameID, fileAndFilter] : listing) {
				if (frameID <= d_endFrame && fileAndFilter.second == nullptr) {
					res->insert(std::make_pair(
					    FrameReferenceAt(frameID),
					    fileAndFilter.first
					));
				}
			}
			return res;
		} catch (const std::exception &e) {
		}
		return {};
	}

	TrackingDataDirectory::ComputedRessourceUnavailable::
	    ComputedRessourceUnavailable(const std::string &typeName) noexcept
	    : std::runtime_error(
	          "Computed ressource " + typeName + " is not available"
	      ) {}

	TrackingDataDirectory::ComputedRessourceUnavailable::
	    ~ComputedRessourceUnavailable() noexcept {}

	const std::vector<TagCloseUp::ConstPtr> &TrackingDataDirectory::TagCloseUps(
	) const {
		if (TagCloseUpsComputed() == false) {
			throw ComputedRessourceUnavailable("TagCloseUp");
		}
		return *d_tagCloseUps;
	}

	const std::map<FrameReference, fs::path> &TrackingDataDirectory::FullFrames(
	) const {
		if (FullFramesComputed() == false) {
			throw ComputedRessourceUnavailable("FullFrame");
		}
		return *d_fullFrames;
	}

	const TagStatisticsHelper::Timed &TrackingDataDirectory::TagStatistics()
	    const {
		if (TagStatisticsComputed() == false) {
			throw ComputedRessourceUnavailable("TagStatistics");
		}
		return *d_tagStatistics;
	}

	bool TrackingDataDirectory::TagCloseUpsComputed() const {
		return !d_tagCloseUps == false;
	}

	bool TrackingDataDirectory::TagStatisticsComputed() const {
		return !d_tagStatistics == false;
	}

	bool TrackingDataDirectory::FullFramesComputed() const {
		return !d_fullFrames == false;
	}

	class TagCloseUpsReducer {
	public:
		TagCloseUpsReducer(size_t count, const TrackingDataDirectory::Ptr &tdd)
		    : d_tdd(tdd)
		    , d_closeUps(count) {
			d_count.store(count);
		}

		FixableError::Ptr Compute(
		    size_t                                                index,
		    FrameID                                               frameID,
		    const TrackingDataDirectory::TagCloseUpFileAndFilter &fileAndFilter
		) {
			auto detector = d_detectorPool.Get(d_tdd->DetectionSettings());

			try {
				auto [tcus, error] = detector->Detect(
				    fileAndFilter,
				    d_tdd->FrameReferenceAt(frameID)
				);
				Reduce(index, tcus);
				return std::move(error);
			} catch (const std::exception &) {
				Reduce(index, {});
				throw;
			}
			return nullptr;
		}

		void
		Reduce(size_t index, const std::vector<TagCloseUp::ConstPtr> &tcus) {
			d_closeUps[index] = tcus;
			if ((d_count.fetch_sub(1) - 1) > 0) {
				return;
			}
			d_tdd->d_tagCloseUps =
			    std::make_shared<std::vector<TagCloseUp::ConstPtr>>();
			for (const auto tcus : d_closeUps) {
				d_tdd->d_tagCloseUps->insert(
				    d_tdd->d_tagCloseUps->end(),
				    tcus.begin(),
				    tcus.end()
				);
			}
			proto::TagCloseUpCache::Save(
			    d_tdd->AbsoluteFilePath(),
			    *d_tdd->d_tagCloseUps
			);
		}

	private:
		class Detector {
		public:
			Detector(const tags::ApriltagOptions &detectorOptions) {
				const auto &[constructor, destructor] =
				    tags::GetFamily(detectorOptions.Family);
				d_family     = constructor();
				d_destructor = destructor;
				d_detector   = apriltag_detector_create();

				detectorOptions.SetUpDetector(d_detector);
				apriltag_detector_add_family(d_detector, d_family);
			}

			~Detector() {
				apriltag_detector_destroy(d_detector);
				d_destructor(d_family);
			}

			static image_u8_t AsImageU8(const video::Frame &frame) {

				if (frame.Format != AV_PIX_FMT_GRAY8) {
					throw cpptrace::invalid_argument{
					    std::string{"invalid image format "} +
					    av_get_pix_fmt_name(frame.Format)};
				}
				return image_u8_t{
				    .width  = frame.Size.Width,
				    .height = frame.Size.Height,
				    .stride = frame.Linesize[0],
				    .buf    = frame.Planes[0],
				};
			}

			std::tuple<std::vector<TagCloseUp::ConstPtr>, FixableError::Ptr>
			Detect(
			    const TrackingDataDirectory::TagCloseUpFileAndFilter
			                         &fileAndFilter,
			    const FrameReference &reference
			) {

				std::vector<TagCloseUp::ConstPtr> res;

				zarray_t *detections = nullptr;
				try {
					auto img   = video::ReadPNG(fileAndFilter.first);
					auto img8  = AsImageU8(*img);
					detections = apriltag_detector_detect(d_detector, &img8);
				} catch (const std::exception &e) {
					std::ostringstream oss;
					oss << "could not read image " << fileAndFilter.first
					    << ": " << e.what();

					return {
					    res,
					    std::make_unique<NoKnownAcquisitionTimeFor>(
					        oss.str(),
					        fileAndFilter.first
					    ),
					};
				}

				defer {
					apriltag_detections_destroy(detections);
				};
				apriltag_detection *d;
				for (size_t i = 0; i < zarray_size(detections); ++i) {
					zarray_get(detections, i, &d);
					if (fileAndFilter.second &&
					    d->id != *fileAndFilter.second) {
						continue;
					}
					res.push_back(std::make_shared<TagCloseUp>(
					    fileAndFilter.first,
					    reference,
					    d
					));
				}

				if (fileAndFilter.second != nullptr && res.empty() == true) {
					std::ostringstream oss;
					oss << "could not detect tag 0x" << std::hex
					    << *fileAndFilter.second << " (decimal: " << std::dec
					    << *fileAndFilter.second << ") in "
					    << fileAndFilter.first;
					return {
					    res,
					    std::make_unique<NoKnownAcquisitionTimeFor>(
					        oss.str(),
					        fileAndFilter.first
					    )};
				}

				return {res, nullptr};
			}

		private:
			apriltag_family_t     *d_family;
			tags::FamilyDestructor d_destructor;
			apriltag_detector_t   *d_detector;
		};

		std::atomic<size_t>                            d_count;
		TrackingDataDirectory::Ptr                     d_tdd;
		std::vector<std::vector<TagCloseUp::ConstPtr>> d_closeUps;
		utils::ObjectPool<Detector>                    d_detectorPool;
	};

	std::vector<TrackingDataDirectory::Loader>
	TrackingDataDirectory::PrepareTagCloseUpsLoaders() {
		auto tagCloseUpFiles = ListTagCloseUpFiles(AbsoluteFilePath() / "ants");

		// we discard all close-up which are out-of-range
		tagCloseUpFiles.erase(
		    tagCloseUpFiles.upper_bound(d_endFrame),
		    tagCloseUpFiles.end()
		);

		if (tagCloseUpFiles.empty() ||
		    d_detectionSettings.Family == tags::Family::Undefined) {
			d_tagCloseUps =
			    std::make_shared<std::vector<TagCloseUp::ConstPtr>>();
			proto::TagCloseUpCache::Save(AbsoluteFilePath(), {});
			return {};
		}

		auto reducer = std::make_shared<TagCloseUpsReducer>(
		    tagCloseUpFiles.size(),
		    shared_from_this()
		);
		size_t              i = 0;
		std::vector<Loader> res;
		res.reserve(tagCloseUpFiles.size());
		for (const auto &[frameID, fileAndFilter] : tagCloseUpFiles) {
			res.push_back([frameID, fileAndFilter, reducer, i]() {
				return reducer->Compute(i, frameID, fileAndFilter);
			});
			++i;
		}

		return res;
	}

	class TagStatisticsReducer {
	public:
		TagStatisticsReducer(
		    size_t count, const TrackingDataDirectory::Ptr &tdd
		)
		    : d_tdd(tdd)
		    , d_stats(count) {
			d_count.store(count);
		}

		void Reduce(size_t index, const TagStatisticsHelper::Timed &stats) {
			d_stats[index] = stats;
			if ((d_count.fetch_sub(1) - 1) > 0) {
				return;
			}
			d_tdd->d_tagStatistics =
			    std::make_shared<TagStatisticsHelper::Timed>(
			        TagStatisticsHelper::MergeTimed(
			            d_stats.begin(),
			            d_stats.end()
			        )
			    );
			proto::TagStatisticsCache::Save(
			    d_tdd->AbsoluteFilePath(),
			    *d_tdd->d_tagStatistics
			);
		}

	private:
		std::atomic<size_t>                     d_count;
		TrackingDataDirectory::Ptr              d_tdd;
		std::vector<TagStatisticsHelper::Timed> d_stats;
	};

	std::vector<TrackingDataDirectory::Loader>
	TrackingDataDirectory::PrepareTagStatisticsLoaders() {
		const auto &segments = d_segments->Segments();
		auto        reducer  = std::make_shared<TagStatisticsReducer>(
            segments.size(),
            shared_from_this()
        );

		std::vector<Loader> res;
		res.reserve(segments.size());
		size_t i = 0;
		for (const auto &s : segments) {

			res.push_back([reducer, s, i, this]() {
				auto [stats, error] = TagStatisticsHelper::BuildStats(
				    (AbsoluteFilePath() / s.second).string()
				);
				reducer->Reduce(i, stats);
				return std::move(error);
			});
			++i;
		}
		return res;
	}

	class FullFramesReducer {
	public:
		FullFramesReducer(size_t count, const TrackingDataDirectory::Ptr &tdd)
		    : d_tdd(tdd) {
			d_count.store(count);
		}

		void Reduce() {
			if ((d_count.fetch_sub(1) - 1) > 0) {
				return;
			}
			d_tdd->d_fullFrames = d_tdd->EnumerateFullFrames("ants/computed");
		}

	private:
		std::atomic<size_t>        d_count;
		TrackingDataDirectory::Ptr d_tdd;
	};

	std::vector<TrackingDataDirectory::Loader>
	TrackingDataDirectory::PrepareFullFramesLoaders() {
		auto firstFrame = *begin();
		int  width      = firstFrame->Width();
		int  height     = firstFrame->Height();
		fs::create_directory(AbsoluteFilePath() / "ants/computed");
		auto reducer = std::make_shared<FullFramesReducer>(
		    d_movies->Segments().size(),
		    shared_from_this()
		);
		std::vector<Loader> res;

		for (const auto &ms : d_movies->Segments()) {
			res.push_back([reducer, ms, width, height, this]() {
				video::Reader v{
				    ms.second->AbsoluteFilePath(),
				    AV_PIX_FMT_GRAY8,
				    {width, height}};

				auto frame = v.CreateFrame();
				if (v.Read(*frame) == false) {
					return nullptr;
				}

				auto filename =
				    "frame_" + std::to_string(ms.second->ToTrackingFrameID(0)) +
				    ".png";
				auto imgPath = AbsoluteFilePath() / "ants/computed" / filename;
				WritePNG(imgPath, *frame);

				reducer->Reduce();
				return nullptr;
			});
		}

		return res;
	}

	void TrackingDataDirectory::LoadComputedFromCache() {
		try {
			d_tagStatistics = std::make_shared<TagStatisticsHelper::Timed>(
			    proto::TagStatisticsCache::Load(AbsoluteFilePath())
			);
		} catch (const std::exception &e) {
		}

		try {
			d_tagCloseUps =
			    std::make_shared<std::vector<TagCloseUp::ConstPtr>>();
			*d_tagCloseUps = proto::TagCloseUpCache::Load(
			    AbsoluteFilePath(),
			    [this](FrameID frameID) -> FrameReference {
				    return FrameReferenceAt(frameID);
			    }
			);
		} catch (const std::exception &e) {
			d_tagCloseUps.reset();
		}

		d_fullFrames = EnumerateFullFrames("ants");
		if (!d_fullFrames || d_fullFrames->empty()) {
			d_fullFrames = EnumerateFullFrames("ants/computed");
		}
	}

	void TrackingDataDirectory::LoadDetectionSettings() {
		auto path = AbsoluteFilePath() / "leto-final-config.yml";
		if (fs::exists(path) == false) {
			path = AbsoluteFilePath() / "leto-final-config.yaml";
			if (fs::exists(path) == false) {
				throw std::runtime_error(
				    "missing either 'leto-final-config.yaml' or "
				    "'leto-final-config.yml' YAML config file"
				);
			}
		}

		auto letoConfig       = YAML::LoadFile(path.string());
		auto apriltagSettings = letoConfig["apriltag"];
		if (!apriltagSettings) {
			return;
		}
		if (apriltagSettings["family"]) {
			d_detectionSettings.Family =
			    tags::FindFamily(apriltagSettings["family"].as<std::string>());
		}
		auto quadSettings = apriltagSettings["quad"];
		if (!quadSettings) {
			return;
		}
#define SET_IF_EXISTS(cppType, cppName, yamlName)                              \
	do {                                                                       \
		if (quadSettings[yamlName]) {                                          \
			d_detectionSettings.cppName =                                      \
			    quadSettings[yamlName].as<cppType>();                          \
		}                                                                      \
	} while (0)

		SET_IF_EXISTS(float, QuadDecimate, "decimate");
		SET_IF_EXISTS(float, QuadSigma, "sigma");
		SET_IF_EXISTS(bool, RefineEdges, "refine-edges");
		SET_IF_EXISTS(int, QuadMinClusterPixel, "min-cluster-pixel");
		SET_IF_EXISTS(int, QuadMaxNMaxima, "max-n-maxima");
		SET_IF_EXISTS(float, QuadCriticalRadian, "critical-angle-radian");
		SET_IF_EXISTS(float, QuadMaxLineMSE, "max-line-mean-square-error");
		SET_IF_EXISTS(int, QuadMinBWDiff, "min-black-white-diff");
		SET_IF_EXISTS(bool, QuadDeglitch, "deglitch");
#undef SET_IF_EXISTS
	}

	std::pair<
	    TrackingDataDirectory::const_iterator,
	    TrackingDataDirectory::const_iterator>
	TrackingDataDirectory::IteratorRange(const Time &start, const Time &end) {
		if (start.Before(end) == false || start >= End() || end < Start()) {
			return std::make_pair(this->end(), this->end());
		}

		const_iterator ibegin = this->begin();
		const_iterator iend   = this->end();

		if (start.After(Start()) == true) {
			ibegin = FrameAfter(start);
		}
		if (end.Before(End()) == true) {
			iend = FrameAfter(end);
		}
		return std::make_pair(std::move(ibegin), std::move(iend));
	}

	std::vector<std::pair<
	    TrackingDataDirectory::const_iterator,
	    TrackingDataDirectory::const_iterator>>
	TrackingDataDirectory::IteratorRanges(
	    const std::vector<Ptr> &list,
	    const Time             &start,
	    const Time             &end
	) {
		if (start.Before(end) == false) {
			return {};
		}
		std::vector<std::pair<const_iterator, const_iterator>> res;
		res.reserve(list.size());
		for (const auto &tdd : list) {
			auto range = tdd->IteratorRange(start, end);
			if (range.first == range.second) {
				continue;
			}
			res.push_back(std::move(range));
		}
		return res;
	}

	std::ostream &operator<<(
	    std::ostream	                                  &out,
	    const fort::myrmidon::priv::TrackingDataDirectory &a
	) {
		return out << "TDD{URI:'" << a.URI() << "', start:" << a.Start()
		           << ", end:" << a.End() << "}";
	}

} // namespace priv
} // namespace myrmidon
} // namespace fort
