#include "identification.h"


#include "Rcpp.h"

RCPP_MODULE(identification) {
	Rcpp::class_<fort::myrmidon::Identification::Ptr>("fmIdentification")
		;
}

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::IdentificationList & l) {
	return wrap(l.begin(),l.end());
}

}
