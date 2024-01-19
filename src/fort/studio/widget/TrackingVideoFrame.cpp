#include "TrackingVideoFrame.hpp"
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
	if (!Data || Data->Format != AV_PIX_FMT_ARGB) {
		return QImage();
	}

	return QImage(
	    Data->Planes[0],
	    Data->Size.Width,
	    Data->Size.Height,
	    Data->Linesize[0],
	    QImage::Format_RGB32
	);
}
