#include "space.h"

#include "helper.h"

#include "Rcpp.h"

#include "wrapMap.h"

IMPLEMENT_FIELD(Space,const std::string & ,Name)
IMPLEMENT_GETTER(Space,const fort::myrmidon::ZoneByID &,Zones)

inline void fmSpace_show(const fort::myrmidon::Space::Ptr * s) {
	Rcpp::Rcout << "fmSpace( $ID= " << (*s)->ID()
	            << ", $name = " << (*s)->Name()
	            << ")\n";
}

IMPLEMENT_GETTER(Space,uint32_t,ID);

IMPLEMENT_METHOD(Space,fort::myrmidon::Zone::Ptr,CreateZone,const std::string &, name)
IMPLEMENT_VOID_METHOD(Space,DeleteZone,fort::myrmidon::ZoneID, zoneID)

inline SEXP fmSpace_locateMovieFrame(const fort::myrmidon::Space::Ptr * space,
                                     const fort::Time & time) {

	auto [filepath,frameID] = (*space)->LocateMovieFrame(time);
	return Rcpp::List::create(Rcpp::Named("filepath") = filepath,
	                          Rcpp::Named("frameID") = frameID);
}

inline uint64_t fmSpace_get(const fort::myrmidon::Space::Ptr * s) {
	return uint64_t(s->get());
}

RCPP_MODULE(space) {
	using namespace Rcpp;

	class_<fort::myrmidon::Space::Ptr>("fmSpace")
		.const_method("show",&fmSpace_show,"shows this space")
		.property("ID",&fmSpace_ID,"This space ID")
		.property("name",&fmSpace_Name,&fmSpace_SetName,"This space name")
		.property("zones",&fmSpace_Zones,"This space zones")
		.method("createZone",&fmSpace_CreateZone, "Creates a Zone in this Space")
		.method("deleteZone",&fmSpace_DeleteZone, "Deletes a Zone in this Space")
		.const_method("locateMovieFrame",&fmSpace_locateMovieFrame,"Locates a movie frame in this space")
		.const_method("get",&fmSpace_get)
		;

}

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::ZoneByID & zones) {
	return wrapMap(zones);
}

}
