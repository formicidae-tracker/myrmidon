#include "TrackingDataDirectory.hpp"

#include <filesystem>
#include <regex>

#include <tbb/concurrent_queue.h>
#include <tbb/parallel_for.h>

#include <fort/hermes/Error.hpp>

#include <fort/myrmidon/utils/Exception.hpp>
#include <fort/myrmidon/utils/Slogpp.hpp>

#include <fort/myrmidon/priv/TimeUtils.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectoryError.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

void TrackingDataDirectory::CheckPaths(
    const fs::path &path, const fs::path &experimentRoot
) {
	if (fs::is_directory(experimentRoot) == false) {
		throw cpptrace::invalid_argument(
		    "experiment root path " + experimentRoot.string() +
		    " is not a directory"
		);
	}
	if (fs::is_directory(path) == false) {
		throw cpptrace::invalid_argument(path.string() + " is not a directory");
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
			throw cpptrace::runtime_error(
			    "Could not extract id in " + p.string()
			);
		}
		return res;
	};
	for (auto const &f : fs::directory_iterator(absoluteFilePath)) {
		if (fs::is_regular_file(f.status()) == false) {
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
    const std::string                                       &parentURI,
    const slog::Logger<1>                                   &logger
) {
	MovieSegment::List movies;
	for (const auto &[id, paths] : moviesPaths) {
		if (!paths.first.empty() && !paths.second.empty()) {
			movies.push_back(
			    MovieSegment::Open(id, paths.first, paths.second, parentURI)
			);
			logger.Trace(
			    "added movie segment",
			    slog::String("video", paths.first),
			    slog::String("frame_matching", paths.second)
			);
		} else {
			logger.Warn(
			    "incomplete movie segment",
			    slog::String("video", paths.first),
			    slog::String("frame_matching", paths.second)
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
    const std::string                       &URI,
    Time::MonoclockID                        monoID,
    const fs::path                          &tddPath,
    const TrackingIndex::ConstPtr           &trackingIndexer,
    FrameReferenceCache                     &cache,
    const std::unique_ptr<ProgressReporter> &progress,
    FixableErrorList                        &errors,
    const slog::Logger<1>                   &logger
) {

	std::atomic<bool> stop = false;
	typedef std::tuple<FrameReference, std::unique_ptr<FixableError>>
	                                           CacheResult;
	tbb::concurrent_bounded_queue<CacheResult> queue;
	size_t                                     total{0};

	struct CacheSegment {
		std::string                                 AbsoluteFilePath;
		const std::string                           URI;
		std::set<FrameID>                           ToFind;
		slog::Logger<2>                             Logger;
		const std::atomic<bool>                    &Stop;
		tbb::concurrent_bounded_queue<CacheResult> &Queue;

		void Load(Time::MonoclockID monoID) {

			fort::hermes::FileContext  fc(AbsoluteFilePath, false);
			fort::hermes::FrameReadout ro;
			FrameID                    curFrameID = 0;
			for (auto iter = ToFind.begin(); iter != ToFind.end();) {
				try {
					if (Stop.load(std::memory_order_acquire) == true) {
						return;
					}

					fc.Read(&ro);
					curFrameID   = ro.frameid();
					Time curTime = TimeFromFrameReadout(ro, monoID);
					if (*iter == curFrameID) {
						Logger.Trace(
						    "found needed reference",
						    slog::Int("ID", curFrameID),
						    slog::FortTime("time", curTime)
						);
						Queue.push(
						    {FrameReference(URI, curFrameID, curTime), nullptr}
						);
					}
					if (*iter <= curFrameID) {
						++iter;
					}
				} catch (const fort::hermes::EndOfFile &) {
					if (iter != ToFind.end()) {
						throw cpptrace::runtime_error(
						    "Frame " + std::to_string(*iter) +
						    " is outside of file " + AbsoluteFilePath
						);
					}
				} catch (hermes::UnexpectedEndOfFileSequence &e) {
					auto error = std::make_unique<CorruptedHermesFileError>(
					    "Could not find frame " + std::to_string(*iter) +
					        " in '" + AbsoluteFilePath + "'",
					    this->AbsoluteFilePath,
					    curFrameID,
					    std::move(e)
					);
					Logger.Error(
					    "missing frame reference",
					    slog::Int("missing_ID", *iter),
					    slog::Int("max_ID", curFrameID)
					);
					Queue.push({FrameReference(), std::move(error)});
					return;
				} catch (const std::exception &e) {
					throw details::WrapLazyException{
					    "[TDD.BuildCache]: Could not find frame " +
					        std::to_string(*iter) + ": " + utils::What(e),
					    details::WrapLazyException::FromException(e)
					};
				}
			}
		}
	};

	std::map<std::string, CacheSegment> toFind;
	std::vector<CacheSegment>           flattened;

	for (const auto &[frameID, neededRef] : cache) {
		const auto &[ref, file] = trackingIndexer->Find(frameID);
		if (toFind.count(file) == 0) {
			toFind.insert(
			    {file,
			     CacheSegment{
			         .AbsoluteFilePath = (tddPath / file).string(),
			         .URI              = URI,
			         .Logger = logger.With(slog::String("file", file)),
			         .Stop   = stop,
			         .Queue  = queue
			     }}
			);
		}
		toFind.at(file).ToFind.insert(frameID);
		toFind.at(file).ToFind.insert(ref.FrameID());
	}
	flattened.reserve(toFind.size());

	for (auto &[file, segment] : toFind) {
		segment.AbsoluteFilePath = (tddPath / file).string();
		total += segment.ToFind.size();
		flattened.push_back(std::move(segment));
	}

	// do the parrallel computations
	if (progress != nullptr) {
		progress->AddTotal(total);
	}

	std::exception_ptr excpt = nullptr;

	std::thread go([&]() {
		try {
			tbb::parallel_for(
			    tbb::blocked_range<size_t>(0, flattened.size()),
			    [&flattened, monoID, &queue, &excpt](
			        const tbb::blocked_range<size_t> &range
			    ) {
				    for (size_t idx = range.begin(); idx != range.end();
				         ++idx) {
					    flattened[idx].Load(monoID);
				    }
			    }
			);
		} catch (...) {
			excpt = std::current_exception();
		}
		queue.push({FrameReference(), nullptr});
	});

	for (;;) {
		CacheResult r;
		queue.pop(r);
		auto [ref, err] = std::move(r);
		if (err == nullptr && ref.Valid() == false) {
			break;
		}
		if (err != nullptr) {
			errors.push_back(std::move(err));
		}
		if (ref.Valid() == true) {
			cache[ref.FrameID()] = ref;
		}
		if (progress != nullptr) {
			try {
				progress->Add(1);
			} catch (const std::exception &e) {
				stop.store(true, std::memory_order_release);
				go.join();
				throw;
			}
		}
	}

	go.join();
	if (excpt != nullptr) {
		std::rethrow_exception(excpt);
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
    const TrackingIndex::Ptr    &trackingIndexer,
    const slog::Logger<1>       &logger
) {
	uint64_t start, end;
	Time     startDate, endDate;

	fort::hermes::FrameReadout                 ro;
	bool                                       first = true;
	std::shared_ptr<fort::hermes::FileContext> fc;

	FixableError::Ptr error;
	std::string       last = "";
	for (const auto &f : hermesFiles) {
		auto sLogger = logger.With(slog::String("segment", f.filename()));
		try {
			// we only read a single file
			fc = std::make_shared<fort::hermes::FileContext>(f.string(), false);
			fc->Read(&ro);
			Time startTime = TimeFromFrameReadout(ro, monoID);

			if (first == true) {
				start     = ro.frameid();
				startDate = startTime;
				first     = false;
				logger.Info(
				    "directory start frame",
				    slog::Int("ID", start),
				    slog::FortTime("time", startTime)
				);
			}

			FrameID        curFrameID = ro.frameid();
			FrameReference curReference(URI, curFrameID, startTime);
			trackingIndexer->Insert(
			    curReference,
			    f.filename().generic_string()
			);
			sLogger.Info(
			    "segment start",
			    slog::Int("ID", curFrameID),
			    slog::FortTime("time", startTime)
			);

			last = f.string();
		} catch (const std::exception &e) {
			if (last.empty()) {
				throw details::WrapLazyException{
				    "Could not read first frame from " + f.string() + ": " +
				        utils::What(e),
				    details::WrapLazyException::FromException(e)
				};
			} else {
				error = std::make_unique<CorruptedHermesFileError>(
				    "could not read first frame from '" + f.string() +
				        "': " + utils::What(e),
				    last,
				    std::numeric_limits<uint64_t>::max(),
				    details::WrapLazyException::FromException(e)
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
	} catch (fort::hermes::UnexpectedEndOfFileSequence &e) {
		error = std::make_unique<CorruptedHermesFileError>(
		    "could not read last frame from '" + last + "': " + e.message(),
		    last,
		    end,
		    std::move(e)
		);
	} catch (const std::exception &e) {
		throw details::WrapLazyException{
		    "could not extract last frame from " + last + ": " + utils::What(e),
		    details::WrapLazyException::FromException(e)
		};
	}

	return std::make_tuple(
	    std::make_pair(start, startDate),
	    std::make_pair(end, endDate),
	    std::move(error)
	);
}

std::multimap<FrameID, std::pair<fs::path, std::shared_ptr<TagID>>>
TrackingDataDirectory::ListTagCloseUpFiles(
    const fs::path &path, const std::string &prefix
) {
	std::multimap<FrameID, std::pair<fs::path, std::shared_ptr<TagID>>> res;

	std::regex        singleRx(prefix + "_([0-9]+)_(frame_)?([0-9]+).png");
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
			res.insert(std::make_pair(frameID, std::make_pair(de.path(), tagID))
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
TrackingDataDirectory::OpenFromFiles(
    const fs::path                          &absoluteFilePath,
    const std::string                       &URI,
    const std::unique_ptr<ProgressReporter> &progress,
    const slog::Logger<1>                   &logger
) {
	auto             ti             = std::make_shared<TrackingIndex>();
	auto             mi             = std::make_shared<MovieIndex>();
	auto             referenceCache = std::make_shared<FrameReferenceCache>();
	FixableErrorList errors;

	auto [hermesFiles, moviesPaths] = LookUpFiles(absoluteFilePath);
	if (hermesFiles.empty()) {
		throw cpptrace::invalid_argument(
		    absoluteFilePath.string() + " does not contains any .hermes file"
		);
	}
	logger.Info(
	    "listed files",
	    slog::Int("num_tracking_segments", hermesFiles.size()),
	    slog::Int("num_movie_segments", moviesPaths.size())
	);

	Time::MonoclockID monoID = GetUID(absoluteFilePath);

	auto [start, end, error] =
	    BuildIndexes(URI, monoID, hermesFiles, ti, logger);
	auto [startFrame, startDate] = start;
	auto [endFrame, endDate]     = end;

	if (error != nullptr) {
		logger.Error(
		    "Tracking segment indexation error",
		    fort::myrmidon::utils::Err(*error)
		);
		errors.push_back(std::move(error));
	}

	decltype(ListTagCloseUpFiles("", "")) closeUpFiles;

	for (const auto &location : CloseUpPaths) {
		if (fs::is_directory(absoluteFilePath / location.Subdir) == false) {
			continue;
		}
		closeUpFiles = ListTagCloseUpFiles(
		    absoluteFilePath / location.Subdir,
		    location.Prefix
		);
		break;
	}

	for (const auto &[frameID, s] : closeUpFiles) {
		auto [filepath, filter] = s;
		if (frameID > endFrame || frameID < startFrame) {
			errors.push_back(std::make_unique<NoKnownAcquisitionTimeFor>(
			    "could not access acquisition time for '" + filepath.string() +
			        "': frame range is [" + std::to_string(startFrame) + "; " +
			        std::to_string(endFrame) + "]",
			    filepath
			));
			logger.Error(
			    "unreachable frame for close-up",
			    slog::String("path", filepath.filename()),
			    slog::Int("close_up_frameID", frameID),
			    slog::Int("endFrame", endFrame)
			);
		} else {
			referenceCache->insert(std::make_pair(
			    frameID,
			    FrameReference(URI, frameID, Time::SinceEver())
			));
		}
	}

	auto movies = LoadMovieSegments(moviesPaths, URI, logger);
	movies.erase(
	    std::remove_if(
	        movies.begin(),
	        movies.end(),
	        [&logger, endFrame](const MovieSegment::Ptr &ms) {
		        if (ms->StartFrame() > endFrame) {
			        logger.Error(
			            "movie segment start out of experiment",
			            slog::String(
			                "segment",
			                ms->AbsoluteFilePath().filename()
			            ),
			            slog::Int("startFrame", ms->StartFrame()),
			            slog::Int("endFrame", endFrame)
			        );
			        return true;
		        }
		        return false;
	        }
	    ),
	    movies.end()
	);
	for (const auto &m : movies) {
		referenceCache->insert(std::make_pair(
		    m->StartFrame(),
		    FrameReference(URI, m->StartFrame(), Time::SinceEver())
		));
		if (m->EndFrame() <= endFrame) {
			referenceCache->insert(
			    std::make_pair(m->EndFrame(), FrameReference(URI, 0, Time()))
			);
		}
	}

	BuildFrameReferenceCache(
	    URI,
	    monoID,
	    absoluteFilePath,
	    ti,
	    *referenceCache,
	    progress,
	    errors,
	    logger
	);
	// caches the last frame
	referenceCache->insert(
	    std::make_pair(endFrame, FrameReference(URI, endFrame, endDate.Add(-1)))
	);

	std::set<FrameID> toErase;
	for (const auto &[frameID, ref] : *referenceCache) {
		// Allow for FrameID == 0???
		if (ref.Valid() == false) {
			toErase.insert(frameID);
			logger.Trace(
			    "removing missed frame reference",
			    slog::Int("ID", ref.FrameID()),
			    slog::FortTime("time", ref.Time())
			);
		}
	}

	for (const auto &m : movies) {
		auto fi = referenceCache->find(m->StartFrame());
		if (fi == referenceCache->cend() || fi->second.Valid() == false) {

			std::ostringstream oss;
			oss << "could not access acquisition time for frame "
			    << m->StartFrame() << ", starting frame of movie segment '"
			    << m->AbsoluteFilePath() << "', likely due to data corruption.";
			errors.push_back(std::make_unique<NoKnownAcquisitionTimeFor>(
			    oss.str(),
			    m->AbsoluteFilePath()
			));
			logger.Error(
			    "missing movie frame reference",
			    slog::Int("ID", m->StartFrame()),
			    slog::String("segment", m->AbsoluteFilePath().filename())
			);
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
		logger.Error(
		    "missing close-up frame reference",
		    slog::Int("ID", frameID),
		    slog::String("file", filepath.filename())
		);
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
	    std::move(errors)
	};
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
