#include "AntHandle.hpp"

#include <fort/myrmidon/priv/Ant.hpp>
#include <fort/myrmidon/priv/Identification.hpp>
#include <fort/myrmidon/Identification.hpp>

#include "IdentificationHandle.hpp"

namespace fort {
namespace myrmidon {

AntHandle::AntHandle(const std::shared_ptr<priv::Ant> & ant,
                     const std::shared_ptr<priv::Experiment> & experiment)
	: HandleInContext(ant,experiment) {
	ReflectPrivateIdentifications();
}

const IdentificationList & AntHandle::Identifications() const {
	return d_identifications;
}

void AntHandle::ReflectPrivateIdentifications() {
	const auto & pIdentifications = Get().Identifications();
	auto isNotInPrivateList
		= [&pIdentifications] (const Identification::Ptr & i) {
			  return std::find_if(pIdentifications.begin(),
			                      pIdentifications.end(),
			                      [&i] (const priv::Identification::Ptr & pi) -> bool {
				                      return i->d_p->Ptr() == pi;
			                      }) == pIdentifications.end();
			  };

	d_identifications.erase(std::remove_if(d_identifications.begin(),
	                                       d_identifications.end(),
	                                       isNotInPrivateList),
	                        d_identifications.end());


	for ( const auto & pi : pIdentifications) {
		if ( FindPrivateIdentification(pi) == d_identifications.end() ) {
			d_identifications.push_back(MakeIdentificationPtr(pi));
		}
	}

	priv::TimeValid::SortAndCheckOverlap(d_identifications.begin(),
	                                     d_identifications.end());
}

IdentificationList::const_iterator AntHandle::FindPrivateIdentification(const priv::Identification::Ptr & pi) {
	return std::find_if(d_identifications.begin(),
	                    d_identifications.end(),
	                    [&pi](const Identification::Ptr & i) -> bool {
		                    return i->d_p->Ptr() == pi;
	                    });
}

Identification::Ptr AntHandle::MakeIdentificationPtr(const priv::Identification::Ptr & identification) {
	return Identification::Ptr(new Identification(std::make_unique<IdentificationHandle>(identification,d_context,d_object)));
}


}
}
