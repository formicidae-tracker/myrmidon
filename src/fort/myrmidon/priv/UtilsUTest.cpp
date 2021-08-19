#include "UtilsUTest.hpp"

#include <fort/myrmidon/UtilsUTest.hpp>

::testing::AssertionResult AssertTimedStatsEqual(const char * aExpr,
                                                 const char * bExpr,
                                                 const fort::myrmidon::priv::TagStatisticsHelper::Timed & a,
                                                 const fort::myrmidon::priv::TagStatisticsHelper::Timed & b) {
	auto startAssertion = AssertTimeEqual((std::string(aExpr)+".Start").c_str(),
	                                      (std::string(bExpr)+".Start").c_str(),
	                                      a.Start,b.Start);
	if ( !startAssertion ) {
		return startAssertion;
	}
	auto endAssertion = AssertTimeEqual((std::string(aExpr)+".End").c_str(),
	                                    (std::string(bExpr)+".End").c_str(),
	                                    a.End,b.End);
	if ( !endAssertion ) {
		return endAssertion;
	}

	return AssertTagStatisticsEqual((std::string(aExpr)+".TagStats").c_str(),
	                                (std::string(bExpr)+".TagStats").c_str(),
	                                a.TagStats,b.TagStats);
}
