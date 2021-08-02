#pragma once

#include "HandleInContext.hpp"

#include "ZoneDefinitionHandle.hpp"

namespace fort {
namespace myrmidon {

class ZoneHandle : public priv::Handle<priv::Zone> {
public:
	ZoneHandle(const priv::Zone::Ptr & zone)
		: Handle(zone) {
		for(const auto & definition : zone->Definitions() ) {
			d_definitions.push_back(MakeDefinitionPtr(definition));
		}
	}

	ZoneDefinition::Ptr AddDefinition(const Shape::List & shapes,
	                                  const Time & start,
	                                  const Time & end) {
		auto definition = MakeDefinitionPtr(d_object->AddDefinition(shapes,start,end));
		d_definitions.push_back(definition);
		priv::TimeValid::SortAndCheckOverlap(d_definitions.begin(),d_definitions.end());
		return definition;
	}

	void DeleteDefinition(size_t index) {
		d_object->EraseDefinition(index);
		d_definitions.erase(d_definitions.begin()+index);
	}

	const ZoneDefinitionList & Definitions() const {
		return d_definitions;
	}

private:
	ZoneDefinition::Ptr MakeDefinitionPtr(const priv::ZoneDefinition::Ptr & definition) {
		return ZoneDefinition::Ptr(new ZoneDefinition(std::make_unique<ZoneDefinitionHandle>(definition,d_object)));
	}

	ZoneDefinitionList d_definitions;
};

} // namespace myrmidon
} // namespace fort
