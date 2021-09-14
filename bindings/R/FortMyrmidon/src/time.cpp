#include "time.h"

#include "Rcpp.h"


void fmTime_show(const fort::Time * t) {
	Rcpp::Rcout << *t << "\n";
}





RCPP_MODULE(time) {
	using namespace Rcpp;
	class_<fort::Time>("fmTime")
		.constructor("foo")
		.const_method("show",&fmTime_show,"foo")
		.const_method("after",&fort::Time::After,"foo")
		.const_method("before",&fort::Time::Before,"foo")
		.const_method("equals",&fort::Time::Equals,"foo")
		.const_method("add",&fort::Time::Add,"foo")
		.const_method("sub",&fort::Time::Sub,"foo")
		.const_method("round",&fort::Time::Round,"foo")
		;

	function("fmTimeNow",
	         &fort::Time::Now,
	         R"rdoc(Returns the current time

@return the current time as a Rcpp_fmTime
@export fmTimeNow
)rdoc");

}
