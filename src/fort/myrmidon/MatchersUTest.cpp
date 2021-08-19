#include <gtest/gtest.h>

#include "Matchers.hpp"
#include "priv/Matchers.hpp"

namespace fort {
namespace myrmidon {

class PublicMatchersUTest : public ::testing::Test {};

TEST_F(PublicMatchersUTest,RightMatcher) {
	struct TestData {
		Matcher::Ptr M;
		std::string  Formatted;
	};
	std::vector<TestData> testdata =
		{
		 { Matcher::And({Matcher::AntID(1),
		                 Matcher::AntID(2)}),"( Ant.ID == 001 && Ant.ID == 002 )" },
		 { Matcher::Or({Matcher::AntID(1),
		                Matcher::AntID(2)}),"( Ant.ID == 001 || Ant.ID == 002 )" },
		 { Matcher::AntID(1),"Ant.ID == 001" },
		 { Matcher::AntMetaData("group",std::string("nurse")),"Ant.'group' == nurse" },
		 { Matcher::AntDistanceSmallerThan(10.0),"Distance(Ant1, Ant2) < 10" },
		 { Matcher::AntDistanceGreaterThan(10.0),"Distance(Ant1, Ant2) > 10" },
		 { Matcher::AntAngleSmallerThan(1.0),"Angle(Ant1, Ant2) < 1" },
		 { Matcher::AntAngleGreaterThan(1.0),"Angle(Ant1, Ant2) > 1" },
		 { Matcher::InteractionType(1,1),"InteractionType(1 - 1)" },
		 { Matcher::InteractionType(2,1),"InteractionType(1 - 2)" },
		 { Matcher::AntDisplacement(10.0,2),"AntDisplacement(under: 10, minimumGap: 2ns)" },
		};

	for ( const auto & d : testdata ) {
		std::ostringstream oss;
		oss << *d.M;
		EXPECT_EQ(oss.str(),d.Formatted);
	}
}

} // namespace myrmidon
} // namespace fort
