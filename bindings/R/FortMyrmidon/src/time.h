#pragma once

#include "../inst/include/FortMyrmidon_types.hpp"

void fmDuration_show(const fort::Duration * t);
void fmTime_show(const fort::Time * t);

Rcpp::Datetime fmTime_asR(const fort::Time & t);
