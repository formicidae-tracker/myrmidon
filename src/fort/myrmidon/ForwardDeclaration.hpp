#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <map>

#include "Typedefs.hpp"

namespace fort {
namespace myrmidon {


class Capsule;
// Forward declaration for <Capsule::Ptr>
typedef std::shared_ptr<Capsule> CapsulePtr;
// Forward declaration for <Capsule::ConstPtr>
typedef std::shared_ptr<const Capsule> CapsuleConstPtr;

class Circle;
// Forward declaration for <Circle::Ptr>
typedef std::shared_ptr<Circle> CirclePtr;
// Forward declaration for <Circle::ConstPtr>
typedef std::shared_ptr<const Circle> CircleConstPtr;

class Polygon;
// Forward declaration for <Polygon::Ptr>
typedef std::shared_ptr<Polygon> PolygonPtr;
// Forward declaration for <Polygon::ConstPtr>
typedef std::shared_ptr<const Polygon> PolygonConstPtr;

class ZoneDefinition;
/**
 * A List of ZoneDefinition
 */
typedef std::vector<std::shared_ptr<ZoneDefinition>> ZoneDefinitionList;

class Zone;
typedef std::map<ZoneID,std::shared_ptr<Zone>> ZoneByID;

class Space;
typedef std::map<SpaceID,std::shared_ptr<Space>> SpaceByID;

class Ant;
typedef std::map<AntID,std::shared_ptr<Ant>> AntByID;

class Identification;
typedef std::vector<std::shared_ptr<Identification>> IdentificationList;

/** A list of Ant virtual shape part
 *
 */
typedef std::vector<std::pair<AntShapeTypeID,std::shared_ptr<Capsule>>> TypedCapsuleList;




} // namespace myrmidon
} // namespace fort
