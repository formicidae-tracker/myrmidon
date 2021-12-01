#pragma once

#include <QObject>
#include <QHash>

#include <vector>
#include <atomic>
#include <tuple>

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>
#include <fort/studio/MyrmidonTypes/MovieSegment.hpp>
#include <fort/studio/MyrmidonTypes/Experiment.hpp>

#include <fort/myrmidon/types/IdentifiedFrame.hpp>
#include <fort/myrmidon/types/Collision.hpp>

namespace fmp = fort::myrmidon::priv;

class ConcurrentFrameLoader : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool done
	           READ isDone
	           NOTIFY done)
public:
	explicit ConcurrentFrameLoader(QObject * parent = nullptr);
	virtual ~ConcurrentFrameLoader();

	bool isDone() const;

	void setExperiment(const fmp::Experiment::ConstPtr & experiment);

	fmp::MovieFrameID frameIDAt(fort::Duration position) const;
	fort::Duration positionAt(fmp::MovieFrameID movieID) const;
	fort::Duration duration() const;
	const fm::IdentifiedFrame::Ptr & frameAt(fmp::MovieFrameID movieID) const;
	const fm::CollisionFrame::Ptr & collisionAt(fmp::MovieFrameID movieID) const;


	void moveToThread(QThread * thread);

	static fort::Duration
	moviePositionAt(const fmp::DenseMap<fmp::MovieFrameID,fm::IdentifiedFrame::Ptr> & frames,
	                fort::Duration expectedFrameDuration,
	                fmp::MovieFrameID movieID);

	static fmp::MovieFrameID
	frameIDAt(const fmp::DenseMap<fmp::MovieFrameID,fm::IdentifiedFrame::Ptr> & frames,
	          fort::Duration expectedFrameDuration,
	          fort::Duration position);

public slots:
	void loadMovieSegment(quint32 spaceID,
	                      const fmp::TrackingDataDirectoryPtr & tdd,
	                      const fmp::MovieSegmentConstPtr & segment,
	                      fort::Duration expectedFrameDuration);
	void clear();

	fort::Duration findAnt(quint32 antID,
	                       quint64 frameID,
	                       int direction);
signals:
	void progressChanged(int done,int toDo);
	void done(bool);
	void durationComputed(fort::Duration duration);

private slots:
	void setExperimentUnsafe(fmp::Experiment::ConstPtr experiment);
	void addDone(int done);

private :
	void abordCurrent();

	void setProgress(int done,int toDo);


	typedef fmp::DenseMap<fmp::MovieFrameID,fm::IdentifiedFrame::Ptr>  FramesByMovieID;
	typedef fmp::DenseMap<fmp::MovieFrameID,fm::CollisionFrame::Ptr> CollisionsByMovieID;
	typedef std::tuple<fmp::MovieFrameID,fm::IdentifiedFrame::Ptr,fm::CollisionFrame::Ptr> ConcurrentResult;

	std::shared_ptr<const fmp::Experiment> d_experiment;
	FramesByMovieID         d_frames;
	CollisionsByMovieID     d_collisions;
	int                     d_done,d_toDo;
	fort::Duration          d_expectedFrameDuration;


	std::shared_ptr<std::atomic<bool>> d_abordFlag;
	size_t                             d_currentLoadingID;
	Qt::ConnectionType                 d_connectionType;
};
