#include <gtest/gtest.h>

#include "Experiment.hpp"
#include "TestSetup.hpp"

#include <fort/hermes/FileContext.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace fort {
namespace myrmidon {

class PublicTrackingSolverUTest : public ::testing::Test {
protected:
	void SetUp() {
		ASSERT_NO_THROW({
			experiment = Experiment::Open(
			    TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath
			);
			solver = experiment->CompileTrackingSolver(false);
		});

		auto firstFramePath =
		    TestSetup::UTestData().NestDataDirs().front().AbsoluteFilePath /
		    "tracking.0000.hermes";

		ASSERT_NO_THROW({
			fort::hermes::FileContext fc(firstFramePath);
			fc.Read(&readout);
		});
	}

	Experiment::Ptr            experiment;
	TrackingSolver::Ptr        solver;
	fort::hermes::FrameReadout readout;
};

TEST_F(PublicTrackingSolverUTest, CanIdentifyAnts) {
	EXPECT_EQ(solver->IdentifyAnt(0, Time()), 1);
	EXPECT_EQ(solver->IdentifyAnt(1, Time()), 2);
	EXPECT_EQ(solver->IdentifyAnt(2, Time()), 3);
	EXPECT_EQ(solver->IdentifyAnt(123, Time()), 0);
}

TEST_F(PublicTrackingSolverUTest, CanIdentifyAndCollideFrames) {
	IdentifiedFrame identified;
	CollisionFrame  collision;
	EXPECT_NO_THROW(solver->IdentifyFrame(identified, readout, 1));
	EXPECT_EQ(identified.Positions.rows(), 3);
	EXPECT_NO_THROW(solver->CollideFrame(identified, collision));
	EXPECT_TRUE(collision.Collisions.empty());
}

} // namespace myrmidon
} // namespace fort
