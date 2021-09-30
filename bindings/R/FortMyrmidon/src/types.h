#pragma once

#include "../inst/include/FortMyrmidon_types.hpp"

#include "Rcpp.h"

Rcpp::DataFrame fmIdentifiedFrame_Positions(fort::myrmidon::IdentifiedFrame & frame);

std::string fmInteractionTypes_asStr(const fort::myrmidon::InteractionTypes & types );

Rcpp::DataFrame fmAntTrajectory_Positions(const fort::myrmidon::AntTrajectory & t);

std::string fmInteraction_ZoneList(const std::set<fort::myrmidon::ZoneID> & zones);
