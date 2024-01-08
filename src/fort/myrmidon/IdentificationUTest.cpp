#include <gtest/gtest.h>

#include "Experiment.hpp"

#include "TestSetup.hpp"
#include "UtilsUTest.hpp"
#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace fort {
namespace myrmidon {

class PublicIdentificationUTest : public ::testing::Test {
protected:
	void SetUp() {
		ASSERT_NO_THROW({
			experiment = Experiment::Create(
			    TestSetup::UTestData().Basedir() /
			    "identification-utest.myrmidon"
			);
			ant = experiment->CreateAnt();
			i   = experiment->AddIdentification(
                ant->ID(),
                123,
                Time::SinceEver(),
                Time::Forever()
            );
		});
	}

	void TearDown() {
		i.reset();
		ant.reset();
		experiment.reset();
	}

	Experiment::Ptr     experiment;
	Ant::Ptr            ant;
	Identification::Ptr i;
};

TEST_F(PublicIdentificationUTest, HasTargetValues) {
	EXPECT_EQ(i->TagValue(), 123);
	EXPECT_EQ(i->TargetAntID(), ant->ID());
}

TEST_F(PublicIdentificationUTest, TimeManipulation) {
	experiment->DeleteIdentification(i);
	i.reset();
	Identification::Ptr idents[2] = {
	    experiment
	        ->AddIdentification(ant->ID(), 123, Time::SinceEver(), Time()),
	    experiment->AddIdentification(ant->ID(), 124, Time(), Time::Forever()),
	};

	EXPECT_TIME_EQ(idents[0]->Start(), Time::SinceEver());
	EXPECT_TIME_EQ(idents[0]->End(), Time());
	EXPECT_TIME_EQ(idents[1]->Start(), Time());
	EXPECT_TIME_EQ(idents[1]->End(), Time::Forever());
	EXPECT_THROW(
	    { idents[0]->SetEnd(Time().Add(1)); },
	    OverlappingIdentification
	);
	EXPECT_THROW(
	    { idents[1]->SetStart(Time().Add(-1)); },
	    OverlappingIdentification
	);
	EXPECT_NO_THROW(idents[0]->SetEnd(Time().Add(-1)));
	EXPECT_NO_THROW(idents[1]->SetStart(Time().Add(1)));
	EXPECT_TIME_EQ(idents[0]->End(), Time().Add(-1));
	EXPECT_TIME_EQ(idents[1]->Start(), Time().Add(1));
}

TEST_F(PublicIdentificationUTest, TagSizeManipulation) {
	EXPECT_TRUE(i->HasDefaultTagSize());
	EXPECT_EQ(i->TagSize(), Identification::DEFAULT_TAG_SIZE);
	EXPECT_NO_THROW(i->SetTagSize(2.4));
	EXPECT_EQ(i->TagSize(), 2.4);
	EXPECT_FALSE(i->HasDefaultTagSize());
}

TEST_F(PublicIdentificationUTest, AntPose) {
	EXPECT_FALSE(i->HasUserDefinedAntPose());
	// normally ant pose is computed from priv algo, and its tested there
	// meanwhile it shoudl be Identity if no measurement where made
	EXPECT_VECTOR2D_EQ(i->AntPosition(), Eigen::Vector2d(0, 0));
	EXPECT_DOUBLE_EQ(i->AntAngle(), 0.0);
	Eigen::Vector2d position(1, 2);
	double          angle = 3.0;
	EXPECT_NO_THROW({ i->SetUserDefinedAntPose(position, angle); });
	EXPECT_TRUE(i->HasUserDefinedAntPose());
	EXPECT_VECTOR2D_EQ(i->AntPosition(), position);
	EXPECT_DOUBLE_EQ(i->AntAngle(), angle);

	EXPECT_NO_THROW(i->ClearUserDefinedAntPose());
	EXPECT_FALSE(i->HasUserDefinedAntPose());
	EXPECT_VECTOR2D_EQ(i->AntPosition(), Eigen::Vector2d(0, 0));
	EXPECT_DOUBLE_EQ(i->AntAngle(), 0.0);
}

TEST_F(PublicIdentificationUTest, Formatting) {
	std::ostringstream oss;
	oss << *i;
	EXPECT_EQ(oss.str(), "Identification{ID:0x07b ↦ 1, From:-∞, To:+∞}");
}

TEST_F(PublicIdentificationUTest, ScopeValidity) {
	experiment.reset();
	ant.reset();
	// this operation needs knowledge of all identifications.
	// if no handle mechanics, a DeleteReference exception would be thrown.
	EXPECT_NO_THROW(i->SetStart(Time()));
}

} // namespace myrmidon
} // namespace fort
