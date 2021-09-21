#include "space.h"

#include "Rcpp.h"


inline fort::myrmidon::Zone::Ptr fmSpace_createZone(fort::myrmidon::Space::Ptr * space,
                                                    const std::string & name) {
	return (*space)->CreateZone(name);
}

RCPP_MODULE(space) {
	using namespace Rcpp;

	class_<fort::myrmidon::Space::Ptr>("fmSpace")
		.method("createZone",&fmSpace_createZone)
		;

}
