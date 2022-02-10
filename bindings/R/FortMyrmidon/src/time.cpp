#include "time.h"

#include "Rcpp.h"

Rcpp::Datetime fmTime_asR(const fort::Time & t) {
	if ( t.IsSinceEver() ) {
		return -std::numeric_limits<double>::infinity();
	} else if ( t.IsForever() ) {
		return std::numeric_limits<double>::infinity();
	}
	return Rcpp::Datetime(t.Sub(fort::Time()).Seconds());
}

void fmTime_show(const fort::Time * t) {
	Rcpp::Rcout << *t << "\n";
}

void fmDuration_show(const fort::Duration * d) {
	Rcpp::Rcout << *d << "\n";
}

fort::Duration fmDuration_add(const fort::Duration * a,
                              const fort::Duration * b) {
	return *a + *b;
}

fort::Duration fmDuration_sub(const fort::Duration * a,
                              const fort::Duration * b) {
	return *a - *b;
}

bool fmDuration_lower(const fort::Duration * a,
                      const fort::Duration * b) {
	return *a < *b;
}

bool fmDuration_lowerEqual(const fort::Duration * a,
                           const fort::Duration * b) {
	return *a <= *b;
}

bool fmDuration_equals(const fort::Duration * a,
                      const fort::Duration * b) {
	return *a == *b;
}

SEXP fmTime_asPOSIXCt(const fort::Time * t) {
	if ( t->IsForever() ) {
		return Rcpp::wrap(R_PosInf);
	}
	if ( t->IsSinceEver() ) {
		return Rcpp::wrap(R_NegInf);
	}
	return Rcpp::wrap(Rcpp::Datetime(t->Sub(fort::Time()).Seconds()));
}

RCPP_MODULE(time) {
	using namespace Rcpp;
	class_<fort::Duration>("fmDuration")
		.constructor()
		.constructor<int32_t>()
		.const_method("show",
		              &fmDuration_show,
		              "shows a fmDuration")
		.const_method("hours",
		              &fort::Duration::Hours,
		              "the amount of hours represented by this duration")
		.const_method("minutes",
		              &fort::Duration::Minutes,
		              "the amount of minutes represented by this duration")
		.const_method("seconds",
		              &fort::Duration::Seconds,
		              "the amount of seconds represented by this duration")
		.const_method("milliseconds",
		              &fort::Duration::Milliseconds,
		              "the amount of milliseconds represented by this duration")
		.const_method("microseconds",
		              &fort::Duration::Microseconds,
		              "the amount of microseconds represented by this duration")
		.const_method("nanoseconds",
		              &fort::Duration::Nanoseconds,
		              "the amount of nanoseconds represented by this duration")
		.const_method("add",
		              &fmDuration_add,
		              "adds two fmDuration")
		.const_method("sub",
		              &fmDuration_sub,
		              "substracts two fmDurations")
		.const_method("lower",
		              &fmDuration_lower,
		              "compares two fmDurations")
		.const_method("lowerEqual",
		              &fmDuration_lowerEqual,
		              "compares two fmDurations")
		.const_method("equals",
		              &fmDuration_equals,
		              "compares two fmDuration")
		;


	class_<fort::Time>("fmTime")
		.constructor()
		.const_method("show",
		              &fmTime_show,
		              "shows a fmTime")
		.const_method("after",
		              &fort::Time::After,
		              "Checks if this fmTime is after another")
		.const_method("before",
		              &fort::Time::Before,
		              "Checks if this fmTime is before another")
		.const_method("equals",
		              &fort::Time::Equals,
		              "Checks if this fmTime is equal to another")
		.const_method("add",
		              &fort::Time::Add,
		              "adds a fmDuration to this fmTime")
		.const_method("sub",
		              &fort::Time::Sub,
		              "Computes the fmDuration between this fmTime and another")
		.const_method("round",
		              &fort::Time::Round,
		              "Rounds this fmTime to a fmDuration")
		.const_method("isInfinite",
		              &fort::Time::IsInfinite,
		              "Tests if this fmTime is infinite")
		.const_method("isForever",
		              &fort::Time::IsForever,
		              "Tests if this fmTime is infinite")
		.const_method("isSinceEver",
		              &fort::Time::IsSinceEver,
		              "Tests if this fmTime is infinite")
		.const_method("asPOSIXct",
		              &fmTime_asPOSIXCt,
		              "converts this time to a posixct")
		;
}

//' Creates a fmTime from an offset in second from the system's epoch
//'
//' @param offset the offset in second, Inf and -Inf are valid and
//'   respectively represents fmTimeForever() and fmTimeSinceEver().
//' @return the \code{\link{fmTime}} offseted by offset seconds from
//'   the system's epoch
//' @family fmTime methods
// [[Rcpp::export]]
fort::Time fmTimeCreate(double offset = 0.0) {
	if ( offset == std::numeric_limits<double>::infinity() ) {
		return fort::Time::Forever();
	} else if (offset == -std::numeric_limits<double>::infinity() ) {
		return fort::Time::SinceEver();
	}

	return fort::Time().Add(offset * fort::Duration::Second.Nanoseconds());
}


//' Returns current time as a fmTime
//'
//' @return the current time as a \code{\link{fmTime}}
//' @family fmTime methods
// [[Rcpp::export]]
fort::Time fmTimeNow() {
	return fort::Time::Now();
}

//' The +∞ time
//'
//' @return a \code{\link{fmTime}} representing +∞.
//' @family fmTime methods
// [[Rcpp::export]]
fort::Time fmTimeForever() {
	return fort::Time::Forever();
}

//' The -∞ time
//'
//' @return a \code{\link{fmTime}} representing -∞.
//' @family fmTime methods
// [[Rcpp::export]]
fort::Time fmTimeSinceEver() {
	return fort::Time::SinceEver();
}

//' Parses a time from a RFC3339 string representation
//' @description Parses a time from a RFC3339 (
//'   i.e. '1970-01-01T00:00:00.000Z' for UNIX epoch) string
//'   representation to a fmTime
//' @param input the string to parse
//' @examples
//' fmTimeParse('1970-01-01T00:00:00.000Z')$equals(fmTimeCreate()) # will be TRUE on UNIX systems
//' @return a \code{\link{fmTime}} representing \code{input}.
//' @family fmTime methods
// [[Rcpp::export]]
fort::Time fmTimeParse(const std::string  & input) {
	return fort::Time::Parse(input);
}


//' Parses a string to a fmDuration
//' @param input a string in the format `[amount][unit]` as a
//'   duration. Valid units are 'h','m','s','ms','us','ns'. The
//'   pattern can be repeated (i.e. '4m32s' is valid).
//' @examples
//' fmDurationParse("1m2s")$show() == '1m2s'
//' @return a \code{\link{fmDuration}} that is equal to input
//' @family fmDuration methods
// [[Rcpp::export]]
fort::Duration fmDurationParse(const std::string & input) {
	return fort::Duration::Parse(input);
}

//' A fmDuration in hours
//' @param h an amount of hours
//' @examples
//' fmHour(1.32)
//' fmHour(-3.2) # fmDuration can be negative
//' @return a \code{\link{fmDuration}} that is equal to \code{h} hours
//' @family fmDuration methods
// [[Rcpp::export]]
fort::Duration fmHour(double h) {
	return h * fort::Duration::Hour.Nanoseconds();
}

//' A fmDuration in minutes
//' @param m an amount of minutes
//' @examples
//' fmMinute(1.32)
//' fmMinute(-3.2) # fmDuration can be negative
//' @return a \code{\link{fmDuration}} that is equal to \code{m} minutes
//' @family fmDuration methods
// [[Rcpp::export]]
fort::Duration fmMinute(double m) {
	return m * fort::Duration::Minute.Nanoseconds();
}

//' A fmDuration in seconds
//' @param s an amount of seconds
//' @examples
//' fmSecond(1.32)
//' fmSecond(-3.2) # fmDuration can be negative
//' @return a \code{\link{fmDuration}} that is equal to \code{s} seconds
//' @family fmDuration methods
// [[Rcpp::export]]
fort::Duration fmSecond(double s) {
	return s * fort::Duration::Second.Nanoseconds();
}

//' A fmDuration in milliseconds
//' @param ms an amount of milliseconds
//' @examples
//' fmMillisecond(1.32)
//' fmMillisecond(-3.2) # fmDuration can be negative
//' @return a \code{\link{fmDuration}} that is equal to \code{ms} milliseconds
//' @family fmDuration methods
// [[Rcpp::export]]
fort::Duration fmMillisecond(double ms) {
	return ms * fort::Duration::Millisecond.Nanoseconds();
}

//' A fmDuration in microseconds
//' @param us an amount of microseconds
//' @examples
//' fmMicrosecond(1.32)
//' fmMicrosecond(-3.2) # fmDuration can be negative
//' @return a \code{\link{fmDuration}} that is equal to \code{us} microsecond
//' @family fmDuration methods
// [[Rcpp::export]]
fort::Duration fmMicrosecond(double us) {
	return us * fort::Duration::Microsecond.Nanoseconds();
}

//' A fmDuration in nanosecond
//' @description the smallest fmDuration representation. can only take integer values
//' @param ns an INTEGER amount of nanoseconds
//' @examples
//' fmNanosecond(1)
//' fmNanosecond(-3) # fmDuration can be negative
//' @return a \code{\link{fmDuration}} that is equal to \code{ns} nanosecond
//' @family fmDuration methods
// [[Rcpp::export]]
fort::Duration fmNanosecond(int64_t ns) {
	return ns;
}

// [[Rcpp::export]]
void pfmConsumeDuration(fort::Duration duration) {
}

namespace Rcpp {
template <> fort::Duration as(SEXP s) {
	RObject object(s);
	if ( object.isS4() ) {
		S4 s4object(s);
		if ( s4object.is("Rcpp_fmDuration") == true ) {
			Environment env(s4object);
			return *XPtr<fort::Duration>(env.get(".pointer"));
		}
	}
	if ( Rcpp::is<Rcpp::IntegerVector>(object) ) {
		return Rcpp::IntegerVector(s)[0];
	}
	throw std::invalid_argument("Only integer can be converted to fmDuration");
}
}
