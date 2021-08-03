#pragma once

#include "HandleInContext.hpp"

#include "SpaceHandle.hpp"
#include "AntHandle.hpp"

namespace fort {
namespace myrmidon {


class ExperimentHandle : public priv::Handle<priv::Experiment> {
public:
	ExperimentHandle(const priv::Experiment::Ptr & experiment)
		: Handle(experiment) {
		for ( const auto & [spaceID,space] : experiment->Spaces() ) {
			d_spaces.insert_or_assign(spaceID,MakeSpacePtr(space));
		}

		for ( const auto & [antID,ant] : experiment->Identifier()->Ants() ) {
			d_ants.insert_or_assign(antID,MakeAntPtr(ant));
		}
	}

	const SpaceByID & Spaces() const {
		return d_spaces;
	}

	Space::Ptr CreateSpace(const std::string & name) {
		auto space = MakeSpacePtr(Get().CreateSpace(name));
		d_spaces.insert_or_assign(space->ID(),space);
		return space;
	}


	void DeleteSpace(SpaceID spaceID) {
		Get().DeleteSpace(spaceID);
		d_spaces.erase(spaceID);
	}

	Ant::Ptr CreateAnt() {
		auto ant = MakeAntPtr(Get().CreateAnt());
		d_ants.insert_or_assign(ant->ID(),ant);
		return ant;
	}

	const AntByID & Ants() const {
		return d_ants;
	}

	void DeleteAnt(AntID antID) {
		Get().Identifier()->DeleteAnt(antID);
		d_ants.erase(antID);
	}

	Identification::Ptr AddIdentification(AntID antID,
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

	void DeleteIdentification(const Identification::Ptr & identification) {
		auto antID = identification->TargetAntID();
		Get().Identifier()->DeleteIdentification(identification->d_p->Ptr());
		d_ants.at(antID)->d_p->ReflectPrivateIdentifications();
	}

private:
	Space::Ptr MakeSpacePtr(const priv::Space::Ptr & space ) const {
		return Space::Ptr(new Space(std::make_unique<SpaceHandle>(space,d_object)));
	}

	Ant::Ptr MakeAntPtr(const priv::Ant::Ptr & ant) const {
		return Ant::Ptr(new Ant(std::make_unique<AntHandle>(ant,d_object)));
	}

	SpaceByID d_spaces;
	AntByID   d_ants;
};

} // namespace myrmidon
} // namespace fort
