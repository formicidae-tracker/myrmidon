#include <gtest/gtest.h>

#include "Experiment.hpp"
#include "Query.hpp"

#include "Video.hpp"

#include "TestSetup.hpp"
#include "UtilsUTest.hpp"

#include <fort/myrmidon/utest-data/UTestData.hpp>

#include <fort/video/Frame.hpp>

#define failure_helper(aExpr, bExpr, a, b, field)                              \
	::testing::AssertionFailure()                                              \
	    << "Value of: " << aExpr << "." << #field << std::endl                 \
	    << "  Actual: " << a.field << std::endl                                \
	    << "Expected: " << bExpr << "." << #field << std::endl                 \
	    << "Which is: " << b.field

#define check(aExpr, bExpr, a, b, field)                                       \
	do {                                                                       \
		if (a.field != b.field) {                                              \
			return failure_helper(aExpr, bExpr, a, b, field);                  \
		}                                                                      \
	} while (0)

#define check_presence(aExpr, bExpr, a, b, field)                              \
	do {                                                                       \
		if ((!b.field) != (!a.field)) {                                        \
			return ::testing::AssertionFailure()                               \
			       << "Value of: " << aExpr << "." << #field << " != nullptr " \
			       << std::endl                                                \
			       << "  Actual: " << std::boolalpha << bool(a.field)          \
			       << std::endl                                                \
			       << "Expected: " << bool(b.field);                           \
		}                                                                      \
	} while (0)

::testing::AssertionResult AssertVideoFrameDataEqual(
    const char                           *aExpr,
    const char                           *bExpr,
    const fort::myrmidon::VideoFrameData &a,
    const fort::myrmidon::VideoFrameData &b
) {
	check(aExpr, bExpr, a, b, Position);
	auto tmp = AssertTimeEqual(
	    (std::string(aExpr) + ".Time").c_str(),
	    (std::string(bExpr) + ".Time").c_str(),
	    a.Time,
	    b.Time
	);
	if (!tmp) {
		return tmp;
	}
	check_presence(aExpr, bExpr, a, b, Identified);
	if (b.Identified) {
		tmp = AssertIdentifiedFrameEqual(
		    ("*" + std::string(aExpr) + ".Identified").c_str(),
		    ("*" + std::string(bExpr) + ".Identified").c_str(),
		    *a.Identified,
		    *b.Identified
		);
		if (!tmp) {
			return tmp;
		}
	}
	check_presence(aExpr, bExpr, a, b, Collided);
	if (b.Collided) {
		tmp = AssertCollisionFrameEqual(
		    ("*" + std::string(aExpr) + ".Collided").c_str(),
		    ("*" + std::string(bExpr) + ".Collided").c_str(),
		    *a.Collided,
		    *b.Collided
		);
		if (!tmp) {
			return tmp;
		}
	}

	check(aExpr, bExpr, a, b, Trajectories.size());
	for (size_t i = 0; i < b.Trajectories.size(); ++i) {
		tmp = AssertAntTrajectoryEqual(
		    ("*" + std::string(aExpr) + ".Trajectories[" + std::to_string(i) +
		     "]")
		        .c_str(),
		    ("*" + std::string(bExpr) + ".Trajectories[" + std::to_string(i) +
		     "]")
		        .c_str(),
		    *a.Trajectories[i],
		    *b.Trajectories[i]
		);
		if (!tmp) {
			return tmp;
		}
	}

	check(aExpr, bExpr, a, b, Interactions.size());

	for (size_t i = 0; i < b.Interactions.size(); ++i) {
		tmp = AssertAntInteractionEqual(
		    ("*" + std::string(aExpr) + ".Interactions[" + std::to_string(i) +
		     "]")
		        .c_str(),
		    ("*" + std::string(bExpr) + ".Interactions[" + std::to_string(i) +
		     "]")
		        .c_str(),
		    *a.Interactions[i],
		    *b.Interactions[i]
		);
		if (!tmp) {
			return tmp;
		}
	}

	return ::testing::AssertionSuccess();
}

::testing::AssertionResult AssertVideoSegmentEqual(const char * aExpr,
                                                   const char * bExpr,
                                                   const fort::myrmidon::VideoSegment & a,
                                                   const fort::myrmidon::VideoSegment & b) {
	check(aExpr,bExpr,a,b,Space);
	check(aExpr,bExpr,a,b,AbsoluteFilePath);
	check(aExpr,bExpr,a,b,Begin);
	check(aExpr,bExpr,a,b,End);
	check(aExpr,bExpr,a,b,Data.size());
	for ( size_t i = 0; i < a.Data.size(); ++i ) {
		auto tmp = AssertVideoFrameDataEqual((std::string(aExpr) + ".Data[" + std::to_string(i) + "]").c_str(),
		                                     (std::string(bExpr) + ".Data[" + std::to_string(i) + "]").c_str(),
		                                     a.Data[i],
		                                     b.Data[i]);
		if ( !tmp ) {
			return tmp;
		}
	}
	return ::testing::AssertionSuccess();
}

namespace fort {
namespace myrmidon {


class VideoUTest : public ::testing::Test {
protected:
	void SetUp() {
		experiment = Experiment::Open(TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath);
	}

	Experiment::Ptr experiment;
};

TEST_F(VideoUTest,MatchDataEdgeCases) {
	std::vector<IdentifiedFrame::Ptr> data;
	EXPECT_NO_THROW({
			VideoSegment::List segments;
			VideoSegment::Match(segments,data.begin(),data.end());
		});

	VideoSegment::List segments =
		{
		 {.Space = 1},
		 {.Space = 2},
		};


	EXPECT_THROW({
			VideoSegment::Match(segments,
			                    data.begin(),
			                    data.end());
		},std::invalid_argument);
};

TEST_F(VideoUTest, ForEachFramesEdgeCases) {
	const auto &expected = TestSetup::UTestData().ExpectedResults().front();
	auto        segments = expected.VideoSegments.at(1);
	auto       &segment  = segments.front();
	segment.Data.resize(segment.Data.size() - 1);
	segment.Data.push_back({.Position = segment.End, .Time = Time::SinceEver()}
	);
	segment.End += 2;

	VideoSequence::ForEach(
	    segments,
	    [&](const video::Frame &frame, const VideoFrameData &d) {
		    auto fi = std::find_if(
		        segment.Data.begin(),
		        segment.Data.end(),
		        [&](const VideoFrameData &it) {
			        return it.Position == d.Position;
		        }
		    );
		    if (fi == segment.Data.end()) {
			    EXPECT_PRED_FORMAT2(
			        AssertVideoFrameDataEqual,
			        d,
			        (VideoFrameData{
			            .Position = d.Position,
			            .Time     = Time::SinceEver()})
			    );
		    } else {
			    EXPECT_PRED_FORMAT2(AssertVideoFrameDataEqual, d, *fi);
		    }
	    }
	);
};

TEST_F(VideoUTest, EndToEnd) {
	const auto &expected = TestSetup::UTestData().ExpectedResults().front();
	const auto &frames   = TestSetup::UTestData().ExpectedFrames();
	std::vector<VideoSegment> segments;

	Query::FindVideoSegments(
	    *experiment,
	    segments,
	    1,
	    expected.Start,
	    expected.End
	);

	VideoSegment::Match(segments, frames.begin(), frames.end());

	VideoSegment::Match(
	    segments,
	    expected.Trajectories.begin(),
	    expected.Trajectories.end()
	);

	VideoSegment::Match(
	    segments,
	    expected.Interactions.begin(),
	    expected.Interactions.end()
	);

	ASSERT_EQ(segments.size(), expected.VideoSegments.at(1).size());

	for (size_t i = 0; i < expected.VideoSegments.at(1).size(); ++i) {
		EXPECT_PRED_FORMAT2(
		    AssertVideoSegmentEqual,
		    segments[i],
		    expected.VideoSegments.at(1)[i]
		) << "  With i: "
		  << i;
	}

	ASSERT_EQ(segments.size(), 1);

	auto iter = segments.front().Data.begin();

	VideoSequence::ForEach(
	    segments,
	    [&](const video::Frame &frame, const VideoFrameData &d) {
		    ASSERT_TRUE(iter != segments.front().Data.end());
		    EXPECT_TRUE(frame.Planes[0] != nullptr);
		    EXPECT_PRED_FORMAT2(AssertVideoFrameDataEqual, d, *iter);
		    ++iter;
	    }
	);

	Query::FindVideoSegments(
	    *experiment,
	    segments,
	    3,
	    expected.Start,
	    expected.End
	);
	EXPECT_TRUE(segments.empty());

	Query::FindVideoSegments(
	    *experiment,
	    segments,
	    2,
	    expected.Start,
	    expected.End
	);
	EXPECT_TRUE(segments.empty());
}

} // namespace myrmidon
} // namespace fort
