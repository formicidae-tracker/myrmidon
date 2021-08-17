#include <fort/time/Time.hpp>

#include "TagStatisticsUTest.hpp"

#include "TagStatistics.hpp"

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/UtilsUTest.hpp>

#include "TrackingDataDirectory.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(TagStatisticsUTest,ComputeAndUpdatesGap) {

	struct TestData {
		Duration D;
		TagStatistics::CountHeader H;
	};

	std::vector<TestData> testdata
		= {
		   {-1,TagStatistics::CountHeader(0)},
		   {1,TagStatistics::GAP_500MS},
		   {499*Duration::Millisecond,TagStatistics::GAP_500MS},
		   {500*Duration::Millisecond,TagStatistics::GAP_1S},
		   {999*Duration::Millisecond,TagStatistics::GAP_1S},
		   {1*Duration::Second,TagStatistics::GAP_10S},
		   {9*Duration::Second,TagStatistics::GAP_10S},
		   {10*Duration::Second,TagStatistics::GAP_1M},
		   {59*Duration::Second,TagStatistics::GAP_1M},
		   {1*Duration::Minute,TagStatistics::GAP_10M},
		   {9*Duration::Minute,TagStatistics::GAP_10M},
		   {10*Duration::Minute,TagStatistics::GAP_1H},
		   {59*Duration::Minute,TagStatistics::GAP_1H},
		   {1*Duration::Hour,TagStatistics::GAP_10H},
		   {9*Duration::Hour,TagStatistics::GAP_10H},
		   {10*Duration::Hour,TagStatistics::GAP_MORE},
	};

	Time t;
	for ( const auto & d : testdata ) {
		auto stat = TagStatisticsHelper::Create(0,t);
		auto gap  = TagStatisticsHelper::ComputeGap(t,t.Add(d.D));
		EXPECT_EQ(gap,d.H) << " testing for " << d.D;
		TagStatisticsHelper::UpdateGaps(stat,t,t.Add(d.D));
		if ( d.H == 0 ) {
			EXPECT_EQ(stat.Counts.sum(),1);
		} else {
			EXPECT_EQ(stat.Counts(d.H),1);
		}
	}

}

::testing::AssertionResult AssertTimedEqual(const char * aExpr,
                                            const char * bExpr,
                                            const TagStatisticsHelper::Timed & a,
                                            const TagStatisticsHelper::Timed & b) {
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



} // namespace priv
} // namespace myrmidon
} // namespace fort
