#include "ZoneHandle.hpp"

#include <fort/myrmidon/Zone.hpp>

#include <fort/myrmidon/priv/Zone.hpp>

#include "ZoneDefinitionHandle.hpp"


namespace fort {
namespace myrmidon {

ZoneHandle::ZoneHandle(const priv::Zone::Ptr & zone)
	: Handle(zone) {
	for(const auto & definition : zone->Definitions() ) {
		d_definitions.push_back(MakeDefinitionPtr(definition));
	}
}

ZoneDefinition::Ptr ZoneHandle::AddDefinition(const Shape::List & shapes,
                                              const Time & start,
                                              const Time & end) {
	auto definition = MakeDefinitionPtr(d_object->AddDefinition(shapes,start,end));
	d_definitions.push_back(definition);
	priv::TimeValid::SortAndCheckOverlap(d_definitions.begin(),d_definitions.end());
	return definition;
}

void ZoneHandle::DeleteDefinition(size_t index) {
	d_object->EraseDefinition(index);
	d_definitions.erase(d_definitions.begin()+index);
}

const ZoneDefinitionList & ZoneHandle::Definitions() const {
	return d_definitions;
}


ZoneDefinition::Ptr ZoneHandle::MakeDefinitionPtr(const priv::ZoneDefinition::Ptr & definition) {
	return ZoneDefinition::Ptr(new ZoneDefinition(std::make_unique<ZoneDefinitionHandle>(definition,d_object)));
}


} // namespace myrmidon
} // namespace fort
