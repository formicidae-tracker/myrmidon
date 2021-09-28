#include "ant.h"

#include "Rcpp.h"

#include "helper.h"

void fmAnt_show(const fort::myrmidon::Ant::Ptr * a) {
	Rcpp::Rcout << "fmAnt( $ID = " << (*a)->ID()
	            << " )\n";
}

inline uint32_t fmAnt_IdentifiedAt(const fort::myrmidon::Ant::Ptr * a,
                                   const fort::Time & time) {
	return (*a)->IdentifiedAt(time);
}

IMPLEMENT_GETTER(Ant,fort::myrmidon::IdentificationList,Identifications)

inline uint32_t fmAnt_ID(fort::myrmidon::Ant::Ptr *a) {
	return (*a)->ID();
}

IMPLEMENT_FIELD(Ant,fort::myrmidon::Color,DisplayColor)

inline int fmAnt_DisplayStatus(fort::myrmidon::Ant::Ptr *a) {
	return int((*a)->DisplayStatus());
}

inline void fmAnt_SetDisplayStatus(fort::myrmidon::Ant::Ptr *a, int status) {
	(*a)->SetDisplayStatus(fort::myrmidon::Ant::DisplayState(status));
}


RCPP_MODULE(ant) {
	Rcpp::class_<fort::myrmidon::Ant::Ptr>("fmAnt")
		.const_method("show",
		              &fmAnt_show,
		              "shows this Ant")
		.const_method("identifiedAt",
		              &fmAnt_IdentifiedAt,
		              "Gets the tag that identifies this Ant for a time")
		.property("identifications",
		          &fmAnt_Identifications,
		          "The fmIdentifications for this Ant")
		.property("ID",
		          &fmAnt_ID,
		          "The ID for this Ant")
		.property("displayColor",
		          &fmAnt_DisplayColor,
		          &fmAnt_SetDisplayColor,
		          "The display color for this Ant")
		.property("displayStatus",
		          &fmAnt_DisplayStatus,
		          &fmAnt_SetDisplayStatus,
		          "The display status for this Ant")
		;

}
