#pragma once

#include "HandleInContext.hpp"

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/ForwardDeclaration.hpp>
#include <fort/myrmidon/Shapes.hpp>

namespace fort {
namespace myrmidon {
class ZoneDefinition;
namespace priv {
class Zone;
class ZoneDefinition;
} // namespace priv

class ZoneHandle : public priv::Handle<priv::Zone> {
public:
	ZoneHandle(const std::shared_ptr<priv::Zone> & zone);
	std::shared_ptr<ZoneDefinition> AddDefinition(const Shape::List & shapes,
	                                              const Time & start,
	                                              const Time & end);

	void DeleteDefinition(size_t index);
	const ZoneDefinitionList & Definitions() const;
private:
	std::shared_ptr<ZoneDefinition> MakeDefinitionPtr(const std::shared_ptr<priv::ZoneDefinition> & definition);

	ZoneDefinitionList d_definitions;
};

} // namespace myrmidon
} // namespace fort
