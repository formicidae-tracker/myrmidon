#pragma once

#include <mutex>

#include <Eigen/Core>

#include <fort/tags/fort-tags.hpp>
#include <fort/tags/options.hpp>

#include <fort/myrmidon/types/ForwardDeclaration.hpp>

#include "FrameReference.hpp"
#include "Isometry2D.hpp"
#include "fort/myrmidon/types/CloseUp.hpp"
#include "fort/myrmidon/types/Typedefs.hpp"

typedef struct apriltag_detection apriltag_detection_t;

namespace fort {
namespace myrmidon {
namespace priv {

class TagCloseUp : public Identifiable, public FileSystemLocatable {
public:
	typedef std::shared_ptr<TagCloseUp>       Ptr;
	typedef std::shared_ptr<const TagCloseUp> ConstPtr;
	typedef std::vector<ConstPtr>             List;

	static std::string
	FormatURI(const std::string &tddURI, FrameID frameID, TagID tagID);

	TagCloseUp(
	    const fs::path                  &absoluteFilePath,
	    const FrameReference            &reference,
	    TagID                            tid,
	    const std::vector<TagDetection> &detections
	);

	virtual ~TagCloseUp();

	bool Valid() const {
		return d_target != d_detections.end();
	}

	const FrameReference &Frame() const;

	const std::string &URI() const override;

	const fs::path &AbsoluteFilePath() const override;

	TagID                              TagValue() const;
	const Eigen::Vector2d             &TagPosition() const;
	double                             TagAngle() const;
	const Eigen::Matrix<double, 2, 4> &Corners() const;

	const std::vector<TagDetection> &Detections() const;

	Isometry2Dd ImageToTag() const;

	double TagSizePx() const;

	double Squareness() const;

private:
	void check() const;
	void findTarget(TagID tid);

	FrameReference d_reference;
	std::string    d_URI;
	fs::path       d_absoluteFilePath;

	std::vector<TagDetection>                 d_detections;
	std::vector<TagDetection>::const_iterator d_target;
	double                                    d_tagWidthPx, d_squareness;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

// Formats a TagInFrameReference
// @out the std::ostream to format to
// @p the <fort::myrmidon::priv::TagInFrameReference> to format
// @return a reference to <out>
std::ostream &
operator<<(std::ostream &out, const fort::myrmidon::priv::TagCloseUp &tcu);

} // namespace priv
} // namespace myrmidon
} // namespace fort
