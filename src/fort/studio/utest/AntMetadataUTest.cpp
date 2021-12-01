#include <gtest/gtest.h>

#include <fort/myrmidon/TestSetup.hpp>

#include <fort/studio/Format.hpp>
#include <fort/studio/bridge/AntKeyValueBridge.hpp>

#include <QAbstractItemModel>

#include <QTest>
#include <QSignalSpy>

class AntKeyValueUTest : public ::testing::Test {
protected :
	void SetUp();
	void TearDown();

	fmp::Experiment::Ptr experiment;
	AntKeyValueBridge  * bridge;
};


void AntKeyValueUTest::SetUp() {
	experiment = fmp::Experiment::Create(TestSetup::UTestData().Basedir() / "antmetadata-utest.myrmidon");
	bridge = new AntKeyValueBridge(nullptr);
}

void AntKeyValueUTest::TearDown() {
	delete bridge;
	experiment.reset();
}

TEST_F(AntKeyValueUTest,Activation) {
	QSignalSpy activated(bridge,SIGNAL(activated(bool)));

	EXPECT_FALSE(bridge->isActive());
	EXPECT_EQ(activated.count(),0);

	bridge->setExperiment(experiment);
	EXPECT_TRUE(bridge->isActive());
	ASSERT_EQ(activated.count(),1);
	EXPECT_TRUE(activated.last().at(0).toBool());

	bridge->setExperiment(experiment);
	EXPECT_TRUE(bridge->isActive());
	EXPECT_EQ(activated.count(),2);
	EXPECT_TRUE(activated.last().at(0).toBool());


	bridge->setExperiment(fmp::Experiment::Ptr());
	EXPECT_FALSE(bridge->isActive());
	EXPECT_EQ(activated.count(),3);
	EXPECT_FALSE(activated.last().at(0).toBool());

}

TEST_F(AntKeyValueUTest,TypeModel) {
	auto m = bridge->typeModel();
	ASSERT_EQ(m->rowCount(),5);

	EXPECT_EQ(ToStdString(m->index(0,0).data(Qt::DisplayRole).toString()),
	          "Bool");
	EXPECT_EQ(m->index(0,0).data(AntKeyValueBridge::KeyTypeRole).toInt(),
	          int(fmp::ValueType::BOOL));

	EXPECT_EQ(ToStdString(m->index(1,0).data(Qt::DisplayRole).toString()),
	          "Int");
	EXPECT_EQ(m->index(1,0).data(AntKeyValueBridge::KeyTypeRole).toInt(),
	          int(fmp::ValueType::INT));

	EXPECT_EQ(ToStdString(m->index(2,0).data(Qt::DisplayRole).toString()),
	          "Double");
	EXPECT_EQ(m->index(2,0).data(AntKeyValueBridge::KeyTypeRole).toInt(),
	          int(fmp::ValueType::DOUBLE));


	EXPECT_EQ(ToStdString(m->index(3,0).data(Qt::DisplayRole).toString()),
	          "String");
	EXPECT_EQ(m->index(3,0).data(AntKeyValueBridge::KeyTypeRole).toInt(),
	          int(fmp::ValueType::STRING));

	EXPECT_EQ(ToStdString(m->index(4,0).data(Qt::DisplayRole).toString()),
	          "Time");
	EXPECT_EQ(m->index(4,0).data(AntKeyValueBridge::KeyTypeRole).toInt(),
	          int(fmp::ValueType::TIME));
}

TEST_F(AntKeyValueUTest,ModificationTest) {
	bridge->setExperiment(experiment);

	QSignalSpy modified(bridge,SIGNAL(modified(bool)));
	ASSERT_TRUE(modified.isValid());

	auto m = bridge->keyModel();
	EXPECT_EQ(m->rowCount(),0);
	EXPECT_FALSE(bridge->isModified());
	EXPECT_EQ(modified.count(),0);

	bridge->setKey("foo",0.0);
	EXPECT_EQ(m->rowCount(),1);
	ASSERT_EQ(modified.count(),1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(ToStdString(m->index(0,0).data(Qt::DisplayRole).toString()),
	          "foo");
	EXPECT_EQ(ToStdString(m->index(0,1).data(Qt::DisplayRole).toString()),
	          "Double");
	EXPECT_EQ(m->index(0,1).data(AntKeyValueBridge::KeyTypeRole).toInt(),
	          quint32(fmp::ValueType::DOUBLE));

	bridge->setKey("bar",std::string(""));
	EXPECT_EQ(modified.count(),1);
	EXPECT_EQ(ToStdString(m->index(0,0).data(Qt::DisplayRole).toString()),
	          "bar");
	EXPECT_EQ(ToStdString(m->index(0,1).data(Qt::DisplayRole).toString()),
	          "String");
	EXPECT_EQ(m->index(0,1).data(AntKeyValueBridge::KeyTypeRole).toInt(),
	          quint32(fmp::ValueType::STRING));

	EXPECT_EQ(m->rowCount(),2);

	bridge->setExperiment(experiment);
	EXPECT_FALSE(bridge->isModified());
	EXPECT_EQ(modified.count(),2);
	EXPECT_FALSE(modified.last().at(0).toBool());

	bridge->removeKey("bar");
	EXPECT_EQ(m->rowCount(),1);
	EXPECT_TRUE(bridge->isModified());
	EXPECT_EQ(modified.count(),3);
	EXPECT_TRUE(modified.last().at(0).toBool());

	bridge->setExperiment(experiment);
	EXPECT_FALSE(bridge->isModified());
	EXPECT_EQ(modified.count(),4);
	EXPECT_FALSE(modified.last().at(0).toBool());

	bridge->setKey("foo",fort::Time());
	EXPECT_TRUE(bridge->isModified());
	EXPECT_EQ(modified.count(),5);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(ToStdString(m->index(0,1).data(Qt::DisplayRole).toString()),
	          "Time");
	EXPECT_EQ(m->index(0,1).data(AntKeyValueBridge::KeyTypeRole).toInt(),
	          quint32(fmp::ValueType::TIME));

	ASSERT_NO_THROW({
			auto ant = experiment->CreateAnt();
			ant->SetValue("foo",fort::Time::Parse("2019-11-02T23:46:23.453Z"),fort::Time::SinceEver());
		});

	bridge->setExperiment(experiment);
	EXPECT_FALSE(bridge->isModified());
	EXPECT_EQ(modified.count(),6);
	EXPECT_FALSE(modified.last().at(0).toBool());

	bridge->removeKey("foo");
	EXPECT_FALSE(bridge->isModified());
	EXPECT_EQ(modified.count(),6);
	ASSERT_EQ(m->rowCount(),1);

	bridge->setKey("foo",false);
	EXPECT_FALSE(bridge->isModified());
	EXPECT_EQ(modified.count(),6);
	EXPECT_EQ(ToStdString(m->index(0,1).data(Qt::DisplayRole).toString()),
	          "Time");
	EXPECT_EQ(m->index(0,1).data(AntKeyValueBridge::KeyTypeRole).toInt(),
	          quint32(fmp::ValueType::TIME));

}
