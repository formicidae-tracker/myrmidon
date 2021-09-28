#include "space.h"

#include "helper.h"

#include "Rcpp.h"

#include "wrapMap.h"

IMPLEMENT_FIELD(Space,std::string,Name)
IMPLEMENT_GETTER(Space,fort::myrmidon::ZoneByID,Zones)

inline void fmSpace_show(const fort::myrmidon::Space::Ptr * s) {
	Rcpp::Rcout << "fmSpace( $ID= " << (*s)->ID()
	            << ", $name = " << (*s)->Name()
	            << ")\n";
}

inline uint32_t fmSpace_ID(fort::myrmidon::Space::Ptr * s) {
	return (*s)->ID();
}

inline fort::myrmidon::Zone::Ptr fmSpace_createZone(fort::myrmidon::Space::Ptr * space,
                                                    const std::string & name) {
	return (*space)->CreateZone(name);
}

inline void fmSpace_deleteZone(fort::myrmidon::Space::Ptr * space,
                               fort::myrmidon::ZoneID zoneID) {
	(*space)->DeleteZone(zoneID);
}

inline SEXP fmSpace_locateMovieFrame(const fort::myrmidon::Space::Ptr * space,
                                     const fort::Time & time) {

	auto [filepath,frameID] = (*space)->LocateMovieFrame(time);
	return Rcpp::List::create(Rcpp::Named("filepath") = filepath,
	                          Rcpp::Named("frameID") = frameID);
}

RCPP_MODULE(space) {
	using namespace Rcpp;

	class_<fort::myrmidon::Space::Ptr>("fmSpace")
		.const_method("show",&fmSpace_show,"shows this space")
		.property("ID",&fmSpace_ID,"This space ID")
		.property("name",&fmSpace_Name,&fmSpace_SetName,"This space name")
		.property("zones",&fmSpace_Zones,"This space zones")
		.method("createZone",&fmSpace_createZone, "Creates a Zone in this Space")
		.method("deleteZone",&fmSpace_deleteZone, "Deletes a Zone in this Space")
		.const_method("locateMovieFrame",&fmSpace_locateMovieFrame,"Locates a movie frame in this space")
		;

}

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::ZoneByID & zones) {
	return wrapMap(zones);
}

}
