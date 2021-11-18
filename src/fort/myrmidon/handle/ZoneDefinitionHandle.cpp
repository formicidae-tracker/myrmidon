#include "ZoneDefinitionHandle.hpp"


namespace fort {
namespace myrmidon {

ZoneDefinitionHandle::ZoneDefinitionHandle(const std::shared_ptr<priv::ZoneDefinition> & zoneDefinition,
                                           const std::shared_ptr<priv::Zone> & zone)
	: HandleInContext(zoneDefinition,zone) {}

} // namespace myrmidon
} // namespace fort
