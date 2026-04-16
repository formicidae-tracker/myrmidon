#pragma once

#include <string>
#include <vector>

#include <Eigen/Core>

typedef struct apriltag_detection apriltag_detection_t;

namespace fort {
namespace myrmidon {

struct TagDetection {
	uint32_t                    ID;
	Eigen::Vector2d             Position;
	double                      Angle;
	Eigen::Matrix<double, 2, 4> Corners;

	double SizePx() const;
	double Squareness() const;

	static TagDetection Convert(const apriltag_detection_t *);

	static double
	ComputeAngleFromCorners(const Eigen::Matrix<double, 2, 4> &corners);
};

struct CloseUp {
	std::string AbsoluteFilePath;

	std::vector<TagDetection> Detections;
};
} // namespace myrmidon
} // namespace fort
