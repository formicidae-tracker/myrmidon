#include <gtest/gtest.h>

#include "Experiment.hpp"
#include "Zone.hpp"

#include "TestSetup.hpp"
#include "UtilsUTest.hpp"
#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace fort {
namespace myrmidon {

class PublicZoneUTest : public ::testing::Test {
protected:
	Experiment::Ptr experiment;
	Space::Ptr      space;

	void SetUp() {
		experiment = Experiment::Create(
		    TestSetup::UTestData().Basedir() / "zone-utests.myrmidon"
		);
		space = experiment->CreateSpace("nest");
	}
};

TEST_F(PublicZoneUTest, FieldManipulation) {
	auto zone = space->CreateZone("exit");
	EXPECT_EQ(zone->Name(), "exit");
	EXPECT_NO_THROW(zone->SetName("food"));
	EXPECT_EQ(zone->Name(), "food");
}

TEST_F(PublicZoneUTest, ZoneDefinitionManipulation) {
	auto                zone           = space->CreateZone("exit");
	ZoneDefinition::Ptr definitions[2] = {
	    zone->AddDefinition({}, Time(), Time::Forever()),
	    zone->AddDefinition({}, Time::SinceEver(), Time()),
	};

	EXPECT_THROW(
	    { zone->AddDefinition({}, Time().Add(-1), Time().Add(1)); },
	    std::runtime_error
	);

	// it should be the same objects, but ordered
	ASSERT_EQ(zone->Definitions().size(), 2);
	EXPECT_EQ(zone->Definitions()[0], definitions[1]);
	EXPECT_EQ(zone->Definitions()[1], definitions[0]);

	EXPECT_THROW({ zone->DeleteDefinition(42); }, std::out_of_range);
	EXPECT_NO_THROW({ zone->DeleteDefinition(0); });

	EXPECT_EQ(zone->Definitions()[0], definitions[0]);
}

TEST_F(PublicZoneUTest, ZoneHaveExperimentUniqueID) {
	auto      space2   = experiment->CreateSpace("foraging");
	Zone::Ptr zones[3] = {
	    space->CreateZone("zone"),
	    space2->CreateZone("zone"),
	    space->CreateZone("zone"),
	};
	ZoneID expected(0);
	for (const auto &z : zones) {
		EXPECT_EQ(z->ID(), ++expected);
	}
}

TEST_F(PublicZoneUTest, ZoneDefinitionHaveAShape) {
	auto zone = space->CreateZone("food");
	auto def  = zone->AddDefinition({}, Time::SinceEver(), Time::Forever());
	auto ci   = std::make_shared<Circle>(Eigen::Vector2d(0.0, 0.0), 1.0);
	auto ca   = std::make_shared<Capsule>(
        Eigen::Vector2d(0.0, 0.0),
        Eigen::Vector2d(1, 1),
        1,
        1
    );

	EXPECT_TRUE(def->Shapes().empty());
	EXPECT_NO_THROW(def->SetShapes({ci, ca}));
	ASSERT_EQ(def->Shapes().size(), 2);
	EXPECT_EQ(def->Shapes()[0], ci);
	EXPECT_EQ(def->Shapes()[1], ca);
}

TEST_F(PublicZoneUTest, ZoneDefinitionHaveTimeValidity) {
	auto zone = space->CreateZone("food");
	EXPECT_THROW(
	    zone->AddDefinition({}, Time::Forever(), Time::SinceEver()),
	    std::invalid_argument
	);

	ZoneDefinition::Ptr definitions[2] = {
	    zone->AddDefinition({}, Time::SinceEver(), Time()),
	    zone->AddDefinition({}, Time(), Time::Forever()),
	};

	EXPECT_THROW(
	    { definitions[0]->SetEnd(Time().Add(1)); },
	    std::runtime_error
	);

	EXPECT_THROW(
	    { definitions[1]->SetStart(Time().Add(-1)); },
	    std::runtime_error
	);

	EXPECT_NO_THROW({
		definitions[0]->SetEnd(Time().Add(-1));
		definitions[1]->SetStart(Time().Add(1));
	});
}

} // namespace myrmidon
} // namespace fort
