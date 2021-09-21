#include "zone.h"

#include "Rcpp.h"

void fmZoneDefinition_show(const fort::myrmidon::ZoneDefinition * zd) {
	Rcpp::Rcout << "fmZoneDefinition($start = " << zd->Start()
	            << ", $end = " << zd->End()
	            << ")";
}

void fmZone_show(const fort::myrmidon::Zone * zd) {
	Rcpp::Rcout << "fmZone($id = " << zd->ID()
	            << ", $name = " << zd->Name()
	            << ")";
}


RCPP_MODULE(zone) {
	using namespace Rcpp;

	class_<fort::myrmidon::ZoneDefinition>("fmZoneDefinition")
		.const_method("show",&fmZoneDefinition_show)
		.property("shapes",
		          &fort::myrmidon::ZoneDefinition::Shapes,
		          &fort::myrmidon::ZoneDefinition::SetShapes)
		.property("start",
		          &fort::myrmidon::ZoneDefinition::Start,
		          &fort::myrmidon::ZoneDefinition::SetStart)
		.property("end",
		          &fort::myrmidon::ZoneDefinition::End,
		          &fort::myrmidon::ZoneDefinition::SetEnd)
		;

	class_<fort::myrmidon::Zone>("fmZone")
		.const_method("show",&fmZone_show)
		.property("ID",&fort::myrmidon::Zone::ID)
		.property("name",
		          &fort::myrmidon::Zone::Name,
		          &fort::myrmidon::Zone::SetName)
		;
}
