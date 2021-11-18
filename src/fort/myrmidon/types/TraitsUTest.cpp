#include <gtest/gtest.h>

#include "Traits.hpp"
#include <fort/myrmidon/UtilsUTest.hpp>

#include "IdentifiedFrame.hpp"
#include "Collision.hpp"

namespace fort {
namespace myrmidon {

class DataTraitsUTest : public ::testing::Test {};


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


} // namespace myrmidon
} // namespace fort
