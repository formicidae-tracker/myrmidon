#pragma once

#include <vector>

#include <fort/time/Time.hpp>

namespace fort {
namespace myrmidon {

/**
 * Represents a Measurement in millimeters at a given Time.
 *
 * Measurement in myrmidon are automatically converted to MM given the
 * Experiment tag family and size, and the size of the tag measured
 * in the image.
 */
struct ComputedMeasurement {
	/**
	 * A list of measurement
	 */
	typedef std::vector<ComputedMeasurement> List;
	/**
	 * the Time of the Measurement
	 */
	fort::Time Time;
	/**
	 * the value in mm of the measurement
	 */
	double     LengthMM;
	/**
	 * the value of the measurement in pixels
	 */
	double     LengthPixel;
};

} // namespace myrmidon
} // namespace fort
