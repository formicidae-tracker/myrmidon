#pragma once

#include "HandleInContext.hpp"

#include <fort/time/Time.hpp>

#include <fort/myrmidon/types/ForwardDeclaration.hpp>
#include <fort/myrmidon/types/Typedefs.hpp>

namespace fort {
namespace myrmidon {

namespace priv {
class Experiment;
class Space;
class Ant;
} // namespace priv

class ExperimentHandle : public priv::Handle<priv::Experiment> {
public:
	ExperimentHandle(const std::shared_ptr<priv::Experiment> & experiment);

	const SpaceByID & Spaces() const;

	std::shared_ptr<Space> CreateSpace(const std::string & name);

	void DeleteSpace(SpaceID spaceID);

	std::shared_ptr<Ant> CreateAnt();

	const AntByID & Ants() const;

	void DeleteAnt(AntID antID);

	std::shared_ptr<Identification>
	AddIdentification(AntID antID,
	                  TagID tagID,
	                  const Time & start,
	                  const Time & end);

	void DeleteIdentification(const std::shared_ptr<Identification> & identification);
private:
	std::shared_ptr<Space> MakeSpacePtr(const std::shared_ptr<priv::Space> & space ) const;
	std::shared_ptr<Ant> MakeAntPtr(const std::shared_ptr<priv::Ant> & ant) const;
	SpaceByID d_spaces;
	AntByID   d_ants;
};

} // namespace myrmidon
} // namespace fort
