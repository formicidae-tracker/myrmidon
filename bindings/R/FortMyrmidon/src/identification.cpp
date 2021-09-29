#include "identification.h"


#include "Rcpp.h"

#include "helper.h"

void fmIdentification_show(const fort::myrmidon::Identification::Ptr *i) {
	Rcpp::Rcout << "fmIdentification( $tagValue = "
	            << fort::myrmidon::FormatTagID((*i)->TagValue())
	            << " , $targetAntID = "
	            << fort::myrmidon::FormatAntID((*i)->TargetAntID())
	            << " , $start = " << (*i)->Start()
	            << " , $end = " << (*i)->End()
	            << " )\n";
}

IMPLEMENT_GETTER(Identification,uint32_t,TagValue)
IMPLEMENT_GETTER(Identification,uint32_t,TargetAntID)
IMPLEMENT_FIELD(Identification,const fort::Time &,Start);
IMPLEMENT_FIELD(Identification,const fort::Time &,End);
IMPLEMENT_GETTER(Identification,Eigen::Vector2d,AntPosition);
IMPLEMENT_GETTER(Identification,double,AntAngle);
IMPLEMENT_FIELD(Identification,double,TagSize);

IMPLEMENT_METHOD(Identification,bool,HasDefaultTagSize);
IMPLEMENT_METHOD(Identification,bool,HasUserDefinedAntPose);
IMPLEMENT_VOID_METHOD(Identification,SetUserDefinedAntPose,
                      const Eigen::Vector2d &, position,
                      double, angle);
IMPLEMENT_VOID_METHOD(Identification,ClearUserDefinedAntPose);

RCPP_MODULE(identification) {
	Rcpp::class_<fort::myrmidon::Identification::Ptr>("fmIdentification")
		.const_method("show",
		              &fmIdentification_show,
		              "shows this identification")
		.property("tagValue",
		          &fmIdentification_TagValue,
		          "tag value for this identification")
		.property("targetAntID",
		          &fmIdentification_TargetAntID,
		          "target Ant ID for this identification")
		.property("antPosition",
		          &fmIdentification_AntPosition,
		          "Ant Position")
		.property("antAngle",
		          &fmIdentification_AntAngle,
		          "Ant Angle")
		.property("start",
		          &fmIdentification_Start,
		          &fmIdentification_SetStart,
		          "first valid time for this identification")
		.property("end",
		          &fmIdentification_End,
		          &fmIdentification_SetEnd,
		          "first invalid time for this identification")
		.property("tagSize",
		          &fmIdentification_TagSize,
		          &fmIdentification_SetTagSize,
		          "tag size in millimeter")
		.method("hasDefaultTagSize",
		              &fmIdentification_HasDefaultTagSize,
		              "Returns TRUE if this identification uses the experiment's default tag size")
		.method("hasUserDefinedAntPose",
		              &fmIdentification_HasUserDefinedAntPose,
		              "Returns TRUE if this identification has a user defined ant pose")
		.method("setUserDefinedAntPose",
		        &fmIdentification_SetUserDefinedAntPose,
		        "Sets the user defined ant pose")
		.method("clearUserDefinedAntPose",
		        &fmIdentification_ClearUserDefinedAntPose,
		        "Clears the user defined ant pose")
		;

}

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::IdentificationList & l) {
	return wrap(l.begin(),l.end());
}

}
