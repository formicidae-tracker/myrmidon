#include "zone.h"

#include "helper.h"

#include "Rcpp.h"


IMPLEMENT_FIELD(ZoneDefinition,fort::myrmidon::Shape::List,Shapes);
IMPLEMENT_FIELD(ZoneDefinition,fort::Time,Start);
IMPLEMENT_FIELD(ZoneDefinition,fort::Time,End)


inline void fmZoneDefinition_show(const fort::myrmidon::ZoneDefinition::Ptr * zd) {
	Rcpp::Rcout << "fmZoneDefinition($start = " << (*zd)->Start()
	            << ", $end = " << (*zd)->End()
	            << ")";
}

inline uint64_t fmZoneDefinition_get(const fort::myrmidon::ZoneDefinition::Ptr * zd) {
	return uint64_t(zd->get());
}


IMPLEMENT_FIELD(Zone,std::string,Name);
IMPLEMENT_GETTER(Zone,fort::myrmidon::ZoneDefinitionList,Definitions);

inline void fmZone_show(const fort::myrmidon::Zone::Ptr * z) {
	Rcpp::Rcout << "fmZone($id = " << (*z)->ID()
	            << ", $name = " << (*z)->Name()
	            << ")";
}




inline uint32_t fmZone_ID(fort::myrmidon::Zone::Ptr * z) {
	return (*z)->ID();
}

inline fort::myrmidon::ZoneDefinition::Ptr
fmZone_AddDefinition(fort::myrmidon::Zone::Ptr * z,
                     const fort::myrmidon::Shape::List & shapes = {},
                     const fort::Time & start = fort::Time::SinceEver(),
                     const fort::Time & end = fort::Time::Forever()) {
	return (*z)->AddDefinition(shapes,start,end);
}

inline void fmZone_DeleteDefinition(fort::myrmidon::Zone::Ptr * z,
                                    size_t index) {
	(*z)->DeleteDefinition(index-1);
}


inline uint64_t fmZone_get(const fort::myrmidon::Zone::Ptr * z) {
	return uint64_t(z->get());
}


RCPP_MODULE(zone) {
	using namespace Rcpp;

	class_<fort::myrmidon::ZoneDefinition::Ptr>("fmZoneDefinition")
		.const_method("show",&fmZoneDefinition_show)
		.property("shapes",
		          &fmZoneDefinition_Shapes,
		          &fmZoneDefinition_SetShapes)
		.property("start",
		          &fmZoneDefinition_Start,
		          &fmZoneDefinition_SetStart)
		.property("end",
		          &fmZoneDefinition_End,
		          &fmZoneDefinition_SetEnd)
		.const_method("get",
		              &fmZoneDefinition_get)
		;

	class_<fort::myrmidon::Zone::Ptr>("fmZone")
		.const_method("show",&fmZone_show)
		.property("ID",&fmZone_ID, "This fmZone ID")
		.property("definitions",&fmZone_Definitions,"This fmZone Definitions")
		.property("name",
		          &fmZone_Name,
		          &fmZone_SetName,
		          "This zone name.")
		.method("addDefinition",
		        &fmZone_AddDefinition,
		        "Adds a zone definition to this zone.")
		.method("deleteDefinition",
		        &fmZone_DeleteDefinition,
		        "Removes a definition in this zone.")
		.const_method("get",
		              &fmZone_get)
		;
}


namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::ZoneDefinitionList & l) {
	return Rcpp::wrap(l.begin(),l.end());
}

}
