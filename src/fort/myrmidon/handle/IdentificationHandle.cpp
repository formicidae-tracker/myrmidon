#include "IdentificationHandle.hpp"


namespace fort {
namespace myrmidon {

IdentificationHandle::IdentificationHandle(const std::shared_ptr<priv::Identification> & identification,
                                           const std::shared_ptr<priv::Experiment> & experiment,
                                           const std::shared_ptr<priv::Ant> & ant)
	: HandleInContext(identification,experiment)
	, d_ant(ant) {
}

} // namespace myrmidon
} // namespace fort
