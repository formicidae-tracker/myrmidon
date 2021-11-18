#pragma once

#include "HandleInContext.hpp"

#include <fort/myrmidon/types/ForwardDeclaration.hpp>

namespace fort {
namespace myrmidon {

namespace priv {
class Ant;
class Experiment;
class Identification;
}


class AntHandle : public priv::HandleInContext<priv::Ant,priv::Experiment> {
public:
	AntHandle(const std::shared_ptr<priv::Ant> & ant,
	          const std::shared_ptr<priv::Experiment> & experiment);


	const IdentificationList & Identifications() const;

	void ReflectPrivateIdentifications();

private:
	IdentificationList::const_iterator FindPrivateIdentification(const std::shared_ptr<priv::Identification> & pi);
	std::shared_ptr<Identification> MakeIdentificationPtr(const std::shared_ptr<priv::Identification> & identification);

	IdentificationList d_identifications;
};

} // namespace myrmidon
} // namespace fort
