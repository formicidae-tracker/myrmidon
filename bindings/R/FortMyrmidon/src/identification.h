#include <fort/myrmidon/Identification.hpp>

#include "RcppCommon.h"

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Identification)

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::Identification::List &);

}

void fmIdentification_show(const fort::myrmidon::Identification * i);

void fmCIdentification_show(const fort::myrmidon::CIdentification * i);
