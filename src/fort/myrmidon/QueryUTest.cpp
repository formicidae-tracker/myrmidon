#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <filesystem>
#include <gtest/gtest.h>

#include "Ant.hpp"
#include "Experiment.hpp"
#include "Query.hpp"

#include "TestSetup.hpp"
#include "UtilsUTest.hpp"
#include "fort/myrmidon/types/IdentifiedFrame.hpp"
#include "fort/myrmidon/types/Reporter.hpp"

#include <fort/myrmidon/utest-data/UTestData.hpp>

#include <gmock/gmock.h>
#include <memory>

namespace fort {
namespace myrmidon {

class MockTimeProgressReporter : public TimeProgressReporter {
public:
	MOCK_METHOD(void, ReportError, (const std::string &error), (override));
	MOCK_METHOD(void, Update, (const fort::Time &time), (override));
	MOCK_METHOD(
	    void,
	    SetBound,
	    (const fort::Time &start, const fort::Time &end),
	    (override)
	);
};

class QueryUTest : public ::testing::Test {
protected:
	Experiment::Ptr experiment;
	void            SetUp();
	void            TearDown();
};

void QueryUTest::SetUp() {
	ASSERT_NO_THROW({
		experiment = Experiment::Open(
		    TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath
		);
	});
}

void QueryUTest::TearDown() {
	experiment.reset();
}

TEST_F(QueryUTest, TagStatistics) {
	TagStatistics::ByTagID tagStats;
	ASSERT_NO_THROW({ tagStats = Query::ComputeTagStatistics(*experiment); });
	EXPECT_TAG_STATISTICS_EQ(
	    tagStats,
	    TestSetup::UTestData().ExpectedTagStatistics()
	);
}

TEST_F(QueryUTest, IdentifyFrames) {
	auto progress =
	    std::make_unique<testing::StrictMock<MockTimeProgressReporter>>();

	std::vector<IdentifiedFrame::Ptr> identifieds;
	const auto &expected = TestSetup::UTestData().ExpectedFrames();

	{
		::testing::InSequence seq;
		EXPECT_CALL(
		    *progress,
		    SetBound(
		        expected.front().first->FrameTime,
		        expected.back().first->FrameTime.Add(1)
		    )
		)
		    .Times(1);

		auto time = fort::Time::SinceEver();
		for (const auto &e : expected) {
			if (time.Before(e.first->FrameTime)) {
				time = e.first->FrameTime;
				EXPECT_CALL(*progress, Update(time)).Times(1);
			}
		}
	}

	ASSERT_NO_THROW({
		myrmidon::Query::IdentifyFramesArgs args;
		args.Progress = std::move(progress);
		Query::IdentifyFramesFunctor(
		    *experiment,
		    [&identifieds](const IdentifiedFrame::Ptr &i) {
			    identifieds.push_back(i);
		    },
		    args
		);
	});
	ASSERT_EQ(identifieds.size(), expected.size());
	for (size_t i = 0; i < std::min(identifieds.size(), expected.size()); ++i) {
		EXPECT_IDENTIFIED_FRAME_EQ(*identifieds[i], *expected[i].first)
		    << "  With i: " << i;
	}
	auto   t = TestSetup::UTestData().NestDataDirs().front().End;
	size_t expectedNumber =
	    std::find_if(
	        expected.begin(),
	        expected.end(),
	        [&t](const std::pair<IdentifiedFrame::Ptr, CollisionFrame::Ptr> &it
	        ) { return it.first->FrameTime > t; }
	    ) -
	    expected.begin();
	identifieds.clear();
	ASSERT_NO_THROW({
		myrmidon::Query::IdentifyFramesArgs args;
		args.End = t;
		Query::IdentifyFrames(*experiment, identifieds, args);
	});
	EXPECT_EQ(identifieds.size(), expectedNumber);

	identifieds.clear();
	try {
		myrmidon::Query::IdentifyFramesArgs args;
		args.Start = t;
		Query::IdentifyFramesFunctor(
		    *experiment,
		    [&identifieds](const IdentifiedFrame::Ptr &i) {
			    identifieds.push_back(i);
		    },
		    args
		);
	} catch (const std::exception &e) {
		ADD_FAILURE() << "Unexpected exception: " << e.what();
		return;
	}
	// we just have removed the first frame
	EXPECT_EQ(identifieds.size(), expected.size() - expectedNumber);
}

TEST_F(QueryUTest, CollideFrames) {
	auto progress =
	    std::make_unique<testing::StrictMock<MockTimeProgressReporter>>();

	const auto &expected = TestSetup::UTestData().ExpectedFrames();

	{
		::testing::InSequence seq;
		EXPECT_CALL(
		    *progress,
		    SetBound(
		        expected.front().first->FrameTime,
		        expected.back().first->FrameTime.Add(1)
		    )
		)
		    .Times(1);

		auto time = fort::Time::SinceEver();
		for (const auto &e : expected) {
			if (time.Before(e.first->FrameTime)) {
				time = e.first->FrameTime;
				EXPECT_CALL(*progress, Update(time)).Times(1);
			}
		}
	}

	std::vector<CollisionData> collisionData;

	ASSERT_NO_THROW({
		myrmidon::Query::CollideFramesArgs args;
		args.Progress = std::move(progress);
		Query::CollideFramesFunctor(
		    *experiment,
		    [&collisionData](const CollisionData &data) {
			    collisionData.push_back(data);
		    },
		    args
		);
	});

	ASSERT_EQ(collisionData.size(), expected.size());
	for (size_t i = 0; i < std::min(collisionData.size(), expected.size());
	     ++i) {
		EXPECT_IDENTIFIED_FRAME_EQ(*collisionData[i].first, *expected[i].first)
		    << "  With i: " << i;
		EXPECT_COLLISION_FRAME_EQ(*collisionData[i].second, *expected[i].second)
		    << "  With i: " << i;
	}
}

TEST_F(QueryUTest, ComputeAntTrajectories) {
	using ::testing::_;
	size_t i = 0;
	for (const auto &expected : TestSetup::UTestData().ExpectedResults()) {

		auto progress =
		    std::make_unique<testing::NiceMock<MockTimeProgressReporter>>();

		EXPECT_CALL(*progress, SetBound(_, _)).Times(1);

		EXPECT_CALL(*progress, Update(_)).Times(testing::AtLeast(2));

		std::vector<AntTrajectory::Ptr> trajectories;

		ASSERT_NO_THROW({
			myrmidon::Query::ComputeAntTrajectoriesArgs args;
			args.Start      = expected.Start;
			args.End        = expected.End;
			args.MaximumGap = expected.MaximumGap;
			args.Matcher    = expected.Matches;
			args.Progress   = std::move(progress);
			if (++i % 2 == 0) {
				Query::ComputeAntTrajectoriesFunctor(
				    *experiment,
				    [&trajectories](const AntTrajectory::Ptr &t) {
					    trajectories.push_back(t);
				    },
				    args
				);
			} else {
				Query::ComputeAntTrajectories(*experiment, trajectories, args);
			}
		});

		EXPECT_EQ(trajectories.size(), expected.Trajectories.size());
		// trajectories, due to TDD boundaries may not be sorted
		std::sort(
		    trajectories.begin(),
		    trajectories.end(),
		    [](const AntTrajectory::Ptr &a, const AntTrajectory::Ptr &b) {
			    if (a->End() == b->End()) {
				    return a->Space < b->Space;
			    }
			    return a->End() < b->End();
		    }
		);

		for (size_t i = 0;
		     i < std::min(trajectories.size(), expected.Trajectories.size());
		     ++i) {
			EXPECT_ANT_TRAJECTORY_EQ(
			    *trajectories[i],
			    *expected.Trajectories[i]
			) << "  With i: "
			  << i;
		}
	}
}

TEST_F(QueryUTest, ComputeAntInteractions) {
	size_t i = 0;
	using ::testing::_;
	for (const auto &expected : TestSetup::UTestData().ExpectedResults()) {

		auto progress =
		    std::make_unique<testing::NiceMock<MockTimeProgressReporter>>();

		EXPECT_CALL(*progress, SetBound(_, _)).Times(1);

		EXPECT_CALL(*progress, Update(_)).Times(testing::AtLeast(2));

		std::vector<AntTrajectory::Ptr>  trajectories;
		std::vector<AntInteraction::Ptr> interactions;
		ASSERT_NO_THROW({
			myrmidon::Query::ComputeAntInteractionsArgs args;
			args.Start      = expected.Start;
			args.End        = expected.End;
			args.MaximumGap = expected.MaximumGap;
			args.Matcher    = expected.Matches;
			args.Progress   = std::move(progress);
			if (++i % 2 == 0) {
				Query::ComputeAntInteractionsFunctor(
				    *experiment,
				    [&trajectories](const AntTrajectory::Ptr &t) {
					    trajectories.push_back(t);
				    },
				    [&interactions](const AntInteraction::Ptr &i) {
					    interactions.push_back(i);
				    },
				    args
				);
			} else {
				Query::ComputeAntInteractions(
				    *experiment,
				    trajectories,
				    interactions,
				    args
				);
			}
		});

		EXPECT_EQ(trajectories.size(), expected.InteractionTrajectories.size());
		EXPECT_EQ(interactions.size(), expected.Interactions.size());

		// trajectories, due to TDD boundaries may not be sorted
		std::sort(
		    trajectories.begin(),
		    trajectories.end(),
		    [](const AntTrajectory::Ptr &a, const AntTrajectory::Ptr &b) {
			    if (a->End() == b->End()) {
				    return a->Space < b->Space;
			    }
			    return a->End() < b->End();
		    }
		);

		for (size_t i = 0; i < std::min(
		                           trajectories.size(),
		                           expected.InteractionTrajectories.size()
		                       );
		     ++i) {
			EXPECT_ANT_TRAJECTORY_EQ(
			    *trajectories[i],
			    *expected.InteractionTrajectories[i]
			) << "  With i: "
			  << i;
		}

		for (size_t i = 0; i < std::min(
		                           expected.Interactions.size(),
		                           expected.Interactions.size()
		                       );
		     ++i) {
			EXPECT_ANT_INTERACTION_EQ(
			    *interactions[i],
			    *expected.Interactions[i]
			) << "  With i: "
			  << i << std::endl
			  << "Expected Interaction End: "
			  << ::testing::PrintToString(expected.Interactions[i]->End)
			  << std::endl
			  << "  Actual Interaction End: "
			  << ::testing::PrintToString(interactions[i]->End) << std::endl
			  << "    Expected Segment End: "
			  << ::testing::PrintToString(
			         std::get<0>(expected.Interactions[i]->Trajectories)
			             .first.EndTime()
			     )
			  << std::endl
			  << "      Actual Segment End: "
			  << ::testing::PrintToString(
			         std::get<0>(interactions[i]->Trajectories).first.EndTime()
			     )
			  << std::endl;
		}
	}
}

TEST_F(QueryUTest, ComputeAntInteractionsSummarized) {
	size_t i = 0;
	for (const auto &expected : TestSetup::UTestData().ExpectedResults()) {
		auto expectedInteractions = expected.Summarized();
		std::vector<AntTrajectory::Ptr>  trajectories;
		std::vector<AntInteraction::Ptr> interactions;

		ASSERT_NO_THROW({
			myrmidon::Query::ComputeAntInteractionsArgs args;
			args.Start                  = expected.Start;
			args.End                    = expected.End;
			args.MaximumGap             = expected.MaximumGap;
			args.Matcher                = expected.Matches;
			args.ReportFullTrajectories = false;
			Query::ComputeAntInteractions(
			    *experiment,
			    trajectories,
			    interactions,
			    args
			);
		});

		EXPECT_EQ(trajectories.size(), 0);
		EXPECT_EQ(interactions.size(), expectedInteractions.size());

		for (size_t i = 0; i < std::min(
		                           expected.Interactions.size(),
		                           expectedInteractions.size()
		                       );
		     ++i) {
			EXPECT_ANT_INTERACTION_EQ(
			    *interactions[i],
			    *expectedInteractions[i]
			) << "  With i: "
			  << i << std::endl;
		}
	}
}

TEST_F(QueryUTest, FrameSelection) {
	auto firstDate = std::min(
	    TestSetup::UTestData().NestDataDirs().front().Start,
	    TestSetup::UTestData().ForagingDataDirs().front().Start
	);

	std::vector<IdentifiedFrame::Ptr> frames;

	myrmidon::Query::IdentifyFramesArgs args;

	// issue 138, should select all frames
	args.Start = firstDate;
	Query::IdentifyFramesFunctor(
	    *experiment,
	    [&frames](const IdentifiedFrame::Ptr &f) { frames.push_back(f); },
	    args
	);

	EXPECT_EQ(frames.size(), TestSetup::UTestData().ExpectedFrames().size());
	frames.clear();

	// selects the first frame
	args.Start = firstDate;
	args.End   = firstDate.Add(1);
	Query::IdentifyFramesFunctor(
	    *experiment,
	    [&frames](const IdentifiedFrame::Ptr &f) { frames.push_back(f); },
	    args
	);

	ASSERT_TRUE(frames.size() > 0);
	ASSERT_TRUE(frames.size() <= 2);

	EXPECT_EQ(frames[0]->FrameTime, firstDate);
	if (frames.size() > 1) {
		EXPECT_EQ(frames[1]->FrameTime, firstDate);
		EXPECT_TRUE(frames[1]->Space != frames[0]->Space);
	}

	frames.clear();
	// won't access any
	args.Start = firstDate;
	args.End   = firstDate;
	Query::IdentifyFramesFunctor(
	    *experiment,
	    [&frames](const IdentifiedFrame::Ptr &f) { frames.push_back(f); },
	    args
	);

	ASSERT_EQ(frames.size(), 0);
}

TEST_F(QueryUTest, GetMetaDataKeyRanges) {
	experiment =
	    Experiment::Create(TestSetup::UTestData().Basedir() / "foo.myrmidon");
	experiment->SetMetaDataKey("alive", true);
	auto a = experiment->CreateAnt();
	a      = experiment->CreateAnt();
	a->SetValue("alive", false, Time());
	a = experiment->CreateAnt();
	a->SetValue("alive", false, Time::SinceEver());
	a->SetValue("alive", true, Time());
	a->SetValue("alive", true, Time().Add(1));
	a->SetValue("alive", false, Time().Add(2));
	a->SetValue("alive", true, Time().Add(3));

	std::vector<std::tuple<AntID, Time, Time>> ranges;
	EXPECT_NO_THROW({
		ranges = Query::GetMetaDataKeyRanges(*experiment, "alive", true);
	});
	EXPECT_EQ(ranges.size(), 4);
	EXPECT_EQ(
	    ranges[0],
	    std::make_tuple(1, Time::SinceEver(), Time::Forever())
	);
	EXPECT_EQ(ranges[1], std::make_tuple(2, Time::SinceEver(), Time()));
	EXPECT_EQ(ranges[2], std::make_tuple(3, Time(), Time().Add(2)));
	EXPECT_EQ(ranges[3], std::make_tuple(3, Time().Add(3), Time::Forever()));

	EXPECT_THROW(
	    { ranges = Query::GetMetaDataKeyRanges(*experiment, "isDead", false); },
	    std::out_of_range
	);

	EXPECT_THROW(
	    { ranges = Query::GetMetaDataKeyRanges(*experiment, "alive", Time()); },
	    std::invalid_argument
	);
}

TEST_F(QueryUTest, CorruptedData) {
	const auto &udata = TestSetup::UTestData();
	auto        e = Experiment::Create(udata.Basedir() / "corrupted.myrmidon");
	auto        s = e->CreateSpace("main");
	e->AddTrackingDataDirectory(
	    s->ID(),
	    udata.CorruptedDataDir().AbsoluteFilePath
	);

	std::vector<IdentifiedFrame::Ptr> res;
	using ::testing::_;
	using ::testing::MatchesRegex;

	auto progress =
	    std::make_unique<testing::StrictMock<MockTimeProgressReporter>>();

	EXPECT_CALL(*progress, SetBound(_, _)).Times(1);
	EXPECT_CALL(*progress, Update(_)).Times(::testing::AtLeast(2));
	EXPECT_CALL(
	    *progress,
	    ReportError(MatchesRegex("could not read \".*\" after frame .*; lost "
	                             ".*s, .*% of total query time of .*s"))
	)
	    .Times(1);
	EXPECT_CALL(
	    *progress,
	    ReportError(MatchesRegex(
	        "data corruption during query representing .*% of total query time"
	    ))
	)
	    .Times(1);

	Query::IdentifyFrames(
	    *e,
	    res,
	    {
	        {.Progress = std::move(progress)},
	    }
	);
}

} // namespace myrmidon
} // namespace fort
