#pragma once

#if defined(NDEBUG)
#define FORT_STUDIO_VIDEO_PLAYER_NDEBUG 1
#endif

#include <fort/time/Time.hpp>
#include <fort/video/Frame.hpp>

#include <fort/myrmidon/priv/Typedefs.hpp>
#include <fort/myrmidon/types/Collision.hpp>
#include <fort/myrmidon/types/IdentifiedFrame.hpp>

#include <QImage>
#include <QObject>

#include <iostream>
#include <memory>

namespace fmp = fort::myrmidon::priv;
namespace fm  = fort::myrmidon;

#ifndef FORT_STUDIO_VIDEO_PLAYER_NDEBUG
#include <mutex>
#define VIDEO_PLAYER_DEBUG(statements)                                         \
	do {                                                                       \
		std::lock_guard<std::mutex> debugLock(TrackingVideoFrame::debugMutex); \
		statements;                                                            \
	} while (0)
#else
#define VIDEO_PLAYER_DEBUG(statements)
#endif

struct TrackingVideoFrame {
	TrackingVideoFrame();
	std::shared_ptr<fort::video::Frame> Data;
	fort::Duration                      StartPos, EndPos;
	fmp::MovieFrameID                   FrameID;
	fm::IdentifiedFrame::Ptr            TrackingFrame;
	fm::CollisionFrame::Ptr             CollisionFrame;
#ifndef FORT_STUDIO_VIDEO_PLAYER_NDEBUG
	static std::mutex debugMutex;
#endif
	bool Contains(quint32 antID) const;

	QImage AsQImage() const noexcept;
};

Q_DECLARE_METATYPE(TrackingVideoFrame);

std::ostream & operator<<(std::ostream & out, const TrackingVideoFrame & f);
