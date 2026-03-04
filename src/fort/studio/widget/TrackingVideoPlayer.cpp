#include "TrackingVideoPlayer.hpp"
#include "fort/myrmidon/utils/Exception.hpp"

#include <QThread>
#include <QTimer>
#include <libavutil/pixfmt.h>
#include <limits>

#include <fort/studio/Format.hpp>
#include <fort/studio/bridge/ConcurrentFrameLoader.hpp>

// #include "TrackingVideoWidget.hpp"

TrackingVideoPlayer::TrackingVideoPlayer(QObject *parent)
    : QObject(parent)
    , d_task(nullptr)
    , d_state(State::Stopped)
    , d_rate(1.0)
    , d_movieThread(new QThread())
    , d_timer(new QTimer(this))
    , d_fps(8.0)
    , d_scrollMode(false)
    , d_currentTaskID(0)
    , d_seekReady(true)
    , d_logger(slog::With(slog::String("module", "TrackingVideoPlayer"))) {

	d_movieThread->start();
	qRegisterMetaType<fort::Time>();
	qRegisterMetaType<fort::Duration>();
	qRegisterMetaType<size_t>();
	qRegisterMetaType<TrackingVideoFrame>();

	connect(
	    d_timer,
	    &QTimer::timeout,
	    this,
	    &TrackingVideoPlayer::onTimerTimeout
	);
}

TrackingVideoPlayer::~TrackingVideoPlayer() {
	d_movieThread->quit();
	d_movieThread->wait();
	d_movieThread->deleteLater();
}

void TrackingVideoPlayer::setup(ConcurrentFrameLoader *loader) {
	d_loader = loader;
	d_loader->setParent(nullptr);
	connect(d_movieThread, &QThread::finished, d_loader, &QObject::deleteLater);
	d_loader->ConcurrentFrameLoader::moveToThread(d_movieThread);
	connect(
	    d_loader,
	    &ConcurrentFrameLoader::done,
	    this,
	    &TrackingVideoPlayer::setSeekReady,
	    Qt::QueuedConnection
	);
	connect(
	    d_loader,
	    &ConcurrentFrameLoader::durationComputed,
	    this,
	    &TrackingVideoPlayer::setDuration,
	    Qt::QueuedConnection
	);
	setSeekReady(d_loader->isDone());
}

bool TrackingVideoPlayer::isSeekReady() const {
	return d_seekReady;
}

TrackingVideoPlayer::State TrackingVideoPlayer::playbackState() const {
	return d_state;
}

qreal TrackingVideoPlayer::playbackRate() const {
	return d_rate;
}

fort::Duration TrackingVideoPlayer::position() const {
	return d_position;
}

fort::Duration TrackingVideoPlayer::duration() const {
	return d_duration;
}

fort::Time TrackingVideoPlayer::start() const {
	return d_start;
}

void TrackingVideoPlayer::stopTask() {
	if (d_task == nullptr) {
		return;
	}
	d_task->deleteLater();
	d_task = nullptr;
	d_frames.clear();
	d_stagging.clear();
	++d_currentTaskID;
}

void TrackingVideoPlayer::bootstrapTask(
    quint32 spaceID, const fmp::TrackingDataDirectory::Ptr &tdd
) {
	if (d_task == nullptr) {
		return;
	}

	std::vector<TrackingVideoFrame> frames;

	for (size_t i = 0; i < BUFFER_SIZE; ++i) {
		TrackingVideoFrame f;
		f.Data = d_task->allocate();
		frames.push_back(f);
	}

	d_task->moveToThread(d_movieThread);
	connect(d_movieThread, &QThread::finished, d_task, &QObject::deleteLater);

	connect(
	    d_task,
	    &TrackingVideoPlayerTask::newFrame,
	    this,
	    &TrackingVideoPlayer::onNewVideoFrame,
	    Qt::QueuedConnection
	);

	d_task->startLoadingFrom(spaceID, tdd);

	for (const auto &f : frames) {
		d_task->processNewFrame(f);
	}
}

void TrackingVideoPlayer::clearMovieSegment() {
	stop();
	stopTask();
	d_segment.reset();
	emit durationChanged(fort::Time(), 0, 8);
	emit positionChanged(0);
}

void TrackingVideoPlayer::setMovieSegment(
    quint32                                spaceID,
    const fmp::TrackingDataDirectory::Ptr &tdd,
    const fmp::MovieSegment::ConstPtr     &segment,
    const fort::Time                      &start
) {
	if (segment == nullptr) {
		return;
	}

	stopTask();

	d_segment = segment;

	try {
		d_task = new TrackingVideoPlayerTask(
		    d_currentTaskID,
		    d_segment,
		    computeRate(d_rate),
		    d_loader
		);
		d_currentSeekID = 0;
		d_fps           = d_task->fps();
		d_interval      = fort::Duration::Second.Nanoseconds() / d_task->fps();
		d_start         = start;
		setDuration(
		    d_interval *
		    (d_segment->EndMovieFrame() - d_segment->StartMovieFrame())
		);
		d_timer->setInterval(
		    d_interval.Milliseconds() / d_rate * computeRate(d_rate)
		);
		d_position = 0;
		emit positionChanged(d_position);
		setSeekReady(false);
		d_displayNext = true;
	} catch (const std::exception &e) {
		d_logger.Error(
		    "unexpected error during loading",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
		d_task = nullptr;
		return;
	}

	bootstrapTask(spaceID, tdd);
}

void TrackingVideoPlayer::pause() {
	if (!d_segment || d_state != State::Playing) {
		return;
	}
	d_state = State::Paused;
	d_timer->stop();
	emit playbackStateChanged(d_state);
}

void TrackingVideoPlayer::play() {
	if (!d_segment || d_state == State::Playing || d_task == nullptr) {
		return;
	}
	d_state = State::Playing;
	d_timer->start(d_interval.Milliseconds() / d_rate);
	emit playbackStateChanged(d_state);
}

void TrackingVideoPlayer::stop() {
	if (!d_segment || d_state == State::Stopped) {
		return;
	}
	d_state = State::Stopped;
	d_timer->stop();
	displayVideoFrameImpl(TrackingVideoFrame());
	emit playbackStateChanged(d_state);
}

size_t TrackingVideoPlayer::computeRate(double rate) {
	return std::max(1.0, std::floor(rate / 4));
}

void TrackingVideoPlayer::setPlaybackRate(qreal rate) {
	if (rate == d_rate || rate <= 0.0) {
		return;
	}
	d_rate     = rate;
	auto pRate = computeRate(d_rate);
	if (d_interval.Nanoseconds() != 0) {
		d_timer->setInterval(d_interval.Milliseconds() / d_rate * pRate);
	}
	if (d_task != nullptr) {
		d_task->setRate(pRate);
	}
	emit playbackRateChanged(rate);
}

void TrackingVideoPlayer::setPosition(fort::Duration position) {
	VIDEO_PLAYER_DEBUG({
		slog::Trace(
		    "setPosition",
		    slog::Location(),
		    slog::Pointer("currentThread", QThread::currentThread()),
		    slog::Pointer("affinityThread", thread())
		);
	});

	position = std::clamp(position, fort::Duration(0), d_duration);

	if (d_task == nullptr || position == d_position) {
		return;
	}
	d_task->seek(++d_currentSeekID, position);
	VIDEO_PLAYER_DEBUG({
		slog::Info(
		    "seeking",
		    slog::Location(),
		    slog::Duration("position", position.ToChrono()),
		    slog::Int("seekID", d_currentSeekID)
		);
	});

	d_displayNext = true;
	for (auto &f : d_frames) {
		VIDEO_PLAYER_DEBUG({
			slog::Warn(
			    "resending buffered frame",
			    slog::Location(),
			    slogVideoFrame("frame", f)
			);
		});
		d_task->processNewFrame(f);
	}
	d_frames.clear();

	for (auto &f : d_stagging) {
		VIDEO_PLAYER_DEBUG({
			slog::Warn(
			    "resending stagged frame",
			    slog::Location(),
			    slogVideoFrame("frame", f)
			);
		});
		d_task->processNewFrame(f);
	}
	d_stagging.clear();
}

void TrackingVideoPlayer::onNewVideoFrame(
    size_t taskID, size_t seekID, TrackingVideoFrame frame
) {
	VIDEO_PLAYER_DEBUG({
		slog::Trace(
		    "onNewVideoFrame",
		    slog::Pointer("current_thread", QThread::currentThread()),
		    slog::Pointer("affinity_thread", thread()),
		    slog::Int("task_id", taskID),
		    slog::Int("seek_id", seekID)
		);
	});

	if (taskID != d_currentTaskID) {
		VIDEO_PLAYER_DEBUG({
			slog::Error(
			    "mismatching taskID, dropping frame",
			    slog::Location(),
			    slog::Int("expected", d_currentTaskID),
			    slog::Int("received", taskID),
			    slogVideoFrame("frame", frame)
			);
		});
		// this videoframe is from a dead task
		return;
	}

	if (seekID != d_currentSeekID) {
		VIDEO_PLAYER_DEBUG({
			slog::Warn(
			    "mismatching seekID",
			    slog::Location(),
			    slog::Int("expected", d_currentSeekID),
			    slog::Int("received", seekID),
			);
		});
		if (d_scrollMode == true) {
			VIDEO_PLAYER_DEBUG({
				slog::Warn(
				    "in scroll mode, displaying frame",
				    slog::Location(),
				    slogVideoFrame("frame", frame)
				);
			});
			displayVideoFrameImpl(frame);
		} else {
			d_task->processNewFrame(frame);
		}
		return;
	}

	if (frame.FrameID == std::numeric_limits<fmp::MovieFrameID>::max()) {
		// no frame case at end of file.
		VIDEO_PLAYER_DEBUG({
			slog::Info(
			    "EOF, stagging frame",
			    slog::Location(),
			    slogVideoFrame("frame", frame)
			);
		});
		d_stagging.push_back(frame);
		return;
	}

	if (d_displayNext == false && frame.EndPos < d_position) {
		VIDEO_PLAYER_DEBUG({
			slog::Warn(
			    "discarding old frame",
			    slog::Location(),
			    slogVideoFrame("frame", frame)
			);
		});
		// already old, we discard it
		d_task->processNewFrame(frame);
		return;
	}

	if (d_displayNext == true) {
		if (d_position == frame.StartPos) {
			// avoids forward deadlock while jumping next frame, can
			// happen depending on OpenCV implementation.
			VIDEO_PLAYER_DEBUG({
				slog::Debug(
				    "did not advance, releasing it",
				    slog::Location(),
				    slogVideoFrame("frame", frame)
				);
			});
			d_task->processNewFrame(frame);
			return;
		}
		VIDEO_PLAYER_DEBUG({
			slog::Debug(
			    "displaying first frame",
			    slog::Location(),
			    slogVideoFrame("frame", frame),
			    slog::Int("count", d_frames.size())
			);
		});

		d_displayNext = false;
		d_position    = frame.StartPos;
		displayVideoFrameImpl(frame);
		if (d_scrollMode == false) {
			emit positionChanged(d_position);
		}
		return;
	}

	VIDEO_PLAYER_DEBUG({
		slog::Debug(
		    "queuing frame",
		    slog::Location(),
		    slogVideoFrame("frame", frame)
		);
	});
	d_frames.push_back(frame);
}

void TrackingVideoPlayer::onTimerTimeout() {
	auto expectedPosition = d_position + d_interval * computeRate(d_rate);
	VIDEO_PLAYER_DEBUG({
		slog::Trace(
		    "onTimerTimeout",
		    slog::Location(),
		    slog::Duration("expected", d_position.ToChrono())
		);
	});

	VIDEO_PLAYER_DEBUG({
		size_t i = 0;
		for (const auto &f : frames) {
			slog::Debug(
			    "before sort",
			    slog::Location(),
			    slog::Int(index, i++),
			    slogVideoFrame("frame", f)
			);
		}
	});

	std::sort(
	    d_frames.begin(),
	    d_frames.end(),
	    [](const TrackingVideoFrame &a, const TrackingVideoFrame &b) {
		    return a.StartPos < b.StartPos;
	    }
	);

	std::deque<TrackingVideoFrame> deleted;
	auto                           last = std::remove_if(
        d_frames.begin(),
        d_frames.end(),
        [&deleted, expectedPosition](const TrackingVideoFrame &a) {
            if (a.EndPos <= expectedPosition) {
                deleted.push_back(a);
                return true;
            }
            return false;
        }
    );

	VIDEO_PLAYER_DEBUG({
		size_t i = 0;
		for (const auto &f : d_frames) {
			slog::Debug(
			    "before sort",
			    slog::Location(),
			    slog::Int(index, i++),
			    slogVideoFrame("frame", f)
			);
		}

		i = 0;
		for (const auto &f : deleted) {
			slog::Debug(
			    "to be removed",
			    slog::Location(),
			    slog::Int(index, i++),
			    slogVideoFrame("frame", f)
			);
		}
	});
	d_frames.erase(last, d_frames.end());

	if (d_frames.empty() == true && deleted.empty() == false &&
	    computeRate(d_rate) > 1) {
		VIDEO_PLAYER_DEBUG({
			slog::Info(
			    "reset time to",
			    slog::Location(),
			    slog::Duration("position", deleted.back().StartPos.ToChrono())
			);
		});
		expectedPosition = deleted.back().StartPos;
	}

	// removes expired frames
	for (const auto &f : deleted) {
		VIDEO_PLAYER_DEBUG({
			slog::Debug(
			    "will release",
			    slog::Location(),
			    slogVideoFrame("frame", f)
			);
		});
		if (d_task != nullptr) {
			d_task->processNewFrame(f);
		}
	}

	VIDEO_PLAYER_DEBUG({
		size_t i = 0;
		for (const auto &f : d_frames) {
			slog::Debug(
			    "after release",
			    slog::Location(),
			    slog::Int(index, i++),
			    slogVideoFrame("frame", f)
			);
		}
	});

	if (d_frames.empty() == true) {
		if (d_stagging.empty() == false) {
			stop();
		}
		d_position = expectedPosition;
		emit positionChanged(d_position);
		return;
	}

	if (d_frames.front().StartPos > expectedPosition) {
		d_position = expectedPosition;
		emit positionChanged(d_position);
		return;
	}

	displayVideoFrameImpl(d_frames.front());
	d_position = d_frames.front().StartPos;
	emit positionChanged(d_position);
	d_frames.pop_front();
}

void TrackingVideoPlayer::displayVideoFrameImpl(const TrackingVideoFrame &frame
) {
	VIDEO_PLAYER_DEBUG({
		slog::Debug(
		    "displaying frame",
		    slog::Location(),
		    slogVideoFrame("frame", frame)
		);
	});

	if (!d_displayed.Data == false && d_task != nullptr) {
		VIDEO_PLAYER_DEBUG({
			slog::Debug(
			    "releasing",
			    slog::Location(),
			    slogVideoFrame("frame", d_displayed)
			);
		});
		d_task->processNewFrame(d_displayed);
	}
	d_displayed = frame;
	emit displayVideoFrame(d_displayed);
}

void TrackingVideoPlayer::togglePlayPause() {
	switch (d_state) {
	case State::Playing:
		pause();
		break;
	case State::Paused:
	case State::Stopped:
		play();
		break;
	}
}

void TrackingVideoPlayer::jumpNextFrame() {
	if (d_state != State::Paused) {
		return;
	}
	if (d_rate != 1.0) {
		setPlaybackRate(1.0);
		setPosition(d_position + d_interval);
	} else {
		onTimerTimeout();
	}
}

void TrackingVideoPlayer::jumpPrevFrame() {
	if (d_state != State::Paused || d_displayNext == true) {
		return;
	}
	if (d_rate != 1.0) {
		setPlaybackRate(1.0);
	}
	setPosition(d_position - d_interval);
}

void TrackingVideoPlayer::skipDuration(fort::Duration duration) {
	if (d_task == nullptr) {
		return;
	}
	setPosition(d_position + duration);
}

void TrackingVideoPlayer::setTime(const fort::Time &time) {
	if (d_task == nullptr) {
		return;
	}
	auto actualTime = std::clamp(time, d_start, d_start.Add(d_duration));
	setPosition(actualTime.Sub(d_start));
}

bool TrackingVideoPlayer::scrollMode() const {
	return d_scrollMode;
}

void TrackingVideoPlayer::setScrollMode(bool scrollMode) {
	d_scrollMode = scrollMode;
}

const fmp::MovieSegment::ConstPtr &TrackingVideoPlayer::currentSegment() const {
	return d_segment;
}

void TrackingVideoPlayer::setSeekReady(bool ready) {
	if (ready == d_seekReady) {
		return;
	}
	d_seekReady = ready;
	emit seekReady(d_seekReady);
}

void TrackingVideoPlayer::setDuration(fort::Duration duration) {
	if (d_duration == duration) {
		return;
	}
	d_duration = duration;
	emit durationChanged(d_start, d_duration, d_fps);
}

void TrackingVideoPlayer::jumpNextVisible(fm::AntID antID, bool backward) {
	if (d_task == nullptr || d_seekReady == false ||
	    d_displayed.Contains(antID) == true || antID == 0) {
		return;
	}

	fort::Duration position;

	metaObject()->invokeMethod(
	    d_loader,
	    "findAnt",
	    Qt::BlockingQueuedConnection,
	    Q_RETURN_ARG(fort::Duration, position),
	    Q_ARG(quint32, antID),
	    Q_ARG(quint64, d_displayed.FrameID),
	    Q_ARG(int, backward == true ? -1 : 1)
	);

	if (position < 0) {
		return;
	}
	setPosition(position);
}

TrackingVideoPlayerTask::TrackingVideoPlayerTask(
    size_t                             taskID,
    const fmp::MovieSegment::ConstPtr &segment,
    size_t                             rate,
    ConcurrentFrameLoader             *loader
)
    : QObject(nullptr)
    , d_segment(segment)
    , d_reader(segment->AbsoluteFilePath().c_str(), AV_PIX_FMT_BGRA)
    , d_loader(loader)
    , d_taskID(taskID)
    , d_seekID(0)
    , d_rate(std::max(size_t(1), rate)) {
	d_width                 = d_reader.Size().Width;
	d_height                = d_reader.Size().Height;
	d_expectedFrameDuration = d_reader.AverageFrameDuration();
}

TrackingVideoPlayerTask::~TrackingVideoPlayerTask() {}

double TrackingVideoPlayerTask::fps() const {
	return double(1e9) / d_expectedFrameDuration.Nanoseconds();
}

qint64 TrackingVideoPlayerTask::numberOfFrame() const {
	return d_reader.Length();
}

void TrackingVideoPlayerTask::processNewFrame(TrackingVideoFrame frame) {
	metaObject()->invokeMethod(
	    this,
	    "processNewFrameUnsafe",
	    Qt::QueuedConnection,
	    Q_ARG(TrackingVideoFrame, frame)
	);
}

void TrackingVideoPlayerTask::seek(size_t seekID, fort::Duration position) {
	VIDEO_PLAYER_DEBUG({
		slog::Info(
		    "seek",
		    slog::Location(),
		    slog::Int("seekID", seekID),
		    slog::Duration("position", position.ToChrono()),
		    slog::Pointer("current_thread", QThread::currentThread()),
		    slog::Pointer("affinity_thread", thread())
		);
	});
	metaObject()->invokeMethod(
	    this,
	    "seekUnsafe",
	    Qt::BlockingQueuedConnection,
	    Q_ARG(size_t, seekID),
	    Q_ARG(fort::Duration, position)
	);
}

void TrackingVideoPlayerTask::setRate(size_t rate) {
	VIDEO_PLAYER_DEBUG({
		slog::Info("setRate", slog::Location(), slog::Int("rate", rate));
	});
	metaObject()->invokeMethod(
	    this,
	    "setRateUnsafe",
	    Qt::BlockingQueuedConnection,
	    Q_ARG(size_t, rate)
	);
}

void TrackingVideoPlayerTask::setRateUnsafe(size_t rate) {
	d_rate = std::max(rate, size_t(1));
}

void TrackingVideoPlayerTask::processNewFrameUnsafe(TrackingVideoFrame frame) {
	VIDEO_PLAYER_DEBUG({
		slog::Trace(
		    "processingImage",
		    slog::Location(),
		    slog::Pointer("image", frame.Image.get()),
		    slog::Pointer("current_thread", QThread::currentThread()),
		    slog::Pointer("affinity_thread", thread())
		);
	});

	for (size_t i = 0; i < d_rate; ++i) {

		if (d_reader.Grab() == false) {
			VIDEO_PLAYER_DEBUG({
				slog::Warn(
				    "could not capture image",
				    slog::Location(),
				    slog::Pointer("image", frame.Image.get())
				);
			});

			frame.FrameID = std::numeric_limits<fmp::MovieFrameID>::max();
			emit newFrame(d_taskID, d_seekID, frame);
			return;
		}
		frame.FrameID = d_reader.Position();
	}

	d_reader.Receive(*frame.Data);

	VIDEO_PLAYER_DEBUG({
		slog::Debug(
		    "emitting new frame image",
		    slog::Location(),
		    slogVideoFrame("frame", frame),
		    slog::Int("seekID", d_seekID),
		    slog::Pointer("current_thread", QThread::currentThread()),
		    slog::Pointer("affinity_thread", thread())
		);
	});

	frame.StartPos       = d_loader->positionAt(frame.FrameID);
	frame.EndPos         = d_loader->positionAt(frame.FrameID + 1) - 1;
	frame.TrackingFrame  = d_loader->frameAt(frame.FrameID);
	frame.CollisionFrame = d_loader->collisionAt(frame.FrameID);
	emit newFrame(d_taskID, d_seekID, frame);
}

std::shared_ptr<fort::video::Frame> TrackingVideoPlayerTask::allocate() const {
	return d_reader.CreateFrame(64);
}

void TrackingVideoPlayerTask::seekUnsafe(
    size_t seekID, fort::Duration position
) {
	VIDEO_PLAYER_DEBUG({
		slog::Trace(
		    "seekUnsafe",
		    slog::Location(),
		    slog::Int("seekID", d_seekID),
		    slog::Duration("position", position.ToChrono()),
		    slog::Pointer("current_thread", QThread::currentThread()),
		    slog::Pointer("affinity_thread", thread())
		);
	});
	d_seekID     = seekID;
	auto frameID = d_loader->frameIDAt(position);
	if (d_reader.Position() == frameID) {
		return;
	}
	d_reader.SeekFrame(frameID, false);
};

void TrackingVideoPlayerTask::startLoadingFrom(
    quint32 spaceID, const fmp::TrackingDataDirectory::Ptr &tdd
) {
	VIDEO_PLAYER_DEBUG({ slog::Trace("startLoadingFrom", slog::Location()); });

	metaObject()->invokeMethod(
	    this,
	    "startLoadingFromUnsafe",
	    Qt::BlockingQueuedConnection,
	    Q_ARG(quint32, spaceID),
	    Q_ARG(fmp::TrackingDataDirectory::Ptr, tdd)
	);
}

void TrackingVideoPlayerTask::startLoadingFromUnsafe(
    quint32 spaceID, fmp::TrackingDataDirectory::Ptr tdd
) {
	VIDEO_PLAYER_DEBUG({
		slog::Trace("startLoadingFromUnsafe", slog::Location());
	});
	d_loader
	    ->loadMovieSegment(spaceID, tdd, d_segment, d_expectedFrameDuration);
}
