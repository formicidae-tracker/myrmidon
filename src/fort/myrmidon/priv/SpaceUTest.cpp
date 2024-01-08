#include <gtest/gtest.h>

#include "Space.hpp"

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

class SpaceUTest : public ::testing::Test {
protected:
	static void SetUpTestSuite();
	static void TearDownTestSuite();

	static std::vector<TrackingDataDirectory::Ptr> s_nest;
};

std::vector<TrackingDataDirectory::Ptr> SpaceUTest::s_nest;

void SpaceUTest::SetUpTestSuite() {
	for (const auto &tddInfo : TestSetup::UTestData().NestDataDirs()) {
		try {
			auto [tdd, errors] = TrackingDataDirectory::Open(
			    tddInfo.AbsoluteFilePath,
			    TestSetup::UTestData().Basedir()
			);
			s_nest.push_back(tdd);
		} catch (const std::exception &e) {
			ADD_FAILURE() << "Could not open " << tddInfo.AbsoluteFilePath
			              << ": " << e.what();
		};
	}

	ASSERT_TRUE(s_nest.size() >= 3);
}

void SpaceUTest::TearDownTestSuite() {
	s_nest.clear();
}

TEST_F(SpaceUTest, NameCheck) {
	struct TestData {
		std::string Name;
		bool        Throws;
	};

	std::vector<TestData> testdata = {
	    {"", true},
	    {"foo", false},
	    {"foo-bar", false},
	    {"foo/bar", true},
	    {"/foo", true},
	    {"foo/", true},
	};
	auto universe = std::make_shared<Universe>();

	auto good = Universe::CreateSpace(universe, 0, "good");
	for (const auto &d : testdata) {
		if (d.Throws == true) {
			EXPECT_THROW(
			    {
				    Universe::CreateSpace(universe, 0, d.Name);
				    ;
			    },
			    Space::InvalidName
			) << "Testing "
			  << d.Name;
			EXPECT_THROW({ good->SetName(d.Name); }, Space::InvalidName)
			    << "Testing " << d.Name;
		} else {
			EXPECT_NO_THROW({
				good->SetName(d.Name);
				EXPECT_EQ(good->Name(), d.Name);
				EXPECT_EQ(good->URI(), "spaces/" + std::to_string(good->ID()));
				good->SetName("good");
			}) << "Testing "
			   << d.Name;

			EXPECT_NO_THROW({
				auto res = Universe::CreateSpace(universe, 0, d.Name);
				EXPECT_EQ(res->Name(), d.Name);
				EXPECT_EQ(res->URI(), "spaces/" + std::to_string(res->ID()));
			}) << "Testing "
			   << d.Name;
		}
	}

	EXPECT_THROW(
	    { Universe::CreateSpace(universe, 0, "good"); },
	    Space::InvalidName
	);

	universe.reset();
	EXPECT_THROW({ good->SetName("willcrash"); }, DeletedReference<Universe>);
}

TEST_F(SpaceUTest, CanHoldTDD) {

	auto universe = std::make_shared<Universe>();
	auto foo      = Universe::CreateSpace(universe, 0, "foo");
	EXPECT_NO_THROW({
		foo->AddTrackingDataDirectory(s_nest[2]);
		foo->AddTrackingDataDirectory(s_nest[1]);
		foo->AddTrackingDataDirectory(s_nest[0]);
	});
	ASSERT_EQ(foo->TrackingDataDirectories().size(), 3);

	// now they are sorted
	EXPECT_EQ(foo->TrackingDataDirectories()[0], s_nest[0]);
	EXPECT_EQ(foo->TrackingDataDirectories()[1], s_nest[1]);
	EXPECT_EQ(foo->TrackingDataDirectories()[2], s_nest[2]);

	try {
		foo->AddTrackingDataDirectory(s_nest[0]);
		ADD_FAILURE(
		) << "Should have thrown Space::TDDOverlap but nothing is thrown";
	} catch (const Space::TDDOverlap &e) {
		EXPECT_EQ(e.A(), s_nest[0]);
		EXPECT_EQ(e.B(), s_nest[0]);
	} catch (...) {
		ADD_FAILURE() << "It have thrown something else";
	}

	EXPECT_NO_THROW({ universe->DeleteTrackingDataDirectory(s_nest[0]->URI()); }
	);

	EXPECT_THROW(
	    { universe->DeleteTrackingDataDirectory(s_nest[0]->URI()); },
	    Space::UnmanagedTrackingDataDirectory
	);

	EXPECT_THROW(
	    {
		    // Still having some data
		    universe->DeleteSpace(foo->ID());
	    },
	    Space::SpaceNotEmpty
	);

	EXPECT_THROW(
	    { universe->DeleteSpace(foo->ID() + 1); },
	    Space::UnmanagedSpace
	);

	auto bar = Universe::CreateSpace(universe, 0, "bar");

	EXPECT_NO_THROW({
		// not used by any other zone
		bar->AddTrackingDataDirectory(s_nest[0]);
	});

	EXPECT_THROW(
	    {
		    // used by foo
		    bar->AddTrackingDataDirectory(s_nest[2]);
	    },
	    Space::TDDAlreadyInUse
	);

	EXPECT_NO_THROW({
		// removes data that is associated with foo
		universe->DeleteTrackingDataDirectory(s_nest[0]->URI());
		// removes the zone is OK now
		universe->DeleteSpace(bar->ID());
	});
}

TEST_F(SpaceUTest, ExceptionFormatting) {
	struct TestData {
		std::shared_ptr<std::exception> E;
		std::string                     What;
	};

	Universe::Ptr universe;
	Space::Ptr    z;

	ASSERT_NO_THROW({
		universe = std::make_shared<Universe>();
		z        = Universe::CreateSpace(universe, 0, "z");
		z->AddTrackingDataDirectory(s_nest[1]);
		z->AddTrackingDataDirectory(s_nest[0]);
	});

	std::vector<TestData> testdata = {
	    {
	        std::make_shared<Space::TDDOverlap>(s_nest[0], s_nest[0]),
	        "TDD{URI:'nest.0000', start:" + s_nest[0]->Start().Format() +
	            ", end:" + s_nest[0]->End().Format() +
	            "} and TDD{URI:'nest.0000', start:" +
	            s_nest[0]->Start().Format() +
	            ", end:" + s_nest[0]->End().Format() + "} overlaps in time",
	    },
	    {
	        std::make_shared<Space::UnmanagedTrackingDataDirectory>("doo"),
	        "Unknown TDD{URI:'doo'}",
	    },
	    {
	        std::make_shared<Space::UnmanagedSpace>(42),
	        "Unknown SpaceID 42",
	    },
	    {
	        std::make_shared<Space::InvalidName>("doh", "it is 'doh'! Doh!"),
	        "Invalid Space name 'doh': it is 'doh'! Doh!",
	    },
	    {
	        std::make_shared<Space::SpaceNotEmpty>(*z),
	        "Space{ID:1, Name:'z'} is not empty "
	        "(contains:{'nest.0000','nest.0001'})",
	    },
	    {
	        std::make_shared<Space::TDDAlreadyInUse>("nest.0000", 42),
	        "TDD{URI:'nest.0000'} is in use in Space{ID:42}",
	    },
	};
	for (const auto &d : testdata) {
		EXPECT_EQ(std::string(d.E->what()), d.What);
	}
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
