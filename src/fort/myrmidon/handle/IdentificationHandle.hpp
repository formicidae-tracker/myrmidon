#pragma once

#include "HandleInContext.hpp"

namespace fort {
namespace myrmidon {

class IdentificationHandle : public priv::HandleInContext<priv::Identification,priv::Experiment> {
public:
	IdentificationHandle(const std::shared_ptr<priv::Identification> & identification,
	                     const std::shared_ptr<priv::Experiment> & experiment,
	                     const std::shared_ptr<priv::Ant> & ant)
		: HandleInContext(identification,experiment)
		, d_ant(ant) {
	}

private:
	std::shared_ptr<priv::Ant> d_ant;
};

} // namespace myrmidon
} // namespace fort
