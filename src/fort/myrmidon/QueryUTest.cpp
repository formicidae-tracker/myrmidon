#include <gtest/gtest.h>

#include "Experiment.hpp"
#include "Query.hpp"
#include "Ant.hpp"

#include "TestSetup.hpp"
#include "UtilsUTest.hpp"

namespace fort {
namespace myrmidon {

class QueryUTest : public ::testing::Test {
protected:
	Experiment::Ptr experiment;
	void SetUp();
	void TearDown();
};

void QueryUTest::SetUp() {
	ASSERT_NO_THROW({
			experiment = Experiment::Open(TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath);
		});
}

void QueryUTest::TearDown() {
	experiment.reset();
}


TEST_F(QueryUTest,TagStatistics) {
	TagStatistics::ByTagID tagStats;
	ASSERT_NO_THROW({
			tagStats = Query::ComputeTagStatistics(*experiment);
		});
	EXPECT_TAG_STATISTICS_EQ(tagStats,TestSetup::UTestData().ExpectedTagStatistics());
}


TEST_F(QueryUTest,IdentifyFrames) {

	std::vector<IdentifiedFrame::Ptr> identifieds;
	const auto & expected = TestSetup::UTestData().ExpectedFrames();
	ASSERT_NO_THROW({
			myrmidon::Query::IdentifyFramesArgs args;
			Query::IdentifyFramesFunctor(*experiment,
			                             [&identifieds] (const IdentifiedFrame::Ptr & i) {
				                             identifieds.push_back(i);
			                             },args);
		});
	ASSERT_EQ(identifieds.size(),expected.size());
	for ( size_t i = 0; i < std::min(identifieds.size(),expected.size()); ++i) {
		EXPECT_IDENTIFIED_FRAME_EQ(*identifieds[i],*expected[i].first) << "  With i: " << i;
	}
	auto t = TestSetup::UTestData().NestDataDirs().front().End;
	size_t expectedNumber = std::find_if(expected.begin(),
	                                     expected.end(),
	                                     [&t](const std::pair<IdentifiedFrame::Ptr,CollisionFrame::Ptr> & it ) {
		                                     return it.first->FrameTime > t;
	                                     }) - expected.begin();
	identifieds.clear();
	ASSERT_NO_THROW({
			myrmidon::Query::IdentifyFramesArgs args;
			args.End = t;
			Query::IdentifyFrames(*experiment,
			                      identifieds,
			                      args);
		});
	EXPECT_EQ(identifieds.size(),expectedNumber);

	identifieds.clear();
	try {
		myrmidon::Query::IdentifyFramesArgs args;
		args.Start = t;
		Query::IdentifyFramesFunctor(*experiment,
		                             [&identifieds] (const IdentifiedFrame::Ptr & i) {
			                             identifieds.push_back(i);
		                             },
		                             args);
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Unexpected exception: " << e.what();
		return;
	}
	// we just have removed the first frame
	EXPECT_EQ(identifieds.size(),expected.size()-expectedNumber);
}

TEST_F(QueryUTest,CollideFrames) {
	const auto & expected = TestSetup::UTestData().ExpectedFrames();

	std::vector<Query::CollisionData> collisionData;

	ASSERT_NO_THROW({
			myrmidon::Query::CollideFramesArgs args;
			Query::CollideFramesFunctor(*experiment,
			                            [&collisionData] (const Query::CollisionData & data) {
				                            collisionData.push_back(data);
			                            },
			                            args);
		});

	ASSERT_EQ(collisionData.size(),expected.size());
	for ( size_t i = 0; i < std::min(collisionData.size(),expected.size()); ++i ) {
		EXPECT_IDENTIFIED_FRAME_EQ(*collisionData[i].first,
		                           *expected[i].first) << "  With i: " << i;
		EXPECT_COLLISION_FRAME_EQ(*collisionData[i].second,
		                          *expected[i].second) << "  With i: " << i;
	}

}


TEST_F(QueryUTest,ComputeAntTrajectories) {
	size_t i = 0;
	for ( const auto & expected : TestSetup::UTestData().ExpectedResults() ) {

		std::vector<AntTrajectory::Ptr> trajectories;

		ASSERT_NO_THROW({
				myrmidon::Query::ComputeAntTrajectoriesArgs args;
				args.Start = expected.Start;
				args.End = expected.End;
				args.MaximumGap = expected.MaximumGap;
				args.Matcher = expected.Matches;
				if ( ++i % 2 == 0 ) {
					Query::ComputeAntTrajectoriesFunctor(*experiment,
					                                     [&trajectories]( const AntTrajectory::Ptr & t) {
						                                     trajectories.push_back(t);
					                                     },
					                                     args);
				} else {
					Query::ComputeAntTrajectories(*experiment,
					                              trajectories,
					                              args);
				}
			});

		EXPECT_EQ(trajectories.size(),expected.Trajectories.size());
		// trajectories, due to TDD boundaries may not be sorted
		std::sort(trajectories.begin(),
		          trajectories.end(),
		          [](const AntTrajectory::Ptr & a,
		             const AntTrajectory::Ptr & b) {
			          if ( a->End() == b->End() ) {
				          return a->Space < b->Space;
			          }
			          return a->End() < b->End();
		          });


		for ( size_t i = 0; i < std::min(trajectories.size(),expected.Trajectories.size()); ++i ) {
			EXPECT_ANT_TRAJECTORY_EQ(*trajectories[i],
			                         *expected.Trajectories[i])
				<< "  With i: " << i;
		}
	}
}

TEST_F(QueryUTest,ComputeAntInteractions) {
	size_t i = 0;
	for ( const auto & expected : TestSetup::UTestData().ExpectedResults() ) {

		std::vector<AntTrajectory::Ptr> trajectories;
		std::vector<AntInteraction::Ptr> interactions;
		ASSERT_NO_THROW({
				myrmidon::Query::ComputeAntInteractionsArgs args;
				args.Start = expected.Start;
				args.End = expected.End;
				args.MaximumGap = expected.MaximumGap;
				args.Matcher = expected.Matches;
				if ( ++i % 2 == 0 ) {
					Query::ComputeAntInteractionsFunctor(*experiment,
					                                     [&trajectories]( const AntTrajectory::Ptr & t) {
						                                     trajectories.push_back(t);
					                                     },
					                                     [&interactions]( const AntInteraction::Ptr & i) {
						                                     interactions.push_back(i);
					                                     },
					                                     args);
				} else {
					Query::ComputeAntInteractions(*experiment,
					                              trajectories,
					                              interactions,
					                              args);
				}
			});


		EXPECT_EQ(trajectories.size(),expected.Trajectories.size());
		EXPECT_EQ(interactions.size(),expected.Interactions.size());

		// trajectories, due to TDD boundaries may not be sorted
		std::sort(trajectories.begin(),
		          trajectories.end(),
		          [](const AntTrajectory::Ptr & a,
		             const AntTrajectory::Ptr & b) {
			          if ( a->End() == b->End() ) {
				          return a->Space < b->Space;
			          }
			          return a->End() < b->End();
		          });


		for ( size_t i = 0; i < std::min(trajectories.size(),expected.Trajectories.size()); ++i ) {
			EXPECT_ANT_TRAJECTORY_EQ(*trajectories[i],
			                         *expected.Trajectories[i])
				<< "  With i: " << i;
		}

		for ( size_t i = 0; i < std::min(expected.Interactions.size(),expected.Interactions.size()); ++i ) {
			EXPECT_ANT_INTERACTION_EQ(*interactions[i],
			                          *expected.Interactions[i])
				<< "  With i: " << i << std::endl
				<< "Expected Interaction End: " << expected.Interactions[i]->End << std::endl
				<< "  Acutal Interaction End: " << interactions[i]->End << std::endl
				<< "    Expected Segment End: " << std::get<0>(expected.Interactions[i]->Trajectories).first.EndTime() << std::endl
				<< "      Actual Segment End: " << std::get<0>(interactions[i]->Trajectories).first.EndTime() << std::endl;

		}
	}
}


TEST_F(QueryUTest,ComputeAntInteractionsSummarized) {
	size_t i = 0;
	for ( const auto & expected : TestSetup::UTestData().ExpectedResults() ) {
		auto expectedInteractions = expected.Summarized();
		std::vector<AntTrajectory::Ptr> trajectories;
		std::vector<AntInteraction::Ptr> interactions;

		ASSERT_NO_THROW({
				myrmidon::Query::ComputeAntInteractionsArgs args;
				args.Start = expected.Start;
				args.End = expected.End;
				args.MaximumGap = expected.MaximumGap;
				args.Matcher = expected.Matches;
				args.ReportFullTrajectories = false;
				Query::ComputeAntInteractions(*experiment,
				                              trajectories,
				                              interactions,
				                              args);
			});

		EXPECT_EQ(trajectories.size(),0);
		EXPECT_EQ(interactions.size(),expectedInteractions.size());

		for ( size_t i = 0; i < std::min(expected.Interactions.size(),expectedInteractions.size()); ++i ) {
			EXPECT_ANT_INTERACTION_EQ(*interactions[i],
			                          *expectedInteractions[i])
				<< "  With i: " << i << std::endl;
		}
	}
}


TEST_F(QueryUTest,FrameSelection) {
	auto firstDate = std::min(TestSetup::UTestData().NestDataDirs().front().Start,
	                          TestSetup::UTestData().ForagingDataDirs().front().Start);

	std::vector<IdentifiedFrame::Ptr> frames;

	myrmidon::Query::IdentifyFramesArgs args;

	// issue 138, should select all frames
	args.Start = firstDate;
	Query::IdentifyFramesFunctor(*experiment,
	                             [&frames](const IdentifiedFrame::Ptr & f) {
		                             frames.push_back(f);
	                             },
	                             args);

	EXPECT_EQ(frames.size(),TestSetup::UTestData().ExpectedFrames().size());
	frames.clear();

	//selects the first frame
	args.Start = firstDate;
	args.End = firstDate.Add(1);
	Query::IdentifyFramesFunctor(*experiment,
	                             [&frames](const IdentifiedFrame::Ptr & f) {
		                             frames.push_back(f);
	                             },
	                             args);

	ASSERT_TRUE(frames.size() >  0);
	ASSERT_TRUE(frames.size() <= 2);

	EXPECT_EQ(frames[0]->FrameTime,firstDate);
	if ( frames.size() > 1 ) {
		EXPECT_EQ(frames[1]->FrameTime,firstDate);
		EXPECT_TRUE(frames[1]->Space != frames[0]->Space);
	}

	frames.clear();
	// won't access any
	args.Start = firstDate;
	args.End = firstDate;
	Query::IdentifyFramesFunctor(*experiment,
	                             [&frames](const IdentifiedFrame::Ptr & f) {
		                             frames.push_back(f);
	                             },
	                             args);

	ASSERT_EQ(frames.size(),0);
}


} // namespace myrmidon
} // namespace fort
