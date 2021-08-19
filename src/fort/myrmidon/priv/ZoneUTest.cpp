#include <gtest/gtest.h>

#include "Zone.hpp"

#include <fort/myrmidon/UtilsUTest.hpp>

namespace fort {
namespace myrmidon {
namespace priv {


class ZoneUTest : public ::testing::Test {
protected:
	void SetUp() {
		zone = Zone::Create(1,"foo","bar");
		shapes.emplace_back(std::unique_ptr<Shape>(new Circle(Eigen::Vector2d(0,0),2.0)));
		shapes.emplace_back(std::unique_ptr<Shape>(new Polygon(Vector2dList({{-1,-1},{1,-1},{1,1},{-1,1}}))));
	}

	void TearDown() {
		zone.reset();
		shapes.clear();
	}

	std::vector<Shape::Ptr> shapes;
	Zone::Ptr               zone;

};



TEST_F(ZoneUTest,GeometryHaveAABB) {

	Zone::Geometry g(std::move(shapes));
	EXPECT_AABB_EQ(g.GlobalAABB(),shapes.front()->ComputeAABB());
	ASSERT_EQ(shapes.size(), g.IndividualAABB().size());
	for( int i = 0; i < shapes.size(); ++i) {
		EXPECT_AABB_EQ(shapes[i]->ComputeAABB(),
		               g.IndividualAABB()[i]);
	}
}

TEST_F(ZoneUTest,DefinitionOwnsGeometry) {
	auto definition = zone->AddDefinition({},
	                                      Time::SinceEver(),
	                                      Time::Forever());

	// Even if we pass an empty list, geometry is valid
	EXPECT_NO_THROW({ZoneGeometry(definition->Shapes());});

	EXPECT_TRUE(definition->Shapes().empty());

	definition->SetShapes(shapes);
	ASSERT_EQ(definition->Shapes(),
	          shapes);
}

TEST_F(ZoneUTest,ZoneCanBeRenamed) {
	EXPECT_NO_THROW({
			zone->SetName("foo");
			zone->SetName("bar");
			zone->SetName("");
		});
}

TEST_F(ZoneUTest,DefinitionAreTimeValidObject) {

	auto start = Time::SinceEver();
	auto end = Time::Forever();
	EXPECT_TRUE(zone->NextFreeTimeRegion(start,end));
	EXPECT_TIME_EQ(start,Time::SinceEver());
	EXPECT_TIME_EQ(end,Time::Forever());


	auto definition = zone->AddDefinition(shapes,Time::SinceEver(),Time::Forever());

	EXPECT_THROW({
			zone->AddDefinition(shapes,
			                    Time::FromTimeT(0),
			                    Time::Forever());
		},std::runtime_error);

	definition->SetStart(Time::FromTimeT(1));

	EXPECT_THROW({
			definition->SetEnd(Time::FromTimeT(0));
		},std::invalid_argument);

	EXPECT_NO_THROW({
			definition->SetEnd(Time::FromTimeT(2));
		});


	EXPECT_NO_THROW({
			zone->AddDefinition(shapes,
			                    Time::FromTimeT(3),
			                    Time::FromTimeT(4));
		});


	EXPECT_TRUE(zone->NextFreeTimeRegion(start,end));
	EXPECT_TIME_EQ(start,Time::SinceEver());
	EXPECT_TIME_EQ(end,Time::FromTimeT(1));
	EXPECT_NO_THROW({
			zone->AddDefinition(shapes,start,end);
		});

	EXPECT_TRUE(zone->NextFreeTimeRegion(start,end));
	EXPECT_TIME_EQ(start,Time::FromTimeT(2));
	EXPECT_TIME_EQ(end,Time::FromTimeT(3));
	EXPECT_NO_THROW({
			zone->AddDefinition(shapes,start,end);
		});
	EXPECT_TRUE(zone->NextFreeTimeRegion(start,end));
	EXPECT_TIME_EQ(start,Time::FromTimeT(4));
	EXPECT_TIME_EQ(end,Time::Forever());
	EXPECT_NO_THROW({
			zone->AddDefinition(shapes,start,end);
		});
	EXPECT_FALSE(zone->NextFreeTimeRegion(start,end));



}




} // namespace priv
} // namespace myrmidon
} // namespace fort
