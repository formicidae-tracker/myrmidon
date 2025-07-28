#include "AntMeasurementUTest.hpp"

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/myrmidon/TestSetup.hpp>

#include <fort/studio/bridge/AntMeasurementBridge.hpp>
#include <fort/studio/bridge/IdentifierBridge.hpp>
#include <fort/studio/bridge/MeasurementBridge.hpp>

#include <QAbstractItemModel>

void AntMeasurementUTest::SetUp() {
	ASSERT_NO_THROW({
		auto experiment = fmp::Experiment::Create(
		    TestSetup::UTestData().Basedir() / "ant-measurement-bridge.myrmidon"
		);
		auto  s       = experiment->CreateSpace("foo");
		auto &tddInfo = TestSetup::UTestData().NestDataDirs().back();
		fmp::TrackingDataDirectory::Ptr tdd;
		fm::FixableErrorList            errors;
		std::tie(tdd, errors) = fmp::TrackingDataDirectory::Open(
		    tddInfo.AbsoluteFilePath,
		    TestSetup::UTestData().Basedir(),
		    {}
		);

		if (tdd->TagCloseUpsComputed() == false) {
			for (const auto &l : tdd->PrepareTagCloseUpsLoaders()) {
				l();
			}
		}
		experiment->AddTrackingDataDirectory(s, tdd);
		d_experiment.setExperiment(experiment);

		for (const auto &tcu : tddInfo.TagCloseUps) {
			d_closeUps[tcu->TagValue()].push_back(tcu);
		}
	});
	ASSERT_TRUE(d_closeUps[0].size() >= 2);
	ASSERT_TRUE(d_closeUps[1].size() >= 2);
}

void AntMeasurementUTest::TearDown() {
	d_experiment.setExperiment(nullptr);
}


TEST_F(AntMeasurementUTest,EndToEnd) {
	auto m = d_experiment.antMeasurements()->model();

	EXPECT_EQ(m->rowCount(),0);

	EXPECT_EQ(m->columnCount(),2);
	EXPECT_EQ(std::string(m->headerData(0,Qt::Horizontal).toString().toUtf8().constData()),
	          "Ant");

	EXPECT_EQ(std::string(m->headerData(1,Qt::Horizontal).toString().toUtf8().constData()),
	          "head-tail");

	d_experiment.createAnt();
	d_experiment.identifier()->addIdentification(1,0,fort::Time::SinceEver(),fort::Time::Forever());
	EXPECT_EQ(m->rowCount(),1);
	EXPECT_EQ(std::string(m->data(m->index(0,0)).toString().toUtf8().constData()),
	          "001 ↤ {0x000}");

	EXPECT_EQ(std::string(m->data(m->index(0,1)).toString().toUtf8().constData()),
	          "0");

	d_experiment.measurements()->setMeasurement(d_closeUps[0][0],
	                                            1,
	                                            QPointF(10,0),
	                                            QPointF(-10,0));

	EXPECT_EQ(std::string(m->data(m->index(0,1)).toString().toUtf8().constData()),
	          "1");

	d_experiment.measurements()->setMeasurement(d_closeUps[0][1],
	                                            1,
	                                            QPointF(10,0),
	                                            QPointF(-10,0));


	EXPECT_EQ(std::string(m->data(m->index(0,1)).toString().toUtf8().constData()),
	          "2");

	auto iModel = d_experiment.identifier()->model();
	iModel->setData(iModel->index(0,3),"1970-01-01T00:00:00.000Z");

	EXPECT_EQ(std::string(m->data(m->index(0,1)).toString().toUtf8().constData()),
	          "0");

	iModel->setData(iModel->index(0,3),"");

	EXPECT_EQ(std::string(m->data(m->index(0,1)).toString().toUtf8().constData()),
	          "2");

	auto lastM = d_experiment.measurements()->measurementForCloseUp(d_closeUps[0][1]->URI(),
	                                                                1);
	d_experiment.measurements()->deleteMeasurement(lastM);

	EXPECT_EQ(std::string(m->data(m->index(0,1)).toString().toUtf8().constData()),
	          "1");

	d_experiment.measurements()->setMeasurementType(-1,"thorax");
	EXPECT_EQ(m->columnCount(),3);
	EXPECT_EQ(std::string(m->headerData(2,Qt::Horizontal).toString().toUtf8().constData()),
	          "thorax");
	EXPECT_EQ(std::string(m->data(m->index(0,2)).toString().toUtf8().constData()),
		          "0");


	d_experiment.measurements()->setMeasurementType(-1,"antenna");
	EXPECT_EQ(m->columnCount(),4);
	EXPECT_EQ(std::string(m->headerData(3,Qt::Horizontal).toString().toUtf8().constData()),
	          "antenna");
	EXPECT_EQ(std::string(m->headerData(2,Qt::Horizontal).toString().toUtf8().constData()),
	          "thorax");
	EXPECT_EQ(std::string(m->data(m->index(0,2)).toString().toUtf8().constData()),
	          "0");


	d_experiment.measurements()->deleteMeasurementType(2);
	EXPECT_EQ(m->columnCount(),3);
	EXPECT_EQ(std::string(m->headerData(2,Qt::Horizontal).toString().toUtf8().constData()),
	          "antenna");

	d_experiment.measurements()->setMeasurementType(-1,"sterm");
	EXPECT_EQ(m->columnCount(),4);
	EXPECT_EQ(std::string(m->headerData(3,Qt::Horizontal).toString().toUtf8().constData()),
	          "antenna");
	EXPECT_EQ(std::string(m->headerData(2,Qt::Horizontal).toString().toUtf8().constData()),
	          "sterm");

	d_experiment.createAnt();
	EXPECT_EQ(std::string(m->data(m->index(1,0)).toString().toUtf8().constData()),
	          "002 <no-tags>");

	d_experiment.identifier()->addIdentification(2,1,fort::Time::SinceEver(),fort::Time::Forever());
	EXPECT_EQ(std::string(m->data(m->index(1,0)).toString().toUtf8().constData()),
	          "002 ↤ {0x001}");

	EXPECT_EQ(m->rowCount(),2);
	EXPECT_EQ(std::string(m->data(m->index(1,1)).toString().toUtf8().constData()),
	          "0");

	EXPECT_EQ(std::string(m->data(m->index(1,2)).toString().toUtf8().constData()),
	          "0");

	EXPECT_EQ(std::string(m->data(m->index(1,3)).toString().toUtf8().constData()),
	          "0");

}
