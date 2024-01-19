#include <gtest/gtest.h>

#include <QSignalSpy>

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/studio/bridge/AntShapeTypeBridge.hpp>
#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/GlobalPropertyBridge.hpp>
#include <fort/studio/bridge/IdentifierBridge.hpp>
#include <fort/studio/bridge/MeasurementBridge.hpp>
#include <fort/studio/bridge/UniverseBridge.hpp>

namespace fmp = fort::myrmidon::priv;

class ExperimentBridgeUTest : public ::testing::Test {
protected:
	void SetUp();

	fs::path pathExisting;
};

void ExperimentBridgeUTest::SetUp() {
	try {
		pathExisting = TestSetup::UTestData().Basedir() / "ExperimentBridge.myrmidon";
		auto experiment = fmp::Experiment::Create(pathExisting);
		experiment->Save(pathExisting);
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Could not create experiment file: " << e.what();
	}

}


TEST_F(ExperimentBridgeUTest,ActiveModifiedState) {
	auto pathCreated = TestSetup::UTestData().Basedir() / "ExperimentBridge.2.myrmidon";
	ExperimentBridge controller;
	QSignalSpy modifiedSpy(&controller,SIGNAL(modified(bool)));
	QSignalSpy activatedSpy(&controller,SIGNAL(activated(bool)));

	EXPECT_EQ(controller.isActive(),false);
	EXPECT_EQ(controller.isModified(),false);
	EXPECT_EQ(modifiedSpy.count(),0);
	EXPECT_EQ(activatedSpy.count(),0);

	ASSERT_TRUE(controller.open(pathExisting.c_str(),nullptr));
	EXPECT_EQ(controller.absoluteFilePath().toUtf8().constData(),
	          pathExisting.string());
	EXPECT_EQ(controller.isModified(),false);
	EXPECT_EQ(controller.isActive(),true);
	EXPECT_EQ(modifiedSpy.count(),0);
	ASSERT_EQ(activatedSpy.count(),1);
	EXPECT_EQ(activatedSpy.at(0).at(0).toBool(),true);

	ASSERT_TRUE(controller.create(pathCreated.c_str()));
	EXPECT_EQ(controller.absoluteFilePath().toUtf8().constData(),
	          pathCreated.string());
	EXPECT_EQ(controller.isModified(),false);
	EXPECT_EQ(controller.isActive(),true);
	EXPECT_EQ(modifiedSpy.count(),0);
	ASSERT_EQ(activatedSpy.count(),2);
	EXPECT_EQ(activatedSpy.at(1).at(0).toBool(),true);


	controller.setModified(true);
	ASSERT_EQ(modifiedSpy.count(),1);
	EXPECT_EQ(modifiedSpy.at(0).at(0).toBool(),true);
	controller.setModified(false);
	ASSERT_EQ(modifiedSpy.count(),2);
	EXPECT_EQ(modifiedSpy.at(1).at(0).toBool(),false);

	// test child connections
	std::vector<GlobalBridge*> children = controller.d_children;
	size_t expected = 2;
	for ( const auto & b : children ) {

		b->setModified(true);

		++expected;
		ASSERT_EQ(modifiedSpy.count(),expected);
		EXPECT_EQ(modifiedSpy.at(expected-1).at(0).toBool(),true);

		controller.setModified(false);

		++expected;
		ASSERT_EQ(modifiedSpy.count(),expected);
		EXPECT_EQ(modifiedSpy.at(expected-1).at(0).toBool(),false);

	}

}
