#include "SpaceHandle.hpp"

#include <fort/myrmidon/priv/Space.hpp>
#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Zone.hpp>

#include "ZoneHandle.hpp"

namespace fort {
namespace myrmidon {

SpaceHandle::SpaceHandle(const priv::Space::Ptr & space,
                         const priv::Experiment::Ptr & experiment)
	: HandleInContext(space,experiment) {
	for (const auto & [zoneID,zone] : space->Zones() ) {
		d_zones.insert_or_assign(zoneID,MakeZonePtr(zone));
	}
}

const ZoneByID & SpaceHandle::Zones() const {
	return d_zones;
}


void SpaceHandle::DeleteZone(ZoneID zoneID) {
	d_object->DeleteZone(zoneID);
	d_zones.erase(zoneID);
}

Zone::Ptr SpaceHandle::CreateZone(const std::string & name) {
	auto zone = MakeZonePtr(d_object->CreateZone(name,0));
	d_zones.insert_or_assign(zone->ID(),zone);
	return d_zones.at(zone->ID());
}


Zone::Ptr SpaceHandle::MakeZonePtr(const priv::Zone::Ptr & zone) {
	return Zone::Ptr(new Zone(std::make_unique<ZoneHandle>(zone)));
}


} // namespace myrmidon
} // namespace fort
