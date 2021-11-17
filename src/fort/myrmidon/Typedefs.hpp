#pragma once

#include <variant>
#include <map>
#include <vector>
#include <memory>

#include <fort/time/Time.hpp>
#include <Eigen/Geometry>

namespace fort {
namespace myrmidon {


/**
 * The ID for a tag
 *
 * The identifier for a tag, which relates to Ant using
 * Identification.
 */
typedef uint32_t TagID;


/**
 * The ID for an Ant.
 *
 * Ant are uniquely identified within an Experiment with an AntID,
 * which is at least `1`. `0` is an invalid AntID.
 */
typedef uint32_t AntID;

/**
 * The ID for a Space.
 *
 * Space are uniquely identified within an Experiment with a SpaceID,
 * which is at least `1`. `0` is an invalid SpaceID.
 */
typedef uint32_t SpaceID;

/**
 * The ID for a Zone.
 *
 * Zone are uniquely identified within an Experiment with a ZoneID, which is
 * at least `1`. `0` is an invalid/undefined Zone.
 */
typedef uint32_t ZoneID;

/**
 * C++ type for named values.
 *
 * A c++ type that can hold only one of any #AntMetaDataType.
 */
typedef std::variant<bool,int32_t,double,std::string,Time> AntStaticValue;

/**
 * A List of 2D Vector.
 *
 */
typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d>> Vector2dList;


/**
 * The ID for Ant virtual body parts
 *
 * Uniquely identifies an Ant shape type in an Experiment, from
 * `1`. `0` is an invalid value.
 */
typedef uint32_t AntShapeTypeID;


/**
 * The ID for Ant manual measurement types
 *
 * Uniquely identifies an Ant measurement type in an Experiment, from
 * `1`. `0` is an invalid value. The value `1` always refers to the
 * valid MeasurementTypeID #HEAD_TAIL_MEASUREMENT_TYPE.
 */
typedef uint32_t MeasurementTypeID;

typedef Eigen::AlignedBox<double,2> AABB;



} // namespace myrmidon
} // namespace fort
