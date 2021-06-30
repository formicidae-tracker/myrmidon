#include "Zone.hpp"

#include <fort/myrmidon/Shapes.hpp>

#include "priv/Zone.hpp"
#include "priv/HandleInContext.hpp"

#include "utils/ConstClassHelper.hpp"

namespace fort {
namespace myrmidon {

class ZoneDefinitionHandle : public priv::HandleInContext<priv::ZoneDefinition,priv::Zone> {
public:
	ZoneDefinitionHandle(const std::shared_ptr<priv::ZoneDefinition> & zoneDefinition,
	                     const std::shared_ptr<priv::Zone> & zone)
		: HandleInContext(zoneDefinition,zone) {}
};

class ZoneHandle : public priv::Handle<priv::Zone> {
public:
	ZoneHandle(const std::shared_ptr<priv::Zone> & zone)
		: Handle(zone) {
		for(const auto & definition : zone->Definitions() ) {
			d_definitions.push_back(MakeDefinitionPtr(definition));
		}
	}

	const ZoneDefinition::Ptr & AddDefinition(const Shape::List & shapes,
	                                          const Time & start,
	                                          const Time & end) {
		auto definition = MakeDefinitionPtr(d_object->AddDefinition(shapes,start,end));
		d_definitions.push_back(definition);
		priv::TimeValid::SortAndCheckOverlap(d_definitions.begin(),d_definitions.end());
		return *std::find(d_definitions.begin(),d_definitions.end(),definition);
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
		return ZoneDefinition::Ptr(new ZoneDefinition(std::make_shared<ZoneDefinitionHandle>(definition,d_object)));
	}

	ZoneDefinitionList d_definitions;
};


Zone::Zone(const std::shared_ptr<ZoneHandle> & handle)
	: d_p(handle) {
}

ZoneDefinition::ZoneDefinition(const std::shared_ptr<ZoneDefinitionHandle> & handle)
	: d_p(handle) {
}

const Shape::List & ZoneDefinition::Shapes() const {
	return d_p->Get().Shapes();
}

void ZoneDefinition::SetShapes(const Shape::List & shapes) {
	d_p->Get().SetShapes(shapes);
}


const Time & ZoneDefinition::Start() const {
	return d_p->Get().Start();
}

const Time & ZoneDefinition::End() const {
	return d_p->Get().End();
}

void ZoneDefinition::SetStart(const Time & start) {
	d_p->Get().SetStart(start);
}

void ZoneDefinition::SetEnd(const Time & end) {
	d_p->Get().SetEnd(end);
}

const ZoneDefinition::Ptr & Zone::AddDefinition(const Shape::List & shapes,
                                                const Time & start,
                                                const Time & end) {
	return d_p->AddDefinition(shapes,start,end);
}

const ZoneDefinitionList & Zone::Definitions() const {
	return d_p->Definitions();
}

void Zone::DeleteDefinition(size_t index) {
	d_p->DeleteDefinition(index);
}

const std::string & Zone::Name() const {
	return d_p->Get().Name();
}

void Zone::SetName(const std::string & name) {
	d_p->Get().SetName(name);
}

ZoneID Zone::ID() const {
	return d_p->Get().ID();
}



} // namespace myrmidon
} // namespace fort
