#include "TagCloseUp.hpp"
#include "fort/myrmidon/types/CloseUp.hpp"

#include <Eigen/src/Core/Matrix.h>
#include <limits>
#include <regex>

#include <apriltag/tag16h5.h>
#include <apriltag/tag25h9.h>
#include <apriltag/tag36h11.h>
#include <apriltag/tagCircle21h7.h>
#include <apriltag/tagCircle49h12.h>
#include <apriltag/tagCustom48h12.h>
#include <apriltag/tagStandard41h12.h>
#include <apriltag/tagStandard52h13.h>
#include <fort/tags/tag36ARTag.h>
#include <fort/tags/tag36h10.h>

#include <fort/myrmidon/TagCloseUpCache.pb.h>
#include <fort/myrmidon/utils/Checker.hpp>

#include <fort/myrmidon/utils/Defer.hpp>

#include <iostream>
#include <stdexcept>

namespace fort {
namespace myrmidon {
namespace priv {

std::string
TagCloseUp::FormatURI(const std::string &tddURI, FrameID frameID, TagID tagID) {
	return (fs::path(tddURI) / "frames" / std::to_string(frameID) / "closeups" /
	        FormatTagID(tagID))
	    .generic_string();
}

TagCloseUp::TagCloseUp(
    const fs::path                  &absoluteFilePath,
    const FrameReference            &reference,
    TagID                            tagID,
    const std::vector<TagDetection> &detections
)
    : d_reference(reference)
    , d_URI(FormatURI(reference.ParentURI(), reference.FrameID(), tagID))
    , d_absoluteFilePath(absoluteFilePath)
    , d_detections(detections) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);
	findTarget(tagID);
}

TagCloseUp::~TagCloseUp() {}

const FrameReference &TagCloseUp::Frame() const {
	return d_reference;
}

const std::string &TagCloseUp::URI() const {
	return d_URI;
}

const fs::path &TagCloseUp::AbsoluteFilePath() const {
	return d_absoluteFilePath;
}

TagID TagCloseUp::TagValue() const {
	if (d_target == d_detections.end()) {
		return std::numeric_limits<TagID>::max();
	}
	return d_target->ID;
}

const Eigen::Vector2d &TagCloseUp::TagPosition() const {
	check();
	return d_target->Position;
}

double TagCloseUp::TagAngle() const {
	check();
	return d_target->Angle;
}

const Eigen::Matrix<double, 2, 4> &TagCloseUp::Corners() const {
	check();
	return d_target->Corners;
}

double TagCloseUp::TagSizePx() const {
	check();
	return d_target->SizePx();
}

double TagCloseUp::Squareness() const {
	check();
	return d_target->Squareness();
}

const std::vector<TagDetection> &TagCloseUp::Detections() const {
	return d_detections;
}

Isometry2Dd TagCloseUp::ImageToTag() const {
	return Isometry2Dd(TagAngle(), TagPosition()).inverse();
}

void TagCloseUp::findTarget(TagID tid) {
	d_target = d_detections.end();
	for (auto d = d_detections.begin(); d != d_detections.end(); ++d) {
		if (d->ID == tid) {
			d_target     = d;
			d_tagWidthPx = (d->Corners.col(0) - d->Corners.col(1)).norm();
			d_squareness = d->Squareness();
			return;
		}
	}
}

void TagCloseUp::check() const {
	if (d_target == d_detections.end()) {
		throw std::runtime_error("Target ID not found in CloseUp");
	}
}

std::ostream &
operator<<(std::ostream &out, const fort::myrmidon::priv::TagCloseUp &p) {
	return out << p.Frame() << "/closeups/"
	           << fort::myrmidon::FormatTagID(p.TagValue());
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
