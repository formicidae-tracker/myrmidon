#pragma once

#include <QObject>

#include <fort/studio/MyrmidonTypes.hpp>

#include <opencv2/videoio.hpp>

#include <vector>
#include <set>
#include <condition_variable>

#include "TrackingVideoFrame.hpp"

class QOpenGLBuffer;

class TrackingVideoWidget;
class QThread;

class TrackingVideoPlayerTask;
class TrackingVideoPlayer : public QObject {
	Q_OBJECT
	Q_PROPERTY(qreal playbackRate
	           READ playbackRate
	           WRITE setPlaybackRate
	           NOTIFY playbackRateChanged)
	Q_PROPERTY(State playbackState
	           READ playbackState
	           NOTIFY playbackStateChanged)
	Q_PROPERTY(qint64 position
	           READ position
	           NOTIFY positionChanged);
public:
	enum class State {
		Stopped = 0,
		Playing = 1,
		Paused  = 2,
	};

	explicit TrackingVideoPlayer(QObject * parent = nullptr);
	virtual ~TrackingVideoPlayer();

	void setVideoOutput(TrackingVideoWidget * widget);

	qreal playbackRate() const;

	State playbackState() const;

	qint64 position() const;

public slots:
	void pause();
	void play();
	void stop();
	void setMovieSegment(const fmp::MovieSegment::ConstPtr & segment,
	                     const fm::Time & start,
	                     const fm::Time & end);
	void setPlaybackRate(qreal rate);

	void setPosition(qint64 durationNS);

signals:
	void durationChanged(fm::Time start,qint64 durationNS);
	void positionChanged(qint64 nanoseconds);

	void playbackRateChanged(qreal rate);

	void playbackStateChanged(State state);

	void frameDone(QOpenGLBuffer * buffer);

	void displayVideoFrame(TrackingVideoFrame * frame);
private slots:
	void onNewVideoFrame(TrackingVideoFrame frame);

private:
	void stopAndWaitTask();

	TrackingVideoPlayerTask   * d_task;
	State                       d_state;
	fmp::MovieSegment::ConstPtr d_segment;
	qreal                       d_rate;
	TrackingVideoWidget       * d_widget;
	QThread                   * d_movieThread;

	std::map<qint64,TrackingVideoFrame> d_frames;
};


class QOpenGLContext;
class QOffscreenSurface;


class TrackingVideoPlayerTask : public QObject {
Q_OBJECT
public:
	explicit TrackingVideoPlayerTask(const QString & path,
	                                 QOpenGLContext * sharedContext);

	virtual ~TrackingVideoPlayerTask();

	quint64 numberFrame() const;

	void startOn(QThread * thread);

	void stop();
	void waitFinished();

	void setPosition(quint64 frameID);

signals:
	void newFrame(TrackingVideoFrame frame);
	void finished();

public slots:

	void onReleasedBuffer(QOpenGLBuffer *);


private slots:
	void start();
	void seek(quint64 pos);

private:
	cv::VideoCapture    d_capture;
	QOpenGLContext    * d_context;
	QOffscreenSurface * d_surface;
	std::condition_variable d_condition;
	std::mutex              d_mutex;
	bool                    d_done;
	std::atomic<bool>       d_stopped;
	std::vector<QOpenGLBuffer> d_PBOs;
	std::set<QOpenGLBuffer*>   d_waitingPBOs;
	int                        d_width,d_height;
};