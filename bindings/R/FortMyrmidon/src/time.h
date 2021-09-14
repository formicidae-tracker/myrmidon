#include <fort/time/Time.hpp>

#include <RcppCommon.h>

RCPP_EXPOSED_CLASS_NODECL(fort::Duration);
RCPP_EXPOSED_CLASS_NODECL(fort::Time);

void fmDuration_show(const fort::Duration * t);
void fmTime_show(const fort::Time * t);
