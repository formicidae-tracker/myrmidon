#pragma once

#include "HandleInContext.hpp"

namespace fort {
namespace myrmidon {

class ZoneDefinitionHandle : public priv::HandleInContext<priv::ZoneDefinition,priv::Zone> {
public:
	ZoneDefinitionHandle(const std::shared_ptr<priv::ZoneDefinition> & zoneDefinition,
	                     const std::shared_ptr<priv::Zone> & zone)
		: HandleInContext(zoneDefinition,zone) {}
};

} // namespace myrmidon
} // namespace fort
