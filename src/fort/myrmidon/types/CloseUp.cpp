#include "CloseUp.hpp"
#include <Eigen/Core>

#include <apriltag/apriltag.h>

namespace fort {
namespace myrmidon {
double TagDetection::SizePx() const {
	double res = (Corners.col(0) - Corners.col(1)).norm() +
	             (Corners.col(1) - Corners.col(2)).norm() +
	             (Corners.col(2) - Corners.col(3)).norm() +
	             (Corners.col(3) - Corners.col(0)).norm();

	return res / 4.0;
}

double TagDetection::Squareness() const {
	double maxAngleDistanceToPI_2 = 0.0;
	for (size_t i = 0; i < 4; ++i) {
		Eigen::Vector2d a     = Corners.col((i - 1) % 4) - Corners.col(i);
		Eigen::Vector2d b     = Corners.col((i + 1) % 4) - Corners.col(i);
		double          aNorm = a.norm();
		double          bNorm = b.norm();
		if (aNorm < 1.0e-3 || bNorm < 1.0e-3) {
			return 0;
		}
		double angle = std::acos(a.dot(b) / (aNorm * bNorm));
		maxAngleDistanceToPI_2 =
		    std::max(maxAngleDistanceToPI_2, std::abs(angle - (M_PI / 2.0)));
	}
	return 1.0 - maxAngleDistanceToPI_2 / M_PI * 2.0;
}

double
TagDetection::ComputeAngleFromCorners(const Eigen::Matrix<double, 2, 4> &corners
) {
	Eigen::Vector2d delta = (corners.col(1) + corners.col(2)) / 2.0 -
	                        (corners.col(0) + corners.col(3)) / 2.0;
	return atan2(delta.y(), delta.x());
}

TagDetection TagDetection::Convert(const apriltag_detection_t *d) {
	Eigen::Matrix<double, 2, 4> corners;
	for (size_t i = 0; i < 4; ++i) {
		corners.col(i) = Eigen::Vector2d{d->p[i][0], d->p[i][1]};
	}
	double angle = ComputeAngleFromCorners(corners);
	return TagDetection{
	    .ID       = static_cast<uint32_t>(d->id),
	    .Position = Eigen::Vector2d{d->c[0], d->c[1]},
	    .Angle    = angle,
	    .Corners  = std::move(corners),
	};
}

} // namespace myrmidon
} // namespace fort
