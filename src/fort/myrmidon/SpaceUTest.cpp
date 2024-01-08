#include <gtest/gtest.h>

#include "Experiment.hpp"
#include "TestSetup.hpp"
#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace fort {
namespace myrmidon {

class PublicSpaceUTest : public ::testing::Test {
protected:
	void SetUp() {
		ASSERT_NO_THROW({
			experiment = Experiment::Create(
			    TestSetup::UTestData().Basedir() / "space-utest.myrmidon"
			);
			space = experiment->CreateSpace("foo");
		});
	}

	Experiment::Ptr experiment;
	Space::Ptr      space;
};

TEST_F(PublicSpaceUTest, FieldsManipulation) {
	EXPECT_EQ(space->ID(), 1);
	EXPECT_EQ(space->Name(), "foo");
	EXPECT_NO_THROW(space->SetName("bar"));
	EXPECT_EQ(space->Name(), "bar");
}

TEST_F(PublicSpaceUTest, ZoneManipulation) {
	auto z = space->CreateZone("food");
	EXPECT_EQ(space->Zones().size(), 1);
	// they are the same objects
	EXPECT_NO_THROW({ EXPECT_EQ(space->Zones().at(z->ID()), z); });

	EXPECT_THROW({ space->DeleteZone(42); }, std::out_of_range);

	EXPECT_NO_THROW(space->DeleteZone(z->ID()));
}

TEST_F(PublicSpaceUTest, CanLocateMovieFrame) {
	experiment = Experiment::Open(
	    TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath
	);
	std::pair<std::string, uint64_t> fileAndFrame;
	const auto &tddInfo = TestSetup::UTestData().WithVideoDataDir();

	EXPECT_NO_THROW({
		fileAndFrame =
		    experiment->Spaces().at(1)->LocateMovieFrame(tddInfo.Start);
	});
	EXPECT_EQ(fileAndFrame.first, tddInfo.AbsoluteFilePath / "stream.0000.mp4");

	EXPECT_EQ(fileAndFrame.second, 0);

	EXPECT_THROW(
	    {
		    fileAndFrame =
		        experiment->Spaces().at(1)->LocateMovieFrame(tddInfo.End);
	    },
	    std::out_of_range
	);
}

TEST_F(PublicSpaceUTest, ScopeValidity) {
	experiment.reset();
	// Creating a zone requires the knowledges of all the zone to
	// generate a UID.
	EXPECT_NO_THROW(space->CreateZone("food"));
}

} // namespace myrmidon
} // namespace fort
