#pragma once

#include "HandleInContext.hpp"

#include "IdentificationHandle.hpp"

namespace fort {
namespace myrmidon {

class AntHandle : public priv::HandleInContext<priv::Ant,priv::Experiment> {
public:
	AntHandle(const std::shared_ptr<priv::Ant> & ant,
	          const std::shared_ptr<priv::Experiment> & experiment)
		: HandleInContext(ant,experiment) {
		ReflectPrivateIdentifications();
	}


	const IdentificationList & Identifications() const {
		return d_identifications;
	}

	void ReflectPrivateIdentifications() {
		const auto & identifications = Get().Identifications();
		auto isNotInPrivateList
			= [&identifications] (const Identification::Ptr & i) {
				  return std::find_if(identifications.begin(),
				                      identifications.end(),
				                      [&i] (const priv::Identification::Ptr & pi) -> bool {
					                      return i->d_p->Ptr() == pi;
				                      }) != identifications.end();
			  };

		d_identifications.erase(std::remove_if(d_identifications.begin(),
		                                       d_identifications.end(),
		                                       isNotInPrivateList));


		for ( const auto & pi : identifications) {
			if ( FindPrivateIdentification(pi) == d_identifications.end() ) {
				d_identifications.push_back(MakeIdentificationPtr(pi));
			}
		}

		priv::TimeValid::SortAndCheckOverlap(d_identifications.begin(),
		                                     d_identifications.end());
	}

private:

	IdentificationList::const_iterator FindPrivateIdentification(const priv::Identification::Ptr & pi) {
		return std::find_if(d_identifications.begin(),
		                    d_identifications.end(),
		                    [&pi](const Identification::Ptr & i) -> bool {
			                    return i->d_p->Ptr() == pi;
		                    });
	}

	Identification::Ptr MakeIdentificationPtr(const priv::Identification::Ptr & identification) {
		return Identification::Ptr(new Identification(std::make_unique<IdentificationHandle>(identification,d_context,d_object)));
	}

	IdentificationList d_identifications;
};

} // namespace myrmidon
} // namespace fort
