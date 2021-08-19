#include <gtest/gtest.h>

#include <fort/myrmidon/Shapes.hpp>

namespace fort {
namespace myrmidon {

class ShapesUTest : public ::testing::Test {};


TEST_F(ShapesUTest,Type) {
	auto circle = std::make_unique<Circle>(Eigen::Vector2d(0,0),0);
	auto capsule = std::make_unique<Capsule>(Eigen::Vector2d(0,0),Eigen::Vector2d(0,0),0,0);
	auto polygon = std::make_unique<Polygon>(Polygon({{0,0},{0,0},{0,0}}));

	EXPECT_EQ(circle->ShapeType(),fort::myrmidon::Shape::Type::CIRCLE);
	EXPECT_EQ(capsule->ShapeType(),fort::myrmidon::Shape::Type::CAPSULE);
	EXPECT_EQ(polygon->ShapeType(),fort::myrmidon::Shape::Type::POLYGON);
}



TEST_F(ShapesUTest,CirclePointCollision) {
	struct TestData {
		double X,Y;
		Circle C;
		bool Expected;
	};

	std::vector<TestData> testdata =
		{
		 {
		  0,0,
		  Circle({0,0},1e-6),
		  true,
		 },
		 {
		  0,1-1e-6,
		  Circle({0,0},1),
		  true,
		 },
		 {
		  std::sqrt(2)/2,std::sqrt(2)/2,
		  Circle({0,0},1),
		  false,
		 },
		 {
		  std::sqrt(2)/2-1.0e-6,std::sqrt(2)/2-1.0e-6,
		  Circle({0,0},1),
		  true,
		 },
		};

	for ( const auto & d : testdata ) {
		EXPECT_EQ(d.C.Contains(Eigen::Vector2d(d.X,d.Y)),d.Expected);
	}

}


TEST_F(ShapesUTest,CapsulePointCollision) {
	Capsule capsule(Eigen::Vector2d(0,0),Eigen::Vector2d(0,1),1.0,0.01);

	struct TestData {
		double X,Y;
		bool   Expected;
	};

	std::vector<TestData> testdata =
		{
		 {0,0,true},
		 {0,1,true},
		 {1,0,true},
		 {0.5-1.0e-6,0.5-1.0e-6,true},
		 {0.1,1,false},
		};
	for ( const auto & d : testdata ) {
		EXPECT_EQ(capsule.Contains(Eigen::Vector2d(d.X,d.Y)),d.Expected)
			<< "Testing (" << d.X << "," << d.Y << ")";
	}

}

TEST_F(ShapesUTest,TestCaspuleCollision) {
	struct TestData {
		double aC1X,aC1Y,aC2X,aC2Y,aR1,aR2;
		double bC1X,bC1Y,bC2X,bC2Y,bR1,bR2;
		bool Expected;
	};


	std::vector<TestData> testdata =
		{
		 //Toy example with square positions
		 {
		  0,0,0,1,0.25,0.25,
		  1,0,1,1,0.25,0.25,
		  false,
		 },
		 {
		  0,0,0,1,0.6,0.6,
		  1,0,1,1,0.6,0.6,
		  true,
		 },
		 {
		  0,0,0,1,0.55,0.35,
		  1,0,1,1,0.35,0.55,
		  false,
		 },
		 {
		  0,0,0,1,0.35,0.55,
		  1,0,1,1,0.55,0.35,
		  false,
		 },
		 {
		  0,0,0,1,0.35,0.55,
		  1,0,1,1,0.35,0.55,
		  true,
		 },
		 {
		  0,0,0,1,0.55,0.35,
		  1,0,1,1,0.55,0.35,
		  true,
		 },
		 // More complicated example, where intersection is not on the
		 // minimal distance between segments
		 {
		  0,0,0,1,0.3,0.7,
		  1,0.1,1.2,1.2,0.3,0.7,
		  true,
		 },
		 //Another edge case found by playing with real shapes
		 {
		  0.00,0.00,0.00,1.00,0.02,0.30,
		  0.30,0.00,0.60,0.90,0.02,0.33,
		  true
		 },
		};

	for ( const auto & d : testdata ) {
		Capsule a(Eigen::Vector2d(d.aC1X,d.aC1Y),
		          Eigen::Vector2d(d.aC2X,d.aC2Y),
		          d.aR1,
		          d.aR2);

		Capsule b(Eigen::Vector2d(d.bC1X,d.bC1Y),
		          Eigen::Vector2d(d.bC2X,d.bC2Y),
		          d.bR1,
		          d.bR2);

		bool res = Capsule::Intersect(a.C1(),a.C2(),a.R1(),a.R2(),
		                              b.C1(),b.C2(),b.R1(),b.R2());
		EXPECT_EQ(res,d.Expected) << " Intersecting " << a << " and " << b;
	}
}

TEST_F(ShapesUTest,PolygonPointCollision) {
	struct TestData {
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		Eigen::Vector2d Point;
		Polygon         Poly;
		bool            Expected;
	};

	std::vector<TestData> testdata =
		{
		 // with a square
		 {
		  Eigen::Vector2d({0,0}),
		  Polygon({{-1,-1},{1,-1},{1,1},{-1,1}}),
		  true,
		 },
		 {
		  Eigen::Vector2d({1,2}),
		  Polygon({{-1,-1},{1,-1},{1,1},{-1,1}}),
		  false,
		 },
		 {
		  Eigen::Vector2d({1,-2}),
		  Polygon({{-1,-1},{1,-1},{1,1},{-1,1}}),
		  false,
		 },
		 {
		  Eigen::Vector2d({0.5,1-1.0e-6}),
		  Polygon({{-1,-1},{1,-1},{1,1},{-1,1}}),
		  true,
		 },
		 // with a extinction rebellion shape
		 {
		  // the center of the shape is now outside
		  Eigen::Vector2d({0.0,0.0}),
		  Polygon({{-1,-1},{1,-1},{-1,1},{1,1}}),
		  false,
		 },
		 {
		  // just above the center is inside
		  Eigen::Vector2d({0.0,1.0e-6}),
		  Polygon({{-1,-1},{1,-1},{-1,1},{1,1}}),
		  true,
		 },
		 {
		  // just right the center is outside
		  Eigen::Vector2d({1.0e-6,0.0}),
		  Polygon({{-1,-1},{1,-1},{-1,1},{1,1}}),
		  false,
		 },

		};

	for ( const auto & d : testdata ) {
		EXPECT_EQ(d.Poly.Contains(d.Point),d.Expected);
	}
}


} // namespace myrmidon
} // namespace fort
