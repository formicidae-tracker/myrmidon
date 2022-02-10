#include "matchers.h"


#include "Rcpp.h"

void fmMatcher_show(const fort::myrmidon::Matcher::Ptr *m) {
	Rcpp::Rcout << "fmMatcher(" << fmMatcher_format(m) << ")\n";
}

std::string fmMatcher_format(const fort::myrmidon::Matcher::Ptr *m) {
	if ( m->get() == nullptr ) {
		return "TRUE";
	}
	std::ostringstream oss;
	oss << **m;
	return oss.str();
}

RCPP_MODULE(matchers) {
	Rcpp::class_<fort::myrmidon::Matcher::Ptr>("fmMatcher")
		.const_method("show",&fmMatcher_show)
		.const_method("format",&fmMatcher_format)
		;
}


namespace Rcpp {
template <> std::vector<fort::myrmidon::Matcher::Ptr> as(SEXP exp) {
	Rcpp::List matchers(exp);
	std::vector<fort::myrmidon::Matcher::Ptr> res;
	res.reserve(matchers.size());
	for ( const auto & m : matchers ) {
		res.push_back(as<fort::myrmidon::Matcher::Ptr>(m));
	}
	return res;
}
}


//' A fmMatcher that matches anything
//' @return a \code{\link{fmMatcher}} that matches everything.
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherAny() {
	return nullptr;
}

//' Combines fmMatcher in conjunction
//' @param matchers the \code{\link{fmMatcher}} to combine
//' @return a \code{\link{fmMatcher}} that matches when all
//'   **matchers** matches.
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherAnd(const std::vector<fort::myrmidon::Matcher::Ptr> & matchers) {
	return fort::myrmidon::Matcher::And(matchers);
}

//' Combines fmMatcher in disjunction
//' @param matchers the \code{\link{fmMatcher}} to combine
//' @return a \code{\link{fmMatcher}} that matches when any of the
//'   **matchers** matches.
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherOr(const std::vector<fort::myrmidon::Matcher::Ptr> & matchers) {
	return fort::myrmidon::Matcher::Or(matchers);
}


//' A fmMatcher that matches AntID
//' @details In the case of interactions, matches interaction with one
//'   of the Ant having **antID**.
//' @param antID the Ant ID to match
//' @return a \code{\link{fmMatcher}} that matches when one of the
//'   considered Ant is **antID**
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherAntID(fort::myrmidon::AntID antID) {
	return fort::myrmidon::Matcher::AntID(antID);
}

//' A fmMatcher that matches metadata key/value
//' @details In the case of interactions, matches interaction with one
//'   of the Ant meeting the criterion.
//' @param key (a character) the key to match
//' @param value the value to match. Should either be a logical,
//'   integer, numerical, character or \code{\link{fmTime}}.
//' @return a \code{\link{fmMatcher}} that matches when one of the
//'   considered Ant is has **key** equal to **value**.
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherAntMetaData(const std::string & key,
                                                  const fort::myrmidon::Value & value) {
	return fort::myrmidon::Matcher::AntMetaData(key,value);
}

//' A fmMatcher that matches ants distances
//' @details In the case of trajectories, it matches anything.
//' @param distance the distance in pixel to be smaller
//' @return a \code{\link{fmMatcher}} that matches when two Ant lies
//'   within the given distance.
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherAntDistanceSmallerThan(double distance) {
	return fort::myrmidon::Matcher::AntDistanceSmallerThan(distance);
}

//' A fmMatcher that matches ants distances
//' @details In the case of trajectories, it matches anything.
//' @param distance the distance in pixel to be greater
//' @return a \code{\link{fmMatcher}} that matches when two Ant lies
//'   apart of the given distance.
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherAntDistanceGreaterThan(double distance) {
	return fort::myrmidon::Matcher::AntDistanceGreaterThan(distance);
}

//' A fmMatcher that matches ants angles
//' @details In the case of trajectories, it matches anything.
//' @param angle the angle in radians to be smaller
//' @return a \code{\link{fmMatcher}} that matches when the absolute
//'     angle between two Ant is smaller than **angle**.
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherAntAngleSmallerThan(double angle) {
	return fort::myrmidon::Matcher::AntAngleSmallerThan(angle);
}


//' A fmMatcher that matches ants angles
//' @details In the case of trajectories, it matches anything.
//' @param angle the angle in radians to be larger
//' @return a \code{\link{fmMatcher}} that matches when the absolute
//'     angle between two Ant is larger than **angle**.
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherAntAngleGreaterThan(double angle) {
	return fort::myrmidon::Matcher::AntAngleGreaterThan(angle);
}


//' A fmMatcher that matches interaction types
//' @details In the case of trajectories, it matches anything.
//' @param type1 (integer) the first ant shape type to match
//' @param type2 (integer) the second ant shape type to match
//' @return a \code{\link{fmMatcher}} that matches
//'     (**type1**,**type2**) and (**type2**,**type1**) interactions.
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherInteractionType(fort::myrmidon::AntShapeTypeID type1,
                                                      fort::myrmidon::AntShapeTypeID type2) {
	return fort::myrmidon::Matcher::InteractionType(type1,type2);
}

//' A fmMatcher that matches ant displacement.
//' @param under the maximal allowed displacement in pixel
//' @param minimumGap minimal time Gap between check, as a
//'        \code{\link{fmDuration}}. Can possibly be 0.
//' @description Matches trajectories and interactions where the Ant
//'     displacement between two consecutive position is smaller than
//'     **under**. If **minimumGap** is not zero, this check will be
//'     enforced only if there was at least a duration of **minimumGap**
//'     between two consecutive positions.
//' @return a \code{\link{fmMatcher}} that rejects large displacement
//'   in a tracking gap.
//' @family fmMatcher methods
//[[Rcpp::export]]
fort::myrmidon::Matcher::Ptr fmMatcherAntDisplacement(double under,
                                                      fort::Duration minimumGap) {
	return fort::myrmidon::Matcher::AntDisplacement(under,minimumGap);
}
