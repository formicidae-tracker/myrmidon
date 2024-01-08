#include <gtest/gtest.h>

#include "Ant.hpp"
#include "Experiment.hpp"
#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

#include <fort/myrmidon/UtilsUTest.hpp>
#include <fort/myrmidon/priv/AntShapeType.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/myrmidon/priv/Measurement.hpp>
#include <fort/myrmidon/priv/Space.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>

#include <fort/myrmidon/ExperimentFile.pb.h>
#include <fort/myrmidon/priv/proto/FileReadWriter.hpp>

#include <fstream>

namespace fort {
namespace myrmidon {
namespace priv {

class ExperimentUTest : public ::testing::Test {
protected:
	void SetUp() {
		e = Experiment::Create(
		    TestSetup::UTestData().Basedir() / "experiment-utest.myrmidon"
		);
	}

	void TearDown() {
		fs::remove_all(e->AbsoluteFilePath());
		e.reset();
	}

	ExperimentPtr e;
};

typedef AlmostContiguousIDContainer<fort::myrmidon::AntID, Ant> Container;

void ReadAll(const fs::path &a, std::vector<uint8_t> &data) {
	data.clear();
	data.reserve(fs::file_size(a));
	std::ifstream f(a.c_str(), std::ios::binary);
	data = std::vector<uint8_t>(std::istreambuf_iterator<char>(f), {});
}

TEST_F(ExperimentUTest, CanAddTrackingDataDirectory) {
	try {
		auto s             = e->CreateSpace("nest");
		auto [tdd, errors] = TrackingDataDirectory::Open(
		    TestSetup::UTestData().NestDataDirs().front().AbsoluteFilePath,
		    TestSetup::UTestData().Basedir()
		);

		e->AddTrackingDataDirectory(s, tdd);

		ASSERT_EQ(s->TrackingDataDirectories().size(), 1);

		EXPECT_THROW(
		    {
			    auto artagData = TrackingDataDirectory::Open(
			        TestSetup::UTestData().ARTagDataDir().AbsoluteFilePath,
			        TestSetup::UTestData().Basedir()
			    );
			    ASSERT_EQ(
			        std::get<0>(artagData)->DetectionSettings().Family,
			        tags::Family::Tag36ARTag
			    );
			    // Could not add wrong family to experiment
			    e->AddTrackingDataDirectory(s, std::get<0>(artagData));
		    },
		    std::invalid_argument
		);

	} catch (const std::exception &e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
}

TEST_F(ExperimentUTest, IOTest) {
	auto experimentPath =
	    TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath;
	auto binaryResPath =
	    TestSetup::UTestData().Basedir() / "test-binary-io-match.myrmidon";
	try {
		e = Experiment::Open(experimentPath);
		ASSERT_EQ(e->Spaces().size(), 2);
		ASSERT_EQ(
		    e->Spaces().at(1)->TrackingDataDirectories().size(),
		    TestSetup::UTestData().NestDataDirs().size()
		);
		ASSERT_EQ(
		    e->Spaces().at(2)->TrackingDataDirectories().size(),
		    TestSetup::UTestData().ForagingDataDirs().size()
		);
		ASSERT_EQ(e->Identifier()->Ants().size(), 3);
		EXPECT_EQ(e->Identifier()->Ants().find(1)->second->AntID(), 1);
		EXPECT_EQ(e->Identifier()->Ants().find(2)->second->AntID(), 2);
		EXPECT_EQ(e->Identifier()->Ants().find(3)->second->AntID(), 3);
		EXPECT_EQ(
		    e->AbsoluteFilePath(),
		    TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath
		);
		EXPECT_EQ(e->Basedir(), TestSetup::UTestData().Basedir());

		EXPECT_EQ(e->Name(), "myrmidon test data");
		EXPECT_EQ(e->Author(), "myrmidon-tests");
		EXPECT_EQ(e->Comment(), "automatically generated data");
		EXPECT_EQ(e->Family(), fort::tags::Family::Tag36h11);

		e->Save(binaryResPath);
	} catch (const std::exception &e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
	auto PrintFile = [=](const std::string &path) {
		typedef priv::proto::FileReadWriter<pb::FileHeader, pb::FileLine> RW;
		RW::Read(
		    path,
		    [](const pb::FileHeader &header) {
			    std::cerr << header.DebugString() << std::endl;
		    },
		    [](const pb::FileLine &line) {
			    std::cerr << line.DebugString() << std::endl;
		    }
		);
	};
	try {
		std::vector<uint8_t> originalData, newData;
		ReadAll(experimentPath, originalData);
		ReadAll(binaryResPath, newData);
		ASSERT_EQ(newData.size(), originalData.size());
		for (size_t i = 0; i < newData.size(); ++i) {
			if (newData[i] != originalData[i]) {
				throw std::make_tuple(
				    (size_t)i,
				    (uint8_t)newData[i],
				    (uint8_t)originalData[i]
				);
			}
		}
	} catch (std::tuple<size_t, uint8_t, uint8_t> t) {
		ADD_FAILURE() << "Wrong byte read at " << std::get<0>(t) << " got "
		              << std::get<1>(t) << " expected " << std::get<2>(t);
		PrintFile(binaryResPath);
		PrintFile(experimentPath);
		return;
	} catch (const std::exception &e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}
}

void ListAllMeasurements(
    const Experiment::MeasurementByTagCloseUp &measurements,
    std::vector<Measurement::ConstPtr>        &result
) {
	size_t size = 0;
	for (const auto &[uri, measurementsByType] : measurements) {
		size += measurementsByType.size();
	}
	result.clear();
	result.reserve(size);
	for (const auto &[uri, measurementsByType] : measurements) {
		for (const auto &[type, m] : measurementsByType) {
			result.push_back(m);
		}
	}
}

TEST_F(ExperimentUTest, MeasurementEndToEnd) {
	TrackingDataDirectory::Ptr nest0, nest1;
	FixableErrorList           errors;
	Space::Ptr                 s;
	ASSERT_NO_THROW({
		e = Experiment::Create(
		    TestSetup::UTestData().Basedir() / "measurement-e2e.myrmidon"
		);
		e->Save(TestSetup::UTestData().Basedir() / "measurement-e2e.myrmidon");
		s                       = e->CreateSpace("box");
		std::tie(nest0, errors) = TrackingDataDirectory::Open(
		    TestSetup::UTestData().NestDataDirs()[0].AbsoluteFilePath,
		    TestSetup::UTestData().Basedir()
		);
		EXPECT_TRUE(errors.empty());
		std::tie(nest1, errors) = TrackingDataDirectory::Open(
		    TestSetup::UTestData().NestDataDirs()[1].AbsoluteFilePath,
		    TestSetup::UTestData().Basedir()
		);
		EXPECT_TRUE(errors.empty());

		e->AddTrackingDataDirectory(s, nest0);
		e->AddTrackingDataDirectory(s, nest1);
	});

	// It has a default measurment type Measurement::HEAD_TAIL_TYPE called
	// "head-tail"
	EXPECT_EQ(e->MeasurementTypes().size(), 1);
	if (e->MeasurementTypes().empty() == false) {
		auto defType = e->MeasurementTypes().begin()->second;
		EXPECT_EQ(defType->MTID(), Measurement::HEAD_TAIL_TYPE);
		EXPECT_EQ(defType->Name(), "head-tail");
	}

	EXPECT_THROW(
	    {
		    // we can't create a new one with the same type
		    e->CreateMeasurementType("foo", Measurement::HEAD_TAIL_TYPE);
	    },
	    std::runtime_error
	);

	EXPECT_THROW(
	    {
		    // we can't delete the default one
		    e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE);
	    },
	    std::invalid_argument
	);

	EXPECT_THROW(
	    {
		    // we can't delete an inexistant one
		    e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE + 1);
	    },
	    std::out_of_range
	);

	EXPECT_THROW(
	    {
		    // We are not allowed to add a measurement with an inexisting Type
		    e->SetMeasurement(std::make_shared<Measurement>(
		        "nest.0000",
		        Measurement::HEAD_TAIL_TYPE + 1,
		        Eigen::Vector2d(12, 1),
		        Eigen::Vector2d(1, 12),
		        12.0
		    ));
	    },
	    std::runtime_error
	);

	EXPECT_NO_THROW({ e->CreateMeasurementType("foo"); });

	EXPECT_NO_THROW({
		// its ok to be clumsy and use the same names for different type
		e->CreateMeasurementType("foo");
	});

	auto tcuPath = fs::path("nest.0000") / "frames" /
	               std::to_string(nest0->StartFrame()) / "closeups/0x015";
	auto badPath = fs::path("bar.0000") / "frames" /
	               std::to_string(nest0->StartFrame()) / "closeups/0x015";

	auto goodCustom = std::make_shared<Measurement>(
	    tcuPath.generic_string(),
	    Measurement::HEAD_TAIL_TYPE + 1,
	    Eigen::Vector2d(12, 1),
	    Eigen::Vector2d(1, 12),
	    12.0
	);
	auto goodDefault = std::make_shared<Measurement>(
	    tcuPath.generic_string(),
	    Measurement::HEAD_TAIL_TYPE,
	    Eigen::Vector2d(12, 12),
	    Eigen::Vector2d(10, 12),
	    12.0
	);
	auto defaultWithBadPath = std::make_shared<Measurement>(
	    badPath.generic_string(),
	    Measurement::HEAD_TAIL_TYPE,
	    Eigen::Vector2d(12, 12),
	    Eigen::Vector2d(10, 12),
	    12.0
	);

	auto badType = std::make_shared<Measurement>(
	    tcuPath.generic_string(),
	    Measurement::HEAD_TAIL_TYPE + 42,
	    Eigen::Vector2d(12, 12),
	    Eigen::Vector2d(10, 12),
	    12.0
	);

	EXPECT_NO_THROW({
		e->SetMeasurement(goodDefault);
		e->SetMeasurement(goodCustom);
	});

	// we cannot remove a directory that have a measurement
	EXPECT_FALSE(e->TrackingDataDirectoryIsDeletable(nest0->URI()));
	EXPECT_THROW(
	    { e->DeleteTrackingDataDirectory(nest0->URI()); },
	    std::runtime_error
	);

	EXPECT_THROW(
	    { e->SetMeasurement(defaultWithBadPath); },
	    std::invalid_argument
	);

	EXPECT_THROW({ e->SetMeasurement(badType); }, std::out_of_range);

	std::vector<Measurement::ConstPtr> list = {
	    goodCustom,
	    goodDefault,
	    defaultWithBadPath};
	ListAllMeasurements(e->Measurements(), list);
	EXPECT_EQ(list.size(), 2);
	auto listContains = [&list](const Measurement::ConstPtr &m) {
		auto fi = std::find_if(
		    list.cbegin(),
		    list.cend(),
		    [&m](const Measurement::ConstPtr &it) -> bool {
			    return it->URI() == m->URI();
		    }
		);
		return fi != list.cend();
	};
	EXPECT_TRUE(listContains(goodCustom));
	EXPECT_TRUE(listContains(goodDefault));
	EXPECT_FALSE(listContains(defaultWithBadPath));

	auto antBefore    = e->CreateAnt();
	auto identBefore1 = Identifier::AddIdentification(
	    e->Identifier(),
	    antBefore->AntID(),
	    1,
	    Time::SinceEver(),
	    nest0->End()
	);
	identBefore1->SetTagSize(2.0);

	auto identBefore2 = Identifier::AddIdentification(
	    e->Identifier(),
	    antBefore->AntID(),
	    0,
	    nest1->Start(),
	    Time::Forever()
	);
	identBefore2->SetTagSize(2.0);

	struct MData {
		TrackingDataDirectory::Ptr TDD;
		size_t                     Offset;
		TagID                      TID;
		MeasurementType::ID        MTID;
	};

	std::vector<MData> mData = {
	    {nest0, 0, 0, 1},
	    {nest0, 0, 0, 2},
	    {nest0, 0, 1, 1},
	    {nest0, 0, 1, 2},
	    {nest0, 1, 0, 1},
	    {nest0, 1, 0, 2},
	    {nest0, 1, 1, 1},
	    {nest0, 1, 1, 2},
	    {nest1, 0, 0, 1},
	    {nest1, 0, 0, 2},
	    {nest1, 0, 1, 1},
	    {nest1, 0, 1, 2},
	    {nest1, 1, 0, 1},
	    {nest1, 1, 0, 2},
	    {nest1, 1, 1, 1},
	    {nest1, 1, 1, 2}};
	std::vector<std::string> paths;
	paths.reserve(mData.size());

	for (const auto &md : mData) {
		auto tcuPath = fs::path(md.TDD->URI()) / "frames" /
		               std::to_string(md.TDD->StartFrame() + md.Offset) /
		               "closeups" / FormatTagID(md.TID);

		auto m = std::make_shared<Measurement>(
		    tcuPath.generic_string(),
		    md.MTID,
		    Eigen::Vector2d(12, 0),
		    Eigen::Vector2d(0, 0),
		    1.0
		);
		paths.push_back(m->URI());
		ASSERT_NO_THROW(e->SetMeasurement(m));
	}

	// Now we add a super Ant
	auto antAfter    = e->CreateAnt();
	auto identAfter1 = Identifier::AddIdentification(
	    e->Identifier(),
	    antAfter->AntID(),
	    0,
	    Time::SinceEver(),
	    nest0->End()
	);

	auto identAfter2 = Identifier::AddIdentification(
	    e->Identifier(),
	    antAfter->AntID(),
	    1,
	    nest1->Start(),
	    Time::Forever()
	);
	e->SetDefaultTagSize(1.0);
	EXPECT_VECTOR2D_EQ(identAfter1->AntPosition(), Eigen::Vector2d(6.0, 0.0));

	EXPECT_FALSE(identAfter1->HasUserDefinedAntPose());
	identAfter1->SetUserDefinedAntPose(Eigen::Vector2d(2, 3), 0.13);
	EXPECT_TRUE(identAfter1->HasUserDefinedAntPose());
	EXPECT_VECTOR2D_EQ(identAfter1->AntPosition(), Eigen::Vector2d(2, 3));
	EXPECT_EQ(identAfter1->AntAngle(), 0.13);
	identAfter1->ClearUserDefinedAntPose();
	EXPECT_VECTOR2D_EQ(identAfter1->AntPosition(), Eigen::Vector2d(6.0, 0.0));

	EXPECT_FALSE(identAfter1->HasUserDefinedAntPose());

	std::vector<ComputedMeasurement> measurements;
	e->ComputeMeasurementsForAnt(measurements, antAfter->AntID(), 1);

	EXPECT_EQ(measurements.size(), 4);
	for (const auto &m : measurements) {
		EXPECT_DOUBLE_EQ(9.6, m.LengthMM);
	}

	EXPECT_VECTOR2D_EQ(identBefore1->AntPosition(), Eigen::Vector2d(6.0, 0.0));

	e->ComputeMeasurementsForAnt(measurements, antBefore->AntID(), 1);

	EXPECT_EQ(measurements.size(), 4);
	for (const auto &m : measurements) {
		EXPECT_DOUBLE_EQ(19.2, m.LengthMM);
	}

	EXPECT_THROW(
	    {
		    e->ComputeMeasurementsForAnt(
		        measurements,
		        antAfter->AntID() + 100,
		        1
		    );
	    },
	    std::out_of_range
	);

	auto antLast = e->CreateAnt();
	Identifier::AddIdentification(
	    e->Identifier(),
	    antLast->AntID(),
	    22,
	    Time::SinceEver(),
	    Time::Forever()
	);

	e->ComputeMeasurementsForAnt(measurements, antAfter->AntID(), 4);
	EXPECT_EQ(measurements.size(), 0);

	e->ComputeMeasurementsForAnt(measurements, antLast->AntID(), 1);
	EXPECT_EQ(measurements.size(), 0);

	for (const auto &uri : paths) {
		ASSERT_NO_THROW(e->DeleteMeasurement(uri));
	}
	// deleting all measurements set the position to 0

	EXPECT_VECTOR2D_EQ(identBefore1->AntPosition(), Eigen::Vector2d(0.0, 0.0));
	EXPECT_VECTOR2D_EQ(identAfter1->AntPosition(), Eigen::Vector2d(0.0, 0.0));

	EXPECT_THROW(
	    {
		    e->DeleteMeasurement("none/frames/23/closeups/0x01a/measurements/1"
		    );
	    },
	    std::invalid_argument
	);

	EXPECT_THROW(
	    {
		    e->DeleteMeasurement(
		        "nest.0000/frames/1/closeups/0x01a/measurements/1"
		    );
	    },
	    std::runtime_error
	);

	EXPECT_THROW(
	    {
		    e->DeleteMeasurement(
		        "nest.0000/frames/1/closeups/0x015/measurements/34"
		    );
	    },
	    std::runtime_error
	);

	EXPECT_NO_THROW({
		// OK it has no measurement
		e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE + 2);
	});

	EXPECT_THROW(
	    {
		    // contains a tracking data directory
		    e->DeleteSpace(s->ID());
	    },
	    std::runtime_error
	);

	EXPECT_THROW(
	    {
		    // contains 2 measurements
		    e->DeleteTrackingDataDirectory(nest0->URI());
	    },
	    std::runtime_error
	);

	EXPECT_THROW(
	    {
		    // It contains data !!
		    e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE + 1);
	    },
	    std::runtime_error
	);

	EXPECT_NO_THROW({
		e->DeleteMeasurement(goodCustom->URI());
		e->DeleteMeasurementType(Measurement::HEAD_TAIL_TYPE + 1);
	});

	ListAllMeasurements(e->Measurements(), list);
	EXPECT_EQ(list.size(), 1);
	EXPECT_TRUE(listContains(goodDefault));

	EXPECT_NO_THROW({ e->DeleteMeasurement(goodDefault->URI()); });

	ListAllMeasurements(e->Measurements(), list);
	EXPECT_EQ(list.size(), 0);

	EXPECT_NO_THROW({
		e->DeleteTrackingDataDirectory(nest0->URI());
		e->DeleteTrackingDataDirectory(nest1->URI());
	});

	EXPECT_NO_THROW({ e->DeleteSpace(s->ID()); });
}

TEST_F(ExperimentUTest, TooSmallHeadTailMeasurementAreNotPermitted) {
	TrackingDataDirectory::Ptr nest0;
	FixableErrorList           errors;
	Space::Ptr                 s;
	ASSERT_NO_THROW({
		e = Experiment::Create(
		    TestSetup::UTestData().Basedir() /
		    "small-head-tail-measurement-failure.myrmidon"
		);
		e->Save(
		    TestSetup::UTestData().Basedir() /
		    "small-head-tail-measurement-failure.myrmidon"
		);
		std::tie(nest0, errors) = TrackingDataDirectory::Open(
		    TestSetup::UTestData().NestDataDirs().front().AbsoluteFilePath,
		    TestSetup::UTestData().Basedir()
		);
		s = e->CreateSpace("nest");
		e->AddTrackingDataDirectory(s, nest0);
		e->SetDefaultTagSize(1.0);
	});
	EXPECT_TRUE(errors.empty());

	auto ant   = e->CreateAnt();
	auto ident = Identifier::AddIdentification(
	    e->Identifier(),
	    ant->AntID(),
	    1,
	    Time::SinceEver(),
	    Time::Forever()
	);

	auto tcuPath = fs::path(nest0->URI()) / "frames" /
	               std::to_string(nest0->StartFrame() + 1) / "closeups" /
	               FormatTagID(1);
	// this measurement is subpixel value, it should throw an exception when set
	// to an experiment
	auto m = std::make_shared<Measurement>(
	    tcuPath.generic_string(),
	    1,
	    Eigen::Vector2d(0.5, 0),
	    Eigen::Vector2d(0, 0),
	    1.0
	);
	ComputedMeasurement::List lengths;
	ASSERT_NO_THROW({ e->ComputeMeasurementsForAnt(lengths, 1, 1); });
	ASSERT_EQ(lengths.size(), 0);
	EXPECT_THROW({ e->SetMeasurement(m); }, std::invalid_argument);
	ASSERT_NO_THROW({ e->ComputeMeasurementsForAnt(lengths, 1, 1); });
	// measurement should not have been stored as it fails
	EXPECT_EQ(lengths.size(), 0);

	m = std::make_shared<Measurement>(
	    tcuPath.generic_string(),
	    1,
	    Eigen::Vector2d(20, 0),
	    Eigen::Vector2d(0, 0),
	    1.0
	);
	ASSERT_NO_THROW({ e->SetMeasurement(m); });
	ASSERT_NO_THROW({ e->ComputeMeasurementsForAnt(lengths, 1, 1); });
	ASSERT_EQ(lengths.size(), 1);
	auto antPosition = ident->AntPosition();
	auto antAngle    = ident->AntAngle();

	m = std::make_shared<Measurement>(
	    tcuPath.generic_string(),
	    1,
	    Eigen::Vector2d(0.5, 0),
	    Eigen::Vector2d(0, 0),
	    1.0
	);
	EXPECT_THROW({ e->SetMeasurement(m); }, std::invalid_argument);
	ASSERT_NO_THROW({ e->ComputeMeasurementsForAnt(lengths, 1, 1); });
	// old measurment should have been kept;
	EXPECT_EQ(lengths.size(), 1);
	EXPECT_EQ(antPosition, ident->AntPosition());
	EXPECT_EQ(antAngle, ident->AntAngle());
}

TEST_F(ExperimentUTest, CornerWidthRatioForFamilies) {
	struct TestData {
		tags::Family F;
	};

	std::vector<TestData> testdata = {
	    {tags::Family::Tag36h11},
	    {tags::Family::Tag36h10},
	    {tags::Family::Tag16h5},
	    {tags::Family::Tag25h9},
	    {tags::Family::Circle21h7},
	    {tags::Family::Circle49h12},
	    {tags::Family::Custom48h12},
	    {tags::Family::Standard41h12},
	    {tags::Family::Standard52h13},
	};

	for (const auto &d : testdata) {
		EXPECT_NO_THROW({
			double ratio = Experiment::CornerWidthRatio(d.F);
			EXPECT_TRUE(ratio < 1.0 && ratio > 0.0);
			// test internal caching of the value
			EXPECT_EQ(ratio, Experiment::CornerWidthRatio(d.F));
		});
	}

	EXPECT_EQ(Experiment::CornerWidthRatio(tags::Family::Tag36ARTag), 1.0);

	EXPECT_THROW(
	    { Experiment::CornerWidthRatio(tags::Family::Undefined); },
	    std::invalid_argument
	);
}

TEST_F(ExperimentUTest, AntShapeTypeManipulation) {
	auto bodyType    = e->CreateAntShapeType("body");
	auto antennaType = e->CreateAntShapeType("antennas");
	auto a           = e->CreateAnt();
	a->AddCapsule(
	    bodyType->TypeID(),
	    std::make_shared<Capsule>(
	        Eigen::Vector2d(0, 0),
	        Eigen::Vector2d(0, 1),
	        0.1,
	        0.1
	    )
	);

	EXPECT_NO_THROW({ e->DeleteAntShapeType(antennaType->TypeID()); });

	EXPECT_THROW(
	    { e->DeleteAntShapeType(bodyType->TypeID()); },
	    std::runtime_error
	);

	EXPECT_THROW(
	    { e->DeleteAntShapeType(antennaType->TypeID()); },
	    std::out_of_range
	);
}

TEST_F(ExperimentUTest, AntMetadataManipulation) {
	auto alive = e->SetMetaDataKey("alive", true);
	auto group = e->SetMetaDataKey("group", std::string());
	ASSERT_EQ(alive->Type(), ValueType::BOOL);
	ASSERT_EQ(group->Type(), ValueType::STRING);
	auto ant = e->CreateAnt();
	EXPECT_NO_THROW({
		ant->SetValue("group", std::string("nurse"), Time::SinceEver());
	});
	// should throw because ant has a value
	EXPECT_THROW(group->SetDefaultValue(12), std::runtime_error);
	// OK to change a column without any values
	EXPECT_NO_THROW(alive->SetDefaultValue(0));
	ASSERT_EQ(alive->Type(), ValueType::INT);
	// Adding a column marks adds a default value to all Ant immediatly
	auto ageInDays = e->SetMetaDataKey("age", 0.0);
	ASSERT_EQ(ageInDays->Type(), ValueType::DOUBLE);
	EXPECT_VALUE_EQ(ant->GetValue("age", Time()), 0.0);
	// always possible to change the column name, even if there are existing
	// values
	EXPECT_NO_THROW({
		ageInDays->SetName("age-in-days");
		group->SetName("social-group");
	});
	EXPECT_THROW(ant->GetValue("group", Time()), std::out_of_range);
	EXPECT_THROW(ant->GetValue("age", Time()), std::out_of_range);
	EXPECT_NO_THROW({
		EXPECT_EQ(
		    std::get<std::string>(ant->GetValue("social-group", Time())),
		    "nurse"
		);
		EXPECT_EQ(std::get<double>(ant->GetValue("age-in-days", Time())), 0.0);
	});

	EXPECT_THROW(e->DeleteMetaDataKey("social-group"), std::runtime_error);
	EXPECT_THROW(e->RenameMetaDataKey("foo", "bar"), std::out_of_range);
	EXPECT_THROW(
	    e->RenameMetaDataKey("social-group", "age-in-days"),
	    std::invalid_argument
	);
	EXPECT_NO_THROW(e->RenameMetaDataKey("social-group", "group"));

	EXPECT_NO_THROW(e->DeleteMetaDataKey("age-in-days"));
	EXPECT_THROW(ant->GetValue("age-in-days", Time()), std::out_of_range);
}

TEST_F(ExperimentUTest, AntCloning) {
	TrackingDataDirectory::Ptr nest0;
	FixableErrorList           errors;
	ASSERT_NO_THROW({
		std::tie(nest0, errors) = TrackingDataDirectory::Open(
		    TestSetup::UTestData().NestDataDirs().front().AbsoluteFilePath,
		    TestSetup::UTestData().Basedir()
		);
	});
	EXPECT_TRUE(errors.empty());
	auto s = e->CreateSpace("nest");
	e->AddTrackingDataDirectory(s, nest0);

	std::vector<Ant::Ptr> ants = {
	    e->CreateAnt(),
	    e->CreateAnt(),
	    e->CreateAnt()};

	e->SetMeasurement(std::make_shared<Measurement>(
	    fs::path(nest0->URI()) / "frames" /
	        std::to_string(nest0->StartFrame()) / "closeups/0x001",
	    Measurement::HEAD_TAIL_TYPE,
	    Eigen::Vector2d(12, 12),
	    Eigen::Vector2d(0, 12),
	    12.0
	));

	e->SetMeasurement(std::make_shared<Measurement>(
	    fs::path(nest0->URI()) / "frames" /
	        std::to_string(nest0->StartFrame()) / "closeups/0x002",
	    Measurement::HEAD_TAIL_TYPE,
	    Eigen::Vector2d(12, 12),
	    Eigen::Vector2d(0, 12),
	    6.0
	));

	Identifier::AddIdentification(
	    e->Identifier(),
	    1,
	    1,
	    Time::SinceEver(),
	    Time::Forever()
	);

	Identifier::AddIdentification(
	    e->Identifier(),
	    2,
	    2,
	    Time::SinceEver(),
	    Time::Forever()
	);

	e->CreateAntShapeType("body");
	ants[0]->AddCapsule(
	    1,
	    std::make_shared<Capsule>(
	        Eigen::Vector2d(0, 0),
	        Eigen::Vector2d(1, 1),
	        1,
	        1
	    )
	);

	EXPECT_THROW(e->CloneAntShape(42, false, false), std::out_of_range);
	EXPECT_NO_THROW(e->CloneAntShape(2, false, false)
	); // do nothing as second ant has no shape
	EXPECT_THROW(
	    e->CloneAntShape(3, true, true),
	    std::runtime_error
	); // cannot work has ant 3 has no shape

	EXPECT_NO_THROW(e->CloneAntShape(1, false, false));
	ASSERT_EQ(ants[1]->Capsules().size(), 1);
	ASSERT_EQ(ants[2]->Capsules().size(), 1);
	EXPECT_CAPSULE_EQ(
	    *ants[1]->Capsules().front().second,
	    *ants[0]->Capsules().front().second
	);

	EXPECT_CAPSULE_EQ(
	    *ants[2]->Capsules().front().second,
	    *ants[0]->Capsules().front().second
	);

	ants[2]->ClearCapsules();
	EXPECT_NO_THROW(e->CloneAntShape(1, true, false));
	ASSERT_EQ(ants[1]->Capsules().size(), 1);
	ASSERT_EQ(ants[2]->Capsules().size(), 1);
	EXPECT_CAPSULE_EQ(
	    *ants[1]->Capsules().front().second,
	    *ants[0]->Capsules().front().second
	);

	EXPECT_CAPSULE_EQ(
	    *ants[2]->Capsules().front().second,
	    *ants[0]->Capsules().front().second
	);

	EXPECT_NO_THROW(e->CloneAntShape(1, true, true));
	EXPECT_CAPSULE_EQ(
	    *ants[1]->Capsules().front().second,
	    Capsule(Eigen::Vector2d(0, 0), Eigen::Vector2d(2, 2), 2.0, 2.0)
	);

	EXPECT_CAPSULE_EQ(
	    *ants[2]->Capsules().front().second,
	    *ants[0]->Capsules().front().second
	);
}

TEST_F(ExperimentUTest, OldFilesAreOpenable) {
	for (const auto &eInfo : TestSetup::UTestData().OldVersionFiles()) {
		EXPECT_NO_THROW({ Experiment::Open(eInfo.AbsoluteFilePath); });
	}
}

TEST_F(ExperimentUTest, WillNotOpenFileWhichAreTooRecent) {
	auto path = TestSetup::UTestData().FutureExperimentFile().AbsoluteFilePath;
	try {
		Experiment::Open(path);
		ADD_FAILURE() << "Opening " << path
		              << " should have thrown a std::runtime_error";
	} catch (const std::runtime_error &e) {
		std::ostringstream expected;
		expected << "Unexpected myrmidon file version "
		         << TestSetup::UTestData().FutureExperimentFile().Version
		         << " in " << path
		         << ": can only works with versions below or equal to 0.3.0";
		EXPECT_EQ(expected.str(), e.what());
	}
}

TEST_F(ExperimentUTest, CannotChangeDirectory) {
	EXPECT_THROW(
	    {
		    e->Save(
		        TestSetup::UTestData().Basedir() / "foo" / "bar" /
		        "exp.myrmidon"
		    );
	    },
	    std::invalid_argument
	);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
