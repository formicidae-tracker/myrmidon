#include <gtest/gtest.h>

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

#include <fort/studio/Format.hpp>
#include <fort/studio/bridge/AntKeyValueBridge.hpp>

#include <QAbstractItemModel>

#include <QCompleter>
#include <QRegularExpressionValidator>
#include <QSignalSpy>
#include <QTest>
#include <QValidator>

class AntKeyValueUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	fmp::Experiment::Ptr experiment;
	AntKeyValueBridge   *bridge;
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

TEST_F(AntKeyValueUTest,KeyModel) {
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

void TestChild(const QAbstractItemModel * model,
               const QModelIndex & parent,
               const std::function<void (const QModelIndex & index, int row)> & test) {
	for ( size_t i = 0; i < model->rowCount(parent); ++i ) {
		auto index = model->index(i,0,parent);
		SCOPED_TRACE(ToStdString(index.data(Qt::DisplayRole).toString())
		             + " at "
		             + std::to_string(i));
		test(index,i);
	}
}


TEST_F(AntKeyValueUTest,DataModel) {
	auto m = bridge->dataModel();
	EXPECT_EQ(m->rowCount(),0);

	experiment->CreateAnt();
	experiment->CreateAnt();
	experiment->CreateAnt();

	bridge->setExperiment(experiment);

	EXPECT_EQ(m->rowCount(),3);
	TestChild(m,QModelIndex(),
	          []( const QModelIndex & index, int ) {
		          EXPECT_EQ(index.model()->rowCount(index),0);
	          });

	bridge->setKey("isAlive",true);
	bridge->setKey("behavior",std::string());

	TestChild(m,QModelIndex(),
	          []( const QModelIndex & antIndex, int ) {
		          EXPECT_EQ(antIndex.model()->rowCount(antIndex),2);
		          TestChild(antIndex.model(),antIndex,
		                    [&] ( const QModelIndex & keyIndex, int row) {
			                    EXPECT_EQ(keyIndex.model()->rowCount(keyIndex),0);
			                    EXPECT_EQ(ToStdString(keyIndex.siblingAtColumn(3).data().toString()),
			                              row == 0 ?  "default ()" : "default (true)" );
		                    });
	          });

	bridge->setValue(1,"isAlive",fort::Time::SinceEver(),false);
	auto ant1AliveIndex = m->index(1,0,m->index(0,0));
	EXPECT_EQ(ToStdString(ant1AliveIndex.siblingAtColumn(3).data().toString()),
	          "false");
	EXPECT_EQ(m->rowCount(ant1AliveIndex),0);

	m->setData(ant1AliveIndex.siblingAtColumn(3),"");
	EXPECT_EQ(ToStdString(ant1AliveIndex.siblingAtColumn(3).data().toString()),
	          "default (true)");
	EXPECT_EQ(m->rowCount(ant1AliveIndex),0);

	bridge->setValue(1,"isAlive",fort::Time(),false);
	EXPECT_EQ(ToStdString(ant1AliveIndex.siblingAtColumn(3).data().toString()),
	          "default (true)");
	EXPECT_EQ(m->rowCount(ant1AliveIndex),1);
	auto timedIndex = m->index(0,0,ant1AliveIndex);
	EXPECT_EQ(ToStdString(timedIndex.siblingAtColumn(3).data().toString()),
	          "false");
	EXPECT_EQ(ToStdString(timedIndex.siblingAtColumn(2).data().toString()),
	          fort::Time().Format());


	bridge->setValue(1,"isAlive",fort::Time().Add(-1),true);
	EXPECT_EQ(m->rowCount(ant1AliveIndex),2);
	auto timed1Index = m->index(0,0,ant1AliveIndex);
	EXPECT_EQ(ToStdString(timed1Index.siblingAtColumn(3).data().toString()),
	          "true");
	EXPECT_EQ(ToStdString(timed1Index.siblingAtColumn(2).data().toString()),
	          fort::Time().Add(-1).Format());

	auto timed2Index = m->index(1,0,ant1AliveIndex);
	EXPECT_EQ(ToStdString(timed2Index.siblingAtColumn(3).data().toString()),
	          "false");
	EXPECT_EQ(ToStdString(timed2Index.siblingAtColumn(2).data().toString()),
	          fort::Time().Format());

	m->setData(timed2Index.siblingAtColumn(2),
	           fort::Time().Add(-2).Format().c_str());
	timed1Index = m->index(0,0,ant1AliveIndex);
	EXPECT_EQ(ToStdString(timed1Index.siblingAtColumn(3).data().toString()),
	          "false");
	EXPECT_EQ(ToStdString(timed1Index.siblingAtColumn(2).data().toString()),
	          fort::Time().Add(-2).Format());
	timed2Index = m->index(1,0,ant1AliveIndex);
	EXPECT_EQ(ToStdString(timed2Index.siblingAtColumn(3).data().toString()),
	          "true");
	EXPECT_EQ(ToStdString(timed2Index.siblingAtColumn(2).data().toString()),
	          fort::Time().Add(-1).Format());

	bridge->deleteValue(1,"isAlive",fort::Time().Add(-1));
	EXPECT_EQ(m->rowCount(ant1AliveIndex),1);
	timedIndex = m->index(0,0,ant1AliveIndex);
	EXPECT_EQ(ToStdString(timedIndex.siblingAtColumn(3).data().toString()),
	          "false");
	EXPECT_EQ(ToStdString(timedIndex.siblingAtColumn(2).data().toString()),
	          fort::Time().Add(-2).Format());
}


void TestValidator(QValidator * validator,
                   QString value,
                   QValidator::State expected) {
	SCOPED_TRACE(ToStdString(value));
	int pos;
	EXPECT_EQ(validator->validate(value,pos),
	          expected);
}



TEST_F(AntKeyValueUTest,Validators) {
	{
		SCOPED_TRACE("validatorForType(fm::ValueType::BOOL)");
		TestValidator(bridge->validatorForType(fm::ValueType::BOOL),
		              "true",
		              QValidator::Acceptable);
		TestValidator(bridge->validatorForType(fm::ValueType::BOOL),
	              "false",
		              QValidator::Acceptable);
		TestValidator(bridge->validatorForType(fm::ValueType::BOOL),
		              "fal",
		              QValidator::Intermediate);
		TestValidator(bridge->validatorForType(fm::ValueType::BOOL),
		              "tru",
		              QValidator::Intermediate);
		TestValidator(bridge->validatorForType(fm::ValueType::BOOL),
		              "fal1",
		              QValidator::Invalid);
		TestValidator(bridge->validatorForType(fm::ValueType::BOOL),
		              "tru1",
		              QValidator::Invalid);
	}
	EXPECT_EQ(ToStdString(bridge->validatorForType(fm::ValueType::INT)->metaObject()->className()),
	          "QIntValidator");
	EXPECT_EQ(ToStdString(bridge->validatorForType(fm::ValueType::DOUBLE)->metaObject()->className()),
	          "QDoubleValidator");
	EXPECT_EQ(bridge->validatorForType(fm::ValueType::STRING),
	          nullptr);
	{
		SCOPED_TRACE("validatorForType(fm::ValueType::TIME)");
		auto v = dynamic_cast<QRegularExpressionValidator*>(bridge->validatorForType(fm::ValueType::TIME));
		ASSERT_TRUE(v != nullptr);
		EXPECT_EQ(ToStdString(v->regularExpression().errorString()),
		          "no error");
		TestValidator(bridge->validatorForType(fm::ValueType::TIME),
		              "1970-01-01T23:59:59.123456Z",
		              QValidator::Acceptable);
		TestValidator(bridge->validatorForType(fm::ValueType::TIME),
		              "1970-01-01T23:59:59.123456+01:00",
		              QValidator::Acceptable);
		TestValidator(bridge->validatorForType(fm::ValueType::TIME),
		              "1970-01-01T23:59:59.123456-01:30",
		              QValidator::Acceptable);
		TestValidator(bridge->validatorForType(fm::ValueType::TIME),
		              "1970-01-01T23:59",
		              QValidator::Intermediate);
		TestValidator(bridge->validatorForType(fm::ValueType::TIME),
		              "1970-01-01T33:59:59.123456-01:30",
		              QValidator::Invalid);
	}


}


TEST_F(AntKeyValueUTest,Completer) {
	EXPECT_EQ(bridge->completerForType(fm::ValueType::INT),
	          nullptr);
	EXPECT_EQ(bridge->completerForType(fm::ValueType::DOUBLE),
	          nullptr);

	EXPECT_EQ(bridge->completerForType(fm::ValueType::TIME),
	          nullptr);
	{
		SCOPED_TRACE("completerForType(fm::ValueType::BOOL)");
		ASSERT_FALSE(bridge->completerForType(fm::ValueType::BOOL) == nullptr);
		auto m = bridge->completerForType(fm::ValueType::BOOL)->model();
		EXPECT_EQ(m->rowCount(),2);
		EXPECT_EQ(ToStdString(m->index(0,0).data().toString()),"false");
		EXPECT_EQ(ToStdString(m->index(1,0).data().toString()),"true");
	}

}
