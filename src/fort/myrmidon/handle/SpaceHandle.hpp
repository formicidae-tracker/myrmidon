#pragma once

#include "HandleInContext.hpp"

#include <fort/myrmidon/types/ForwardDeclaration.hpp>

namespace fort {
namespace myrmidon {

class Zone;

namespace priv {
class Space;
class Zone;
class Experiment;
}

class SpaceHandle : public priv::HandleInContext<priv::Space,priv::Experiment> {
public :
	SpaceHandle(const std::shared_ptr<priv::Space> & space,
	            const std::shared_ptr<priv::Experiment> & experiment);

	const ZoneByID & Zones() const;

	void DeleteZone(ZoneID zoneID);

	std::shared_ptr<Zone> CreateZone(const std::string & name);
private:
	std::shared_ptr<Zone> MakeZonePtr(const std::shared_ptr<priv::Zone> & zone);

	ZoneByID d_zones;
};

} // namespace myrmidon
} // namespace fort
