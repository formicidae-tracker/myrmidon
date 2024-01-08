#include <gtest/gtest.h>

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

#include <fort/myrmidon/priv/Ant.hpp>
#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/myrmidon/priv/Measurement.hpp>
#include <fort/myrmidon/priv/Space.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

class ExperimentDataLessUTest : public ::testing::Test {
protected:
	static void SetUpTestSuite();
	static void TearDownTestSuite();

	static fs::path experimentPath;
};

fs::path ExperimentDataLessUTest::experimentPath;

void ExperimentDataLessUTest::SetUpTestSuite() {
	experimentPath  = TestSetup::UTestData().Basedir() / "data-less.myrmidon";
	auto experiment = Experiment::Create(experimentPath);

	// First we add some space and tracking data directories

	auto nest     = experiment->CreateSpace("nest");
	auto foraging = experiment->CreateSpace("foraging");

	for (const auto &tddInfo : TestSetup::UTestData().NestDataDirs()) {
		auto [tdd, errors] = TrackingDataDirectory::Open(
		    tddInfo.AbsoluteFilePath,
		    TestSetup::UTestData().Basedir()
		);
		experiment->AddTrackingDataDirectory(nest, tdd);
	}
	auto nest0 = nest->TrackingDataDirectories().front();

	for (const auto &tddInfo : TestSetup::UTestData().ForagingDataDirs()) {
		auto [tdd, errors] = TrackingDataDirectory::Open(
		    tddInfo.AbsoluteFilePath,
		    TestSetup::UTestData().Basedir()
		);
		EXPECT_TRUE(errors.empty());
		experiment->AddTrackingDataDirectory(foraging, tdd);
	}

	auto ant = experiment->CreateAnt();

	auto ident = Identifier::AddIdentification(
	    experiment->Identifier(),
	    ant->AntID(),
	    1,
	    Time::SinceEver(),
	    Time::Forever()
	);

	auto tcuPath = fs::path(nest0->URI()) / "frames" /
	               std::to_string(nest0->StartFrame()) / "closeups/0x001";

	auto mtype = experiment->CreateMeasurementType("antennas");

	auto poseEstimate = std::make_shared<Measurement>(
	    tcuPath.generic_string(),
	    Measurement::HEAD_TAIL_TYPE,
	    Eigen::Vector2d(1, -10),
	    Eigen::Vector2d(1, 14),
	    12.0
	);
	auto antennas = std::make_shared<Measurement>(
	    tcuPath.generic_string(),
	    mtype->MTID(),
	    Eigen::Vector2d(-2, 10),
	    Eigen::Vector2d(6, 10),
	    12.0
	);
	experiment->SetMeasurement(poseEstimate);
	experiment->SetMeasurement(antennas);

	EXPECT_DOUBLE_EQ(ident->AntPosition().x(), 1.0);
	EXPECT_DOUBLE_EQ(ident->AntPosition().y(), 2.0);
	EXPECT_DOUBLE_EQ(ident->AntAngle(), M_PI / 2.0);

	experiment->Save(experimentPath);
}

void ExperimentDataLessUTest::TearDownTestSuite() {}

TEST_F(ExperimentDataLessUTest, DataLessSupports) {

	try {
		Experiment::OpenDataLess(
		    TestSetup::UTestData().V0_1_File().AbsoluteFilePath
		);
		ADD_FAILURE(
		) << "No exception thrown when opening outdated myrmidon file";
	} catch (const std::runtime_error &e) {
		EXPECT_STREQ(
		    "Uncorrect myrmidon file version 0.1.0: data-less opening is only "
		    "supported for myrmidon file version above 0.2.0",
		    e.what()
		);
	}
}

TEST_F(ExperimentDataLessUTest, DataLessDoesNotListTDD) {
	auto experiment = Experiment::OpenDataLess(experimentPath);

	EXPECT_EQ(experiment->TrackingDataDirectories().size(), 0);
	const auto &spaces = experiment->Spaces();
	ASSERT_EQ(spaces.size(), 2);
	EXPECT_EQ(spaces.at(1)->Name(), "nest");
	EXPECT_EQ(spaces.at(2)->Name(), "foraging");
}

TEST_F(ExperimentDataLessUTest, DataLessDoesNotListMeasurements) {
	auto experiment = Experiment::OpenDataLess(experimentPath);

	EXPECT_TRUE(experiment->Measurements().empty());

	ComputedMeasurement::List measurements;
	try {
		experiment->ComputeMeasurementsForAnt(measurements, 1, 1);
		EXPECT_TRUE(measurements.empty());
		experiment->ComputeMeasurementsForAnt(measurements, 1, 2);
		EXPECT_TRUE(measurements.empty());

	} catch (const std::exception &e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
}

TEST_F(ExperimentDataLessUTest, PoseInformationIsConserved) {
	auto experiment = Experiment::OpenDataLess(experimentPath);

	ASSERT_EQ(experiment->Identifier()->Ants().size(), 1);

	auto a = experiment->Identifier()->Ants().at(1);

	ASSERT_FALSE(a->Identifications().empty());

	auto ident = a->Identifications().front();

	EXPECT_DOUBLE_EQ(ident->AntPosition().x(), 1.0);
	EXPECT_DOUBLE_EQ(ident->AntPosition().y(), 2.0);
	EXPECT_DOUBLE_EQ(ident->AntAngle(), M_PI / 2.0);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
