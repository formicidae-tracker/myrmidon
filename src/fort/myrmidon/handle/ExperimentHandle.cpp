#include "ExperimentHandle.hpp"

#include <fort/myrmidon/Space.hpp>
#include <fort/myrmidon/Identification.hpp>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/myrmidon/priv/Space.hpp>
#include <fort/myrmidon/priv/Ant.hpp>

#include "AntHandle.hpp"
#include "SpaceHandle.hpp"
#include "ZoneHandle.hpp"
#include "IdentificationHandle.hpp"

namespace fort {
namespace myrmidon {

ExperimentHandle::ExperimentHandle(const priv::Experiment::Ptr & experiment)
	: Handle(experiment) {
	for ( const auto & [spaceID,space] : experiment->Spaces() ) {
		d_spaces.insert_or_assign(spaceID,MakeSpacePtr(space));
	}

	for ( const auto & [antID,ant] : experiment->Identifier()->Ants() ) {
		d_ants.insert_or_assign(antID,MakeAntPtr(ant));
	}
}

const SpaceByID & ExperimentHandle::Spaces() const {
	return d_spaces;
}

Space::Ptr ExperimentHandle::CreateSpace(const std::string & name) {
	auto space = MakeSpacePtr(Get().CreateSpace(name));
	d_spaces.insert_or_assign(space->ID(),space);
	return space;
}

void ExperimentHandle::DeleteSpace(SpaceID spaceID) {
	Get().DeleteSpace(spaceID);
	d_spaces.erase(spaceID);
}

Ant::Ptr ExperimentHandle::CreateAnt() {
	auto ant = MakeAntPtr(Get().CreateAnt());
	d_ants.insert_or_assign(ant->ID(),ant);
	return ant;
}

const AntByID & ExperimentHandle::Ants() const {
	return d_ants;
}

void ExperimentHandle::DeleteAnt(AntID antID) {
	Get().Identifier()->DeleteAnt(antID);
	d_ants.erase(antID);
}


Identification::Ptr ExperimentHandle::AddIdentification(AntID antID,
                                                        TagID tagID,
                                                        const Time & start,
                                                        const Time & end) {
	auto privateIdentification = priv::Identifier::AddIdentification(Get().Identifier(),
	                                                                 antID,
	                                                                 tagID,
	                                                                 start,
	                                                                 end);
	auto ant = d_ants.at(antID);
	ant->d_p->ReflectPrivateIdentifications();
	return *std::find_if(ant->Identifications().begin(),
	                     ant->Identifications().end(),
	                     [&privateIdentification](const Identification::Ptr & i) {
		                     return i->d_p->Ptr() == privateIdentification;
	                     });
}


void ExperimentHandle::DeleteIdentification(const Identification::Ptr & identification) {
	auto antID = identification->TargetAntID();
	Get().Identifier()->DeleteIdentification(identification->d_p->Ptr());
	d_ants.at(antID)->d_p->ReflectPrivateIdentifications();
}

Space::Ptr ExperimentHandle::MakeSpacePtr(const priv::Space::Ptr & space ) const {
	return Space::Ptr(new Space(std::make_unique<SpaceHandle>(space,d_object)));
}


Ant::Ptr ExperimentHandle::MakeAntPtr(const priv::Ant::Ptr & ant) const {
	return Ant::Ptr(new Ant(std::make_unique<AntHandle>(ant,d_object)));
}


} // namespace myrmidon
} // namespace fort
