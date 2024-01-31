#include "TrackingVideoFrame.hpp"
#include <fort/video/Types.hpp>
#include <iterator>
#include <libavutil/pixfmt.h>
#include <qimage.h>

TrackingVideoFrame::TrackingVideoFrame()
    : FrameID(std::numeric_limits<fmp::MovieFrameID>::max()) {}

std::ostream &operator<<(std::ostream &out, const TrackingVideoFrame &f) {
	return out << "{ID:" << f.FrameID << ",range:" << f.StartPos << "-"
	           << f.EndPos << ",data:" << f.Data.get() << "}";
}

#ifndef FORT_STUDIO_VIDEO_PLAYER_NDEBUG
std::mutex TrackingVideoFrame::debugMutex;
#endif

bool TrackingVideoFrame::Contains(quint32 antID) const {
	if (!TrackingFrame == true) {
		return false;
	}
	return TrackingFrame->Contains(antID);
}

QImage TrackingVideoFrame::AsQImage() const noexcept {
	static std::map<fort::video::PixelFormat, QImage::Format> supported = {
	    {AV_PIX_FMT_BGRA, QImage::Format_RGB32},
	    {AV_PIX_FMT_ARGB, QImage::Format_RGB32},
	    {AV_PIX_FMT_RGB24, QImage::Format_RGB888},
	};

	if (supported.count(Data->Format) == 0) {
		return QImage();
	}

	return QImage(
	    Data->Planes[0],
	    Data->Size.Width,
	    Data->Size.Height,
	    Data->Linesize[0],
	    supported[Data->Format]
	);
}
