#include <gtest/gtest.h>

#include "Traits.hpp"
#include <fort/myrmidon/UtilsUTest.hpp>

#include "IdentifiedFrame.hpp"
#include "Collision.hpp"
#include "AntTrajectory.hpp"
#include "AntInteraction.hpp"

namespace fort {
namespace myrmidon {

class DataTraitsUTest : public ::testing::Test {};

TEST_F(DataTraitsUTest,HasSpaceField) {
	static_assert(has_space_field<IdentifiedFrame>::value);
	static_assert(has_space_field<CollisionFrame>::value);
	static_assert(has_space_field<AntInteraction>::value);
	static_assert(has_space_field<AntTrajectory>::value);
	static_assert(not has_space_field<fort::Time>::value);
}


TEST_F(DataTraitsUTest,HasEndFieldOrMember) {
	static_assert(not has_end_field<IdentifiedFrame>::value);
	static_assert(not has_end_field<CollisionFrame>::value);
	static_assert(not has_end_field<AntTrajectory>::value);
	static_assert(has_end_field<AntInteraction>::value);

	static_assert(not has_end_member<IdentifiedFrame>::value);
	static_assert(not has_end_member<CollisionFrame>::value);
	static_assert(has_end_member<AntTrajectory>::value);
	static_assert(not has_end_member<AntInteraction>::value);
}


TEST_F(DataTraitsUTest,IdentifiedFrame) {

	static_assert(std::is_same<data_traits<IdentifiedFrame>::data_category,timed_data>::value);
	static_assert(data_traits<IdentifiedFrame>::spaced_data);

	IdentifiedFrame a,b;
	a.Space = 1;
	b.Space = 2;
	b.FrameTime = Time().Add(1);
	// Accessor check
	EXPECT_TIME_EQ(data_traits<IdentifiedFrame>::time(a),Time());
	EXPECT_TIME_EQ(data_traits<IdentifiedFrame>::time(b),Time().Add(1));
	EXPECT_EQ(data_traits<IdentifiedFrame>::space(a),1);
	EXPECT_EQ(data_traits<IdentifiedFrame>::space(b),2);
	// ordering check
	EXPECT_TRUE(data_traits<IdentifiedFrame>::compare(a,b));
	EXPECT_FALSE(data_traits<IdentifiedFrame>::compare(a,a));
	EXPECT_FALSE(data_traits<IdentifiedFrame>::compare(b,a));
}

TEST_F(DataTraitsUTest,CollisionData) {

	static_assert(std::is_same<data_traits<CollisionData>::data_category,timed_data>::value);
	static_assert(data_traits<CollisionData>::spaced_data);

	CollisionData a,b;
	a.first = std::make_shared<IdentifiedFrame>();
	b.first = std::make_shared<IdentifiedFrame>();
	a.first->Space = 1;
	b.first->Space = 2;
	b.first->FrameTime = Time().Add(1);
	// Accessor check
	EXPECT_TIME_EQ(data_traits<CollisionData>::time(a),Time());
	EXPECT_TIME_EQ(data_traits<CollisionData>::time(b),Time().Add(1));
	EXPECT_EQ(data_traits<CollisionData>::space(a),1);
	EXPECT_EQ(data_traits<CollisionData>::space(b),2);
	// ordering check
	EXPECT_TRUE(data_traits<CollisionData>::compare(a,b));
	EXPECT_FALSE(data_traits<CollisionData>::compare(a,a));
	EXPECT_FALSE(data_traits<CollisionData>::compare(b,a));
}

TEST_F(DataTraitsUTest,AntTrajectory) {

	static_assert(std::is_same<data_traits<AntTrajectory>::data_category,time_ranged_data>::value);
	static_assert(data_traits<AntTrajectory>::spaced_data);

	AntTrajectory a,b;
	a.Start = Time();
	b.Start = Time().Add(1);
	a.Space = 1;
	b.Space = 2;
	// Accessor check
	EXPECT_TIME_EQ(data_traits<AntTrajectory>::start(a),Time());
	EXPECT_TIME_EQ(data_traits<AntTrajectory>::start(b),Time().Add(1));
	EXPECT_TIME_EQ(data_traits<AntTrajectory>::end(a),Time());
	EXPECT_TIME_EQ(data_traits<AntTrajectory>::end(b),Time().Add(1));
	EXPECT_EQ(data_traits<AntTrajectory>::space(a),1);
	EXPECT_EQ(data_traits<AntTrajectory>::space(b),2);
	// ordering check
	EXPECT_TRUE(data_traits<AntTrajectory>::compare(a,b));
	EXPECT_FALSE(data_traits<AntTrajectory>::compare(a,a));
	EXPECT_FALSE(data_traits<AntTrajectory>::compare(b,a));
}


TEST_F(DataTraitsUTest,AntInteraction) {
	static_assert(std::is_same<data_traits<AntInteraction>::data_category,time_ranged_data>::value);
	static_assert(data_traits<AntInteraction>::spaced_data);

	AntInteraction a,b;
	a.Start = Time().Add(-1);
	a.End = Time();
	b.Start = Time().Add(-2);
	b.End = Time().Add(1);
	a.Space = 1;
	b.Space = 2;
	// Accessor check
	EXPECT_TIME_EQ(data_traits<AntInteraction>::start(a),Time().Add(-1));
	EXPECT_TIME_EQ(data_traits<AntInteraction>::start(b),Time().Add(-2));
	EXPECT_TIME_EQ(data_traits<AntInteraction>::end(a),Time());
	EXPECT_TIME_EQ(data_traits<AntInteraction>::end(b),Time().Add(1));
	EXPECT_EQ(data_traits<AntInteraction>::space(a),1);
	EXPECT_EQ(data_traits<AntInteraction>::space(b),2);
	// ordering check
	EXPECT_TRUE(data_traits<AntInteraction>::compare(a,b));
	EXPECT_FALSE(data_traits<AntInteraction>::compare(a,a));
	EXPECT_FALSE(data_traits<AntInteraction>::compare(b,a));
}


TEST_F(DataTraitsUTest,UserDefinedData) {

	struct TimedData {
		typedef timed_data data_category;
		Time    FrameTime;
	};

	struct TimeSpacedData : public TimedData {
		SpaceID Space;
	};

	struct TimeRangedData {
		typedef timed_data data_category;
		Time    Start,End;
	};

	struct TimeRangedSpacedData : public TimeRangedData {
		SpaceID Space;
	};

	static_assert(not data_traits<TimedData>::spaced_data);
	static_assert(    data_traits<TimeSpacedData>::spaced_data);
	static_assert(not data_traits<TimeRangedData>::spaced_data);
	static_assert(    data_traits<TimeRangedSpacedData>::spaced_data);
}



} // namespace myrmidon
} // namespace fort
