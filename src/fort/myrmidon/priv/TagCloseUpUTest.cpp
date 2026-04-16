#include <gtest/gtest.h>

#include "TagCloseUp.hpp"
#include "fort/myrmidon/types/CloseUp.hpp"
#include "fort/myrmidon/types/Typedefs.hpp"

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/UtilsUTest.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

class TagCloseUpUTest : public ::testing::Test {};

TEST_F(TagCloseUpUTest, CanBeFormatted) {
	struct TestData {
		fs::path      Path;
		priv::FrameID FrameID;
		priv::TagID   TagID;
		std::string   Expected;
	};

	std::vector<TestData> data = {
	    {"", 0, 0, "/frames/0/closeups/0x000"},
	    {"", 2134, 34, "/frames/2134/closeups/0x022"},
	    {"foo", 42, 43, "foo/frames/42/closeups/0x02b"},
	    {"foo/bar/baz", 42, 56, "foo/bar/baz/frames/42/closeups/0x038"},
	};

	if (fs::path::preferred_separator == '\\') {
		data.push_back(
		    {"foo\bar\baz", 42, 103, "foo/bar/baz/frames/42/closeups/0x067"}
		);
	}

	Eigen::Matrix<double, 2, 4> corners;
	corners.setConstant(0.0);

	for (const auto &d : data) {
		FrameReference a(
		    d.Path.generic_string(),
		    d.FrameID,
		    fort::Time::FromTimeT(0)
		);
		TagCloseUp t(
		    TestSetup::UTestData().Basedir() / "foo",
		    a,
		    d.TagID,
		    std::vector<TagDetection>{TagDetection{
		        .ID       = d.TagID,
		        .Position = Eigen::Vector2d::Zero(),
		        .Angle    = 0.0,
		        .Corners  = corners,
		    }}
		);
		fs::path expectedParentPath(
		    d.Path.generic_string().empty() ? "/" : d.Path
		);
		std::ostringstream os;
		os << t;
		EXPECT_EQ(os.str(), d.Expected);
		auto expectedURI = expectedParentPath / "frames" /
		                   std::to_string(d.FrameID) / "closeups" /
		                   FormatTagID(d.TagID);
		EXPECT_EQ(t.URI(), expectedURI);
	}
}

TEST_F(TagCloseUpUTest, CanComputeGeometricValues) {
	Vector2dList corners = {
	    Eigen::Vector2d(1.0, 1.0),
	    Eigen::Vector2d(2.0, 1.0),
	    Eigen::Vector2d(2.0, 2.0),
	    Eigen::Vector2d(1.0, 2.0)
	};

	Eigen::Vector2d center =
	    (corners[0] + corners[1] + corners[2] + corners[3]) / 4.0;
	std::vector<double> angles = {
	    0.0,
	    M_PI / 3.0,
	    M_PI / 5.0,
	    -3.0 * M_PI / 4,
	};

	for (const auto &a : angles) {
		Isometry2Dd                 trans(a, Eigen::Vector2d(2.0, 1.0));
		Eigen::Matrix<double, 2, 4> transCorners;

		transCorners.col(0) = trans * corners[0];
		transCorners.col(1) = trans * corners[1];
		transCorners.col(2) = trans * corners[2];
		transCorners.col(3) = trans * corners[3];

		double res = TagDetection::ComputeAngleFromCorners(transCorners);
		EXPECT_DOUBLE_EQ(res, a);

		auto tcu = TagCloseUp(
		    TestSetup::UTestData().Basedir() / "foo.png",
		    FrameReference("", 0, Time()),
		    0,
		    std::vector<TagDetection>{TagDetection{
		        .ID       = 0,
		        .Position = trans * center,
		        .Angle    = res,
		        .Corners  = transCorners
		    }}
		);

		EXPECT_DOUBLE_EQ(tcu.TagSizePx(), 1.0);
		EXPECT_DOUBLE_EQ(tcu.Squareness(), 1.0);

		auto expectedImageToTag = Isometry2Dd(a, trans * center).inverse();
		EXPECT_DOUBLE_EQ(expectedImageToTag.angle(), tcu.ImageToTag().angle());
		EXPECT_VECTOR2D_EQ(
		    expectedImageToTag.translation(),
		    tcu.ImageToTag().translation()
		);
	}
}

TEST_F(TagCloseUpUTest, ComputesSquareness) {
	struct TestData {
		Eigen::Matrix<double, 2, 4> Corners;
		double                      Expected;
	};

	static auto build = [](const Vector2dList &corners) {
		Eigen::Matrix<double, 2, 4> res;
		for (size_t i = 0; i < 4; ++i) {
			res.col(i) = corners[i];
		}
		return res;
	};

	std::vector<TestData> testdata = {
	    {
	        build({{1, 1}, {1, -1}, {-1, -1}, {-1, 1}}),
	        1.0,
	    },
	    // this is a triangle, it is not square
	    {
	        build({{1, 1}, {1, -1}, {-1, -1}, {1, 1}}),
	        0.0,
	    },
	    {
	        build({{11, 12}, {9, -11}, {-11, -10}, {-12, 9}}),
	        0.8622,
	    },
	};

	for (const auto &d : testdata) {
		auto tcu = TagCloseUp(
		    TestSetup::UTestData().Basedir() / "foo.png",
		    FrameReference("", 0, Time()),
		    0,
		    std::vector<TagDetection>{TagDetection{
		        .ID       = 0,
		        .Position = Eigen::Vector2d(0, 0),
		        .Angle    = 0,
		        .Corners  = d.Corners,
		    }}
		);
		EXPECT_NEAR(tcu.Squareness(), d.Expected, 1.0e-3);
	}
}

TEST_F(TagCloseUpUTest, ClassInvariants) {
	EXPECT_THROW(
	    {
		    // not an absolute path
		    TagCloseUp(
		        "foo",
		        FrameReference(),
		        0,
		        std::vector<TagDetection>{TagDetection{.ID = 0, .Angle = 0.0}}
		    );
	    },
	    cpptrace::invalid_argument
	);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
