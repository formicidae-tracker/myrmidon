#include "ConcurrentFrameLoader.hpp"

#include <QtConcurrent>

#include <fort/studio/Format.hpp>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/myrmidon/priv/MovieSegment.hpp>
#include <fort/myrmidon/priv/RawFrame.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/myrmidon/priv/CollisionSolver.hpp>

#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/studio/MyrmidonTypes/Experiment.hpp>


#ifdef NDEBUG
#define FORT_STUDIO_CONC_LOADER_NDEBUG 1
#endif
#ifndef FORT_STUDIO_CONC_LOADER_NDEBUG
#include <mutex>
std::mutex clDebugMutex;
#define CONC_LOADER_DEBUG(statements) do{	  \
		std::lock_guard<std::mutex> dlock(clDebugMutex); \
		statements; \
	}while(0)
#else
#define CONC_LOADER_DEBUG(statements)
#endif


ConcurrentFrameLoader::ConcurrentFrameLoader(QObject * parent)
	: QObject(parent)
	, d_done(0)
	, d_toDo(-1)
	, d_currentLoadingID(-1)
	, d_connectionType(Qt::QueuedConnection) {
	qRegisterMetaType<fmp::Experiment::ConstPtr>();
	qRegisterMetaType<fort::Time>();
}

ConcurrentFrameLoader::~ConcurrentFrameLoader() {
	if ( !d_abordFlag == false ) {
		d_abordFlag->store(true);
	}
}


bool ConcurrentFrameLoader::isDone() const {
	return d_done == d_toDo;
}

void ConcurrentFrameLoader::moveToThread(QThread * thread) {
	if ( thread != QObject::thread() ) {
		d_connectionType = Qt::BlockingQueuedConnection;
	}
	QObject::moveToThread(thread);
}


void ConcurrentFrameLoader::setExperiment(const fmp::Experiment::ConstPtr & experiment) {
	metaObject()->invokeMethod(this,"setExperimentUnsafe",d_connectionType,
	                           Q_ARG(fmp::Experiment::ConstPtr,experiment));
}

void ConcurrentFrameLoader::setExperimentUnsafe(fmp::Experiment::ConstPtr experiment) {
	if ( !d_experiment ) {
		clear();
	}
	d_experiment = experiment;
}

const fm::IdentifiedFrame::Ptr &
ConcurrentFrameLoader::frameAt(fmp::MovieFrameID movieID) const {
	static fm::IdentifiedFrame::Ptr empty;
	auto fi = d_frames.find(movieID+1);
	if ( fi == d_frames.cend() ) {
		return empty;
	}
	return fi->second;
}

const fm::CollisionFrame::Ptr &
ConcurrentFrameLoader::collisionAt(fmp::MovieFrameID movieID) const {
	static fm::CollisionFrame::Ptr empty;
	auto fi = d_collisions.find(movieID+1);
	if ( fi == d_collisions.cend() ) {
		return empty;
	}
	return fi->second;
}

void ConcurrentFrameLoader::loadMovieSegment(quint32 spaceID,
                                             const fmp::TrackingDataDirectory::Ptr & tdd,
                                             const fmp::MovieSegment::ConstPtr & segment,
                                             fort::Duration expectedFrameDuration) {
	if ( !d_experiment ) {
		return;
	}

	if ( fs::path(segment->URI()).parent_path().parent_path().generic_string() != tdd->URI() ) {
		qCritical() << "Cannot load frame from " << ToQString(segment->URI())
		            << " from TrackingDataDirectory " << ToQString(tdd->URI());
		return;
	}

	clear();
	d_expectedFrameDuration = expectedFrameDuration;
	auto identifier = fmp::Identifier::Compile(d_experiment->Identifier());
	auto solver = d_experiment->CompileCollisionSolver(false);

	size_t currentLoadingID = ++d_currentLoadingID;

	d_abordFlag = std::make_shared<std::atomic<bool>>();
	auto abordFlag = d_abordFlag;
	abordFlag->store(false);

	int maxThreadCount = QThreadPool::globalInstance()->maxThreadCount();
	if ( maxThreadCount < 2 ) {
		qWarning() << "Increases the work thread to at least 2 from " << maxThreadCount;
		maxThreadCount = 2;
		// avoids deadlock on the global instance !!!
		QThreadPool::globalInstance()->setMaxThreadCount(maxThreadCount);
	}
	CONC_LOADER_DEBUG({
			std::cerr << "Setting nbFrames to " << segment->EndFrame() - segment->StartFrame() + 1 << std::endl;
			std::cerr << "Segment:[" << segment->StartFrame() << ";" << segment->EndFrame() << "]" << std::endl;
			std::cerr << "TDD:[" << tdd->StartFrame() << ";" << tdd->EndFrame() << "]" << std::endl;
		});
	setProgress(0, segment->EndFrame() - segment->StartFrame() + 1);
	// even if we take one of the thread to populate tge other, we
	// make sure there could be one in the queue, but no more, to be
	// able to abord computation rapidly..
	auto sem = std::make_shared<QSemaphore>(maxThreadCount);
	// frames are single threaded read and loaded in memory, and we
	// spawn instance to compute them.
	auto load =
		[tdd,segment,identifier,solver,abordFlag,currentLoadingID,sem,spaceID,this]() {
			try {
				auto start = tdd->FrameAt(segment->StartFrame());
				auto lastFrame = segment->StartFrame() - 1;
				while( abordFlag->load() != true ) {
					if ( start == tdd->end() ) {
						break;
					}

					auto rawFrame = *start;
					// We may jump frame number if there is no data,
					// it may be the last frame on the MovieSegment
					// that is jumped.
					if ( !rawFrame  || rawFrame->Frame().FrameID() > segment->EndFrame() ) {
						CONC_LOADER_DEBUG(std::cerr << "marking " <<segment->EndFrame() - lastFrame << " done" << std::endl);
						//mark all jumped frame done
						this->metaObject()->invokeMethod(this,"addDone",Qt::QueuedConnection,
						                                 Q_ARG(int,segment->EndFrame() - lastFrame));
						break;
					}
					auto frameID = rawFrame->Frame().FrameID();
					//we mark all jumped frame done
					CONC_LOADER_DEBUG(std::cerr << "advanced " << frameID - lastFrame << std::endl);
					if ( (frameID - lastFrame) > 1 ) {
						this->metaObject()->invokeMethod(this,"addDone",Qt::QueuedConnection,
						                                 Q_ARG(int,frameID - lastFrame - 1));
					}
					lastFrame = frameID;

					auto loadFrame =
						[rawFrame,identifier,solver,spaceID,segment,frameID,this] () -> ConcurrentResult {
							CONC_LOADER_DEBUG(std::cerr << "Processing " << rawFrame->Frame().FID() << std::endl);
							try {
								auto movieID = segment->ToMovieFrameID(frameID);
								// TODO optimize memory allocation here
								auto identified = std::make_shared<fm::IdentifiedFrame>();
								rawFrame->IdentifyFrom(*identified,*identifier,spaceID);
								// TODO optimize memory allocation here
								auto collisions = std::make_shared<fm::CollisionFrame>();
								solver->ComputeCollisions(*collisions,*identified);
								return std::make_tuple(movieID,
								                       identified,
								                       collisions);
							} catch( const std::exception & ) {
								return std::make_tuple(segment->EndMovieFrame()+1,
								                       fm::IdentifiedFrame::Ptr(),
								                       fm::CollisionFrame::Ptr());
							}
						};
					CONC_LOADER_DEBUG(std::cerr << "Spawning " << frameID << std::endl);
					sem->acquire(1);
					QFuture<ConcurrentResult> future = QtConcurrent::run(loadFrame);
					auto watcher = new QFutureWatcher<ConcurrentResult>();
					watcher->moveToThread(this->thread());
					connect(watcher,
					        &QFutureWatcher<ConcurrentResult>::finished,
					        this,
					        [watcher,currentLoadingID,segment,sem,frameID,this]() {
						        CONC_LOADER_DEBUG({
								        std::cerr << "Received " << frameID << " status " << d_done << "/" << d_toDo << std::endl;
								        std::cerr << "Wanted Thread:" << this->thread() << " current: " << QThread::currentThread() <<  std::endl;
							        });
						        auto res = watcher->result();
						        watcher->deleteLater();
						        sem->release(1);

						        if ( currentLoadingID != this->d_currentLoadingID ) {
							        CONC_LOADER_DEBUG(std::cerr << "Unexpected loadingID " << currentLoadingID << " (expected:" << this->d_currentLoadingID << std::endl);
							        // outdated computation, we ignore it
							        return;
						        }

						        addDone(1);

						        if ( std::get<0>(res) == segment->EndMovieFrame()+1 ) {
							        // no result for that computation
							        return;
						        }

						        d_frames.insert(std::make_pair(std::get<0>(res)+1,std::get<1>(res)));
						        d_collisions.insert(std::make_pair(std::get<0>(res)+1,std::get<2>(res)));
					        },
					        Qt::QueuedConnection);
					watcher->setFuture(future);
					++start;
				}
			} catch ( const std::exception & e) {
				qCritical() << "Could not extract tracking data for "
				            << ToQString(segment->URI())
				            << ": " << e.what();
				setProgress(d_toDo,d_toDo);
				return;
			}
		};

	QtConcurrent::run(QThreadPool::globalInstance(),load);
}

void ConcurrentFrameLoader::clear() {
	abordCurrent();
	d_frames.clear();
	d_collisions.clear();
}


void ConcurrentFrameLoader::abordCurrent() {
	if ( !d_abordFlag ) {
		return;
	}
	d_abordFlag->store(true);
	d_abordFlag.reset();
}

void ConcurrentFrameLoader::setProgress(int doneValue,int toDo) {
	CONC_LOADER_DEBUG({
			std::cerr << "[setProgress]: wantedThread: " << this->thread() << " current: " << QThread::currentThread() <<  std::endl;
			std::cerr << "[setProgress]: current:" << d_done << "/" << d_toDo << " wants:" << doneValue << "/" << toDo << std::endl;
		});

	if ( d_done == doneValue && d_toDo == toDo ) {
		return;
	}
	bool doneState = isDone();
	d_done = doneValue;
	d_toDo = toDo;
	emit progressChanged(d_done,d_toDo);
	if ( doneState != isDone() ) {
		emit done(isDone());
	}
}


void ConcurrentFrameLoader::addDone(int done) {
	setProgress(d_done + done,d_toDo);
}

fort::Duration
ConcurrentFrameLoader::findAnt(quint32 antID,
                               quint64 frameID,
                               int direction) {
	auto fi = d_frames.find(frameID + 1);
	if ( d_done == false || fi == d_frames.end() ) {
		return -1;
	}
	const auto & start = d_frames.at(1)->FrameTime;

	if ( direction > 0 ) {
		for ( ; fi != d_frames.end(); ++fi ) {
			if( fi->second->Contains(antID) == true ) {
				return fi->second->FrameTime.Sub(start);
			}
		}
		return -1;
	}


	for ( ; fi != d_frames.begin(); --fi) {
		if( fi->second->Contains(antID) == true ) {
			return fi->second->FrameTime.Sub(start);
		}
	}
	if ( fi->second->Contains(antID) == true ) {
		return fi->second->FrameTime.Sub(start);
	}
	return -1;
}

fort::Duration ConcurrentFrameLoader::duration() const {
	if ( d_frames.empty() ) {
		return 0;
	}
	return (--d_frames.end())->second->FrameTime.Sub(d_frames.begin()->second->FrameTime);
}


fort::Duration
ConcurrentFrameLoader::moviePositionAt(const FramesByMovieID & frames,
                                       fort::Duration expectedFrameDuration,
                                       fmp::MovieFrameID movieID) {
	if ( frames.empty() ) {
		return movieID * expectedFrameDuration;
	}
	// we get or interpolate the start time of the segment.
	auto begin = frames.cbegin();
	auto start = begin->second->FrameTime.Add((1 - int(begin->first)) * expectedFrameDuration );

	auto lower = frames.lower_key(movieID+1);
	if ( lower == frames.cend() ) {
		auto upper = frames.upper_key(movieID+1);
		// interpolation from the closest;
		return upper->second->FrameTime.Sub(start) +  ( movieID - upper->first + 1) * expectedFrameDuration;
	}

	if ( lower->first == (movieID + 1) ) {
		// found the exact frame
		return lower->second->FrameTime.Sub(start);
	}
	auto upper = frames.upper_key(movieID+1);

	if ( upper == frames.cend() ) {
		// interpolation from the closest;
		return lower->second->FrameTime.Sub(start) +  ( movieID - lower->first + 1 ) * expectedFrameDuration;
	}
	// interpolate between the two frames
	fort::Duration estimatedFrameDuration = upper->second->FrameTime.Sub(lower->second->FrameTime).Nanoseconds() / ( upper->first - lower->first );
	return lower->second->FrameTime.Sub(start) + ( movieID - lower->first + 1 ) * estimatedFrameDuration;
}

fort::Duration ConcurrentFrameLoader::positionAt(fmp::MovieFrameID movieID) const {
	return moviePositionAt(d_frames,d_expectedFrameDuration,movieID);
}
