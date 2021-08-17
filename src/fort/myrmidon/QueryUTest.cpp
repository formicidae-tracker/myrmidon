#include "QueryUTest.hpp"

#include "Query.hpp"
#include "Ant.hpp"

#include <fort/myrmidon/TestSetup.hpp>

#include "UtilsUTest.hpp"

namespace fort {
namespace myrmidon {

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
	for ( size_t i = 0; i < expected.size(); ++i) {
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
	ASSERT_NO_THROW({
		});

	std::vector<Query::CollisionData> collisionData;

	ASSERT_NO_THROW({
			myrmidon::Query::QueryArgs args;
			Query::CollideFramesFunctor(*experiment,
			                            [&collisionData] (const Query::CollisionData & data) {
				                            collisionData.push_back(data);
			                            },
			                            args);
		});

	ASSERT_EQ(collisionData.size(),600);

	size_t nonEmptyFrame(0);
	for ( const auto & [positions,collisions] : collisionData ) {
		EXPECT_EQ(positions->Space,1);
		EXPECT_EQ(collisions->Space,1);
		if ( collisions->Collisions.empty() == false ) {
			++nonEmptyFrame;
		}
	}
	EXPECT_EQ(nonEmptyFrame,156);

}


TEST_F(QueryUTest,ComputeAntTrajectories) {
	std::vector<AntTrajectory::Ptr> trajectories;

	ASSERT_NO_THROW({
			myrmidon::Query::ComputeAntTrajectoriesArgs args;
			args.MaximumGap = 20000 * Duration::Millisecond;
			Query::ComputeAntTrajectoriesFunctor(*experiment,
			                                     [&trajectories]( const AntTrajectory::Ptr & t) {
				                                     trajectories.push_back(t);
			                                     },
			                                     args);
		});

	ASSERT_EQ(trajectories.size(),3);

	for( const auto & trajectory : trajectories ) {
		EXPECT_EQ(trajectory->Ant,1);
		EXPECT_EQ(trajectory->Space,1);
		ASSERT_EQ(trajectory->Positions.rows(),200);
		EXPECT_EQ(trajectory->Positions(0,0),0);
	}

}

TEST_F(QueryUTest,ComputeAntInteractions) {

	std::vector<AntTrajectory::Ptr> trajectories;
	std::vector<AntInteraction::Ptr> interactions;
	ASSERT_NO_THROW({
			myrmidon::Query::ComputeAntInteractionsArgs args;
			args.MaximumGap = 220 * Duration::Millisecond;

			Query::ComputeAntInteractionsFunctor(*experiment,
			                                     [&trajectories]( const AntTrajectory::Ptr & t) {
				                                     trajectories.push_back(t);
			                                     },
			                                     [&interactions]( const AntInteraction::Ptr & i) {
				                                     interactions.push_back(i);
			                                     },
			                                     args);
		});


	EXPECT_EQ(trajectories.size(),6);
	EXPECT_EQ(interactions.size(),15);
	for (const auto & interaction : interactions ) {
		EXPECT_EQ(interaction->IDs.first,1);
		EXPECT_EQ(interaction->IDs.second,2);
		EXPECT_EQ(interaction->Types.rows(),1);
		EXPECT_EQ(interaction->Types(0,0),1U);
		EXPECT_EQ(interaction->Types(0,1),1U);

		auto segmentStart =
			[](const AntTrajectorySegment &s) {
				Duration ellapsed = s.Trajectory->Positions(s.Begin,0) * double(Duration::Second.Nanoseconds());
				return s.Trajectory->Start.Add(ellapsed);
			};


		EXPECT_EQ(segmentStart(interaction->Trajectories.first),
		          interaction->Start);
		EXPECT_EQ(segmentStart(interaction->Trajectories.second),
		          interaction->Start);
	}
}


TEST_F(QueryUTest,FrameSelection) {
	auto firstDate = TestSetup::UTestData().NestDataDirs().front().Start;

	std::vector<IdentifiedFrame::Ptr> frames;

	myrmidon::Query::IdentifyFramesArgs args;

	// issue 138, should select all frames
	args.Start = firstDate;
	Query::IdentifyFramesFunctor(*experiment,
	                             [&frames](const IdentifiedFrame::Ptr & f) {
		                             frames.push_back(f);
	                             },
	                             args);

	EXPECT_FALSE(frames.empty());
	frames.clear();

	//selects the first frame
	args.Start = firstDate;
	args.End = firstDate.Add(1);
	Query::IdentifyFramesFunctor(*experiment,
	                             [&frames](const IdentifiedFrame::Ptr & f) {
		                             frames.push_back(f);
	                             },
	                             args);

	ASSERT_EQ(frames.size(),1);
	ASSERT_EQ(frames[0]->FrameTime,firstDate);

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
