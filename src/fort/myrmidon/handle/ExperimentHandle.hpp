#pragma once

#include "HandleInContext.hpp"

#include "SpaceHandle.hpp"

namespace fort {
namespace myrmidon {


class ExperimentHandle : public priv::Handle<priv::Experiment> {
public:
	ExperimentHandle(const priv::Experiment::Ptr & experiment)
		: Handle(experiment) {
		for ( const auto & [spaceID,space] : experiment->Spaces() ) {
			d_spaces.insert_or_assign(spaceID,MakeSpacePtr(space));
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


private:
	Space::Ptr MakeSpacePtr(const priv::Space::Ptr & space ) const {
		return Space::Ptr(new Space(std::make_unique<SpaceHandle>(space,d_object)));
	}

	SpaceByID d_spaces;
};

} // namespace myrmidon
} // namespace fort
