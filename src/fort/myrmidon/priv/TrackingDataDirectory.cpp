#include "fort/myrmidon/types/CloseUp.hpp"
#include <filesystem>
extern "C" {
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
}

#include "TrackingDataDirectory.hpp"

#include <yaml-cpp/yaml.h>

#include <fort/utils/Defer.hpp>

#include <fort/video/PNG.hpp>
#include <fort/video/Reader.hpp>

#include <fort/hermes/Error.hpp>

#include <fort/myrmidon/utils/Checker.hpp>
#include <fort/myrmidon/utils/Exception.hpp>
#include <fort/myrmidon/utils/ObjectPool.hpp>

#include <fort/myrmidon/priv/RawFrame.hpp>
#include <fort/myrmidon/priv/TagCloseUp.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectoryError.hpp>

#include <fort/myrmidon/priv/proto/TDDCache.hpp>
#include <fort/myrmidon/priv/proto/TagCloseUpCache.hpp>
#include <fort/myrmidon/priv/proto/TagStatisticsCache.hpp>

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
	    SelectCloseUpLocation(absoluteFilePath),
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
    const CloseUpLocation             &closeUpLocation,
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
    , d_closeUpLocation(closeUpLocation)
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
		throw cpptrace::invalid_argument(os.str());
	}

	if (startdate.Before(enddate) == false) {
		std::ostringstream os;
		os << "TrackingDataDirectory: startDate:" << startdate
		   << " >= endDate: " << enddate;
		throw cpptrace::invalid_argument(os.str());
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

slog::Logger<0>
buildLogger(const fs::path &absolutePath, const OpenArguments &args) {
#ifndef NDEBUG
	constexpr auto level = slog::Level::Debug;
#else
	constexpr auto level = slog::Level::Info;
#endif
	std::vector<std::shared_ptr<slog::Sink>> sinks;
	sinks.reserve(3);

	sinks.push_back(slog::BuildSink(slog::WithFileOutput(
	    absolutePath / ".opening.log",
	    slog::WithFormat(slog::OutputFormat::JSON),
	    slog::FromLevel(level),
	    slog::WithAsync(),
	    slog::WithLocking()
	)));

	if (args.LogToStderr) {
		sinks.push_back(slog::BuildSink(slog::WithProgramOutput(
		    slog::WithFormat(slog::OutputFormat::TEXT),
		    slog::FromLevel(level),
		    slog::WithAsync(),
		    slog::WithLocking()
		)));
	}
	if (args.LogSink != nullptr) {
		sinks.push_back(args.LogSink);
	}

	if (sinks.size() == 1) {
		return slog::Logger<0>(sinks.front());
	}
	return slog::Logger<0>(slog::TeeSink(std::move(sinks)));
}

std::tuple<TrackingDataDirectory::Ptr, FixableErrorList>
TrackingDataDirectory::Open(
    const fs::path      &filepath,
    const fs::path      &experimentRoot,
    const OpenArguments &args
) {
	CheckPaths(filepath, experimentRoot);

	auto absoluteFilePath = fs::weakly_canonical(fs::absolute(filepath));
	auto URI = fs::relative(absoluteFilePath, fs::absolute(experimentRoot));
	auto logger =
	    buildLogger(absoluteFilePath, args).With(slog::String("URI", URI));

	Ptr              res;
	FixableErrorList errors;
	bool             saveCache = false;

	try {
		res = LoadFromCache(absoluteFilePath, URI.generic_string());
		logger.Debug("loaded from cache");
	} catch (const std::exception &e) {
		logger.Warn("could not load from cache", utils::Err(e));
		if (args.Progress) {
			args.Progress->ReportError(
			    std::string{"could not load from cache: "} + utils::What(e)
			);
		}
		std::tie(res, errors) = OpenFromFiles(
		    absoluteFilePath,
		    URI.generic_string(),
		    args.Progress,
		    logger
		);
		logger.Info("loaded from file", slog::Int("num_errors", errors.size()));
		if (errors.empty()) {
			saveCache = true;
		} else {
			if (args.FixCorruptedData == false) {
				logger.Warn(
				    "not saving to cache",
				    slog::Int("num_errors", errors.size())
				);
				if (args.Progress != nullptr) {
					args.Progress->ReportError(
					    "not saving to cache as it got " +
					    std::to_string(errors.size()) + " errors"
					);
					for (const auto &e : errors) {
						args.Progress->ReportError(utils::What(*e));
					}
				}
			} else {
				saveCache = true;
				for (const auto &e : errors) {
					logger.Info(
					    "applying fix",
					    slog::String("what", e->FixDescription())
					);
					if (args.Progress != nullptr) {
						args.Progress->ReportError(
						    std::string("Got error: ") + e->message() +
						    "\nApplying fix: " + e->FixDescription()
						);
					}
					e->Fix();
				}
			}
		}
	}

	if (saveCache == true) {
		try {
			res->SaveToCache();
			logger.Info("cached tracking data");
		} catch (const std::exception &e) {
			logger.Error("could not cache tracking data", utils::Err(e));
			if (args.Progress != nullptr) {
				args.Progress->ReportError(
				    std::string{"could not cache tracking data: "} +
				    utils::What(e)
				);
			}
		}
	}

	res->LoadComputedFromCache();
	res->LoadDetectionSettings();
	logger.Debug("opened");

	return std::make_tuple(res, std::move(errors));
}

const TrackingDataDirectory::TrackingIndex &
TrackingDataDirectory::TrackingSegments() const {
	return *d_segments;
}

TrackingDataDirectory::const_iterator::const_iterator(
    const TrackingDataDirectory::Ptr &parent, uint64_t current
)
    : d_parent(parent)
    , d_current(current) {}

TrackingDataDirectory::const_iterator &
TrackingDataDirectory::const_iterator::operator=(const const_iterator &other) {
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

TrackingDataDirectory::const_iterator::const_iterator(const_iterator &&other)
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
			d_frame = RawFrame::Create(parent->d_URI, d_message, parent->d_uid);
		} catch (const fort::hermes::UnexpectedEndOfFileSequence &e) {
			auto lastValidID = d_current;
			auto lastValidTime =
			    d_frame ? d_frame->Frame().Time() : parent->Start();

			d_current = parent->d_endFrame + 1;
			d_frame.reset();
			std::optional<FrameID> next;

			if (e.FileLineContext().Next.has_value()) {
				const auto &segments = parent->TrackingSegments().Segments();

				auto iter = std::find_if(
				    segments.begin(),
				    segments.end(),
				    [next = e.FileLineContext().Next.value().filename().string(
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
			    details::WrapLazyException::FromException(e)
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

TrackingDataDirectory::const_iterator
TrackingDataDirectory::FrameAt(uint64_t frameID) const {
	if (frameID < d_startFrame || frameID > d_endFrame) {
		return end();
	}
	return const_iterator(
	    std::const_pointer_cast<TrackingDataDirectory>(shared_from_this()),
	    frameID
	);
}

TrackingDataDirectory::const_iterator
TrackingDataDirectory::FrameAfter(const Time &t) const {
	if (t < Start()) {
		std::ostringstream oss;
		oss << t << " is not in [" << Start() << ",+∞[";
		throw cpptrace::out_of_range(oss.str());
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

FrameReference TrackingDataDirectory::FrameReferenceAt(FrameID frameID) const {
	auto fi = d_referencesByFID->find(frameID);
	if (fi != d_referencesByFID->cend()) {
		return fi->second;
	}
	auto it = FrameAt(frameID);
	if (it == end()) {
		throw cpptrace::out_of_range(
		    "Could not find frame " + std::to_string(frameID) + " in [" +
		    std::to_string(d_startFrame) + ";" + std::to_string(d_endFrame) +
		    "]"
		);
	}

	return (*it)->Frame();
}

FrameReference TrackingDataDirectory::FrameReferenceAfter(const Time &t) const {
	auto fi = d_frameIDByTime.find(t);
	if (fi != d_frameIDByTime.cend()) {
		return FrameReferenceAt(fi->second);
	}
	auto it = FrameAfter(t);
	if (it == end()) {
		throw cpptrace::out_of_range(
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
    const fs::path &absoluteFilePath, const std::string &URI
) {
	return proto::TDDCache::Load(absoluteFilePath, URI);
}

void TrackingDataDirectory::SaveToCache() const {
	proto::TDDCache::Save(
	    std::const_pointer_cast<TrackingDataDirectory>(shared_from_this())
	);
}

std::shared_ptr<std::map<FrameReference, fs::path>>
TrackingDataDirectory::EnumerateFullFrames(const fs::path &subpath
) const noexcept {
	auto dirpath = AbsoluteFilePath() / subpath;
	if (fs::is_directory(dirpath) == false) {
		return {};
	}

	try {
		auto listing = ListTagCloseUpFiles(dirpath, "");
		auto res     = std::make_shared<std::map<FrameReference, fs::path>>();
		for (const auto &[frameID, fileAndFilter] : listing) {
			if (frameID <= d_endFrame &&
			    fileAndFilter.second.has_value() == false) {
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
    : cpptrace::runtime_error(
          "Computed ressource " + typeName + " is not available"
      ) {}

TrackingDataDirectory::ComputedRessourceUnavailable::
    ~ComputedRessourceUnavailable() noexcept {}

const std::vector<TagCloseUp::ConstPtr> &
TrackingDataDirectory::TagCloseUps() const {
	if (TagCloseUpsComputed() == false) {
		throw ComputedRessourceUnavailable("TagCloseUp");
	}
	return *d_tagCloseUps;
}

const std::map<FrameReference, fs::path> &
TrackingDataDirectory::FullFrames() const {
	if (FullFramesComputed() == false) {
		throw ComputedRessourceUnavailable("FullFrame");
	}
	return *d_fullFrames;
}

const TagStatisticsHelper::Timed &TrackingDataDirectory::TagStatistics() const {
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

	void Reduce(size_t index, const std::vector<TagCloseUp::ConstPtr> &tcus) {
		d_closeUps[index] = tcus;
		if ((d_count.fetch_sub(1) - 1) > 0) {
			return;
		}
		d_tdd->d_tagCloseUps =
		    std::make_shared<std::vector<TagCloseUp::ConstPtr>>();
		for (const auto &tcus : d_closeUps) {
			d_tdd->d_tagCloseUps
			    ->insert(d_tdd->d_tagCloseUps->end(), tcus.begin(), tcus.end());
		}
		proto::TagCloseUpCache::Save(
		    d_tdd->AbsoluteFilePath(),
		    d_tdd->d_closeUpLocation.Subdir,
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
				    av_get_pix_fmt_name(frame.Format)
				};
			}
			return image_u8_t{
			    .width  = frame.Size.Width,
			    .height = frame.Size.Height,
			    .stride = frame.Linesize[0],
			    .buf    = frame.Planes[0],
			};
		}

		std::tuple<std::vector<TagCloseUp::ConstPtr>, FixableError::Ptr> Detect(
		    const TrackingDataDirectory::TagCloseUpFileAndFilter &fileAndFilter,
		    const FrameReference                                 &reference
		) {

			std::vector<TagCloseUp::ConstPtr> res;

			zarray_t *detections = nullptr;
			try {
				auto img   = video::ReadPNG(fileAndFilter.first);
				auto img8  = AsImageU8(*img);
				detections = apriltag_detector_detect(d_detector, &img8);
			} catch (const std::exception &e) {
				std::ostringstream oss;
				oss << "could not read image " << fileAndFilter.first << ": "
				    << utils::What(e);

				return {
				    res,
				    std::make_unique<NoKnownAcquisitionTimeFor>(
				        oss.str(),
				        fileAndFilter.first,
				        details::WrapLazyException::FromException(e)
				    ),
				};
			}

			defer {
				apriltag_detections_destroy(detections);
			};
			apriltag_detection *d;
			if (fileAndFilter.second.has_value() == false) {
				for (int i = 0; i < zarray_size(detections); ++i) {
					zarray_get(detections, i, &d);
					auto td = TagDetection::Convert(d);
					res.push_back(std::make_shared<TagCloseUp>(
					    fileAndFilter.first,
					    reference,
					    td.ID,
					    std::vector<TagDetection>{td}
					));
				}
				return {res, nullptr};
			}
			std::vector<TagDetection> all;
			all.reserve(zarray_size(detections));
			for (int i = 0; i < zarray_size(detections); ++i) {
				zarray_get(detections, i, &d);
				all.emplace_back(TagDetection::Convert(d));
			}
			res.push_back(std::make_shared<TagCloseUp>(
			    fileAndFilter.first,
			    reference,
			    fileAndFilter.second.value(),
			    all
			));

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
	// remove all disabled frame
	for (const auto &de : fs::directory_iterator(
	         AbsoluteFilePath() / d_closeUpLocation.Subdir
	     )) {
		if (de.path().stem().extension() != ".dis" ||
		    de.path().filename().string().starts_with(d_closeUpLocation.Prefix
		    ) == false) {
			continue;
		}
		auto newName =
		    de.path().parent_path() /
		    (de.path().stem().stem().string() + de.path().extension().string());
		fs::rename(de.path(), newName);
	}

	auto tagCloseUpFiles = ListTagCloseUpFiles(
	    AbsoluteFilePath() / d_closeUpLocation.Subdir,
	    d_closeUpLocation.Prefix
	);

	// we discard all close-up which are out-of-range
	tagCloseUpFiles.erase(
	    tagCloseUpFiles.upper_bound(d_endFrame),
	    tagCloseUpFiles.end()
	);

	if (tagCloseUpFiles.empty() ||
	    d_detectionSettings.Family == tags::Family::Undefined) {
		d_tagCloseUps = std::make_shared<std::vector<TagCloseUp::ConstPtr>>();
		proto::TagCloseUpCache::Save(
		    AbsoluteFilePath(),
		    d_closeUpLocation.Subdir,
		    {}
		);
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
	TagStatisticsReducer(size_t count, const TrackingDataDirectory::Ptr &tdd)
	    : d_tdd(tdd)
	    , d_stats(count) {
		d_count.store(count);
	}

	void Reduce(size_t index, const TagStatisticsHelper::Timed &stats) {
		d_stats[index] = stats;
		if ((d_count.fetch_sub(1) - 1) > 0) {
			return;
		}
		d_tdd->d_tagStatistics = std::make_shared<TagStatisticsHelper::Timed>(
		    TagStatisticsHelper::MergeTimed(d_stats.begin(), d_stats.end())
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
		d_tdd->d_fullFrames = d_tdd->EnumerateFullFrames(
		    fs::path{d_tdd->d_closeUpLocation.Subdir} / "computed"
		);
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

	fs::create_directory(
	    AbsoluteFilePath() / d_closeUpLocation.Subdir / "computed"
	);
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
			    {width, height}
			};

			auto frame = v.CreateFrame();
			if (v.Read(*frame) == false) {
				return nullptr;
			}

			auto filename = "frame_" +
			                std::to_string(ms.second->ToTrackingFrameID(0)) +
			                ".png";
			auto imgPath = AbsoluteFilePath() / d_closeUpLocation.Subdir /
			               "computed" / filename;
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
		d_tagCloseUps  = std::make_shared<std::vector<TagCloseUp::ConstPtr>>();
		*d_tagCloseUps = proto::TagCloseUpCache::Load(
		    AbsoluteFilePath(),
		    d_closeUpLocation.Subdir,
		    [this](FrameID frameID) -> FrameReference {
			    return FrameReferenceAt(frameID);
		    }
		);
	} catch (const std::exception &e) {
		d_tagCloseUps.reset();
	}

	d_fullFrames = EnumerateFullFrames(d_closeUpLocation.Subdir);
	if (!d_fullFrames || d_fullFrames->empty()) {
		d_fullFrames = EnumerateFullFrames(
		    fs::path{d_closeUpLocation.Subdir} / "computed"
		);
	}
}

void TrackingDataDirectory::LoadDetectionSettings() {
	auto path = AbsoluteFilePath() / "leto-final-config.yml";
	if (fs::exists(path) == false) {
		path = AbsoluteFilePath() / "leto-final-config.yaml";
		if (fs::exists(path) == false) {
			throw cpptrace::runtime_error(
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
    const std::vector<Ptr> &list, const Time &start, const Time &end
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
    std::ostream &out, const fort::myrmidon::priv::TrackingDataDirectory &a
) {
	return out << "TDD{URI:'" << a.URI() << "', start:" << a.Start()
	           << ", end:" << a.End() << "}";
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
