#include <gtest/gtest.h>

#include "TestSetup.hpp"
#include <fort/myrmidon/types/IdentifiedFrame.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace fort {
namespace myrmidon {

class PublicTypesUTest : public ::testing::Test {};

TEST_F(PublicTypesUTest, IdentifiedFrameMethods) {
	IdentifiedFrame::Ptr data;
	for (const auto &[identified, collided] :
	     TestSetup::UTestData().ExpectedFrames()) {
		if (identified->Space == 1) {
			data = identified;
			break;
		}
	}
	EXPECT_TRUE(data->Contains(1));
	EXPECT_TRUE(data->Contains(2));
	EXPECT_TRUE(data->Contains(3));
	EXPECT_FALSE(data->Contains(43));

	EXPECT_THROW({ data->At(42); }, std::out_of_range);

	auto [antID, position, zoneID] = data->At(0);
	EXPECT_EQ(antID, 1);
	EXPECT_EQ(zoneID, 0);
}

TEST_F(PublicTypesUTest, AntTrajectoryMethods) {
	AntTrajectory data;
	EXPECT_EQ(data.End(), data.Start);
	data.Positions.resize(1, 5);
	data.Positions.setZero();
	EXPECT_EQ(data.End(), data.Start);
	data.Positions.conservativeResize(2, 5);
	data.Positions(1, 0) = 1.0;
	EXPECT_EQ(data.End(), data.Start.Add(Duration::Second));
}

TEST_F(PublicTypesUTest, AntTrajectorySegment) {
	AntTrajectorySegment s;
	EXPECT_EQ(s.StartTime(), Time::SinceEver());
	EXPECT_EQ(s.EndTime(), Time::Forever());
	s.Trajectory = std::make_shared<AntTrajectory>();
	s.Trajectory->Positions.resize(3, 5);
	s.Trajectory->Positions(0, 0) = 0.0;
	s.Trajectory->Positions(1, 0) = 0.5;
	s.Trajectory->Positions(2, 0) = 1.0;
	s.Begin                       = 0;
	s.End                         = 2;
	EXPECT_EQ(s.StartTime(), s.Trajectory->Start);
	EXPECT_EQ(
	    s.EndTime(),
	    s.Trajectory->Start.Add(500 * Duration::Millisecond)
	);
	s.Begin = 1;
	s.End   = 3;
	EXPECT_EQ(
	    s.StartTime(),
	    s.Trajectory->Start.Add(500 * Duration::Millisecond)
	);
	EXPECT_EQ(
	    s.EndTime(),
	    s.Trajectory->Start.Add(1000 * Duration::Millisecond)
	);
}

} // namespace myrmidon
} // namespace fort
