#include "TagCloseUpBridge.hpp"

#include <fort/myrmidon/TestSetup.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/UniverseBridge.hpp>
#include <fort/studio/bridge/TagCloseUpBridge.hpp>

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>


void TagCloseUpUTest::SetUp() {
	experiment = fmp::Experiment::Create(TestSetup::UTestData().Basedir() / "tag-close-up.myrmidon");
	bridge = new ExperimentBridge();
	bridge->setExperiment(experiment);
	bridge->universe()->addSpace("foo");
}

void TagCloseUpUTest::TearDown() {
	delete bridge;
}


TEST_F(TagCloseUpUTest,ListTagsForNewFolder) {
	auto model = bridge->tagCloseUps()->tagModel();

	EXPECT_EQ(model->rowCount(),0);

	auto tddInfo = TestSetup::UTestData().NestDataDirs().back();

	auto [foo,errors] = fmp::TrackingDataDirectory::Open(tddInfo.AbsoluteFilePath,
	                                                     TestSetup::UTestData().Basedir());
	EXPECT_TRUE(errors.empty());
	auto loaders = foo->PrepareTagCloseUpsLoaders();
	for ( const auto & l : loaders ) {
		EXPECT_NO_THROW({
				l();
			});
	}
	std::map<fm::TagID,size_t> tagCounts;
	for ( const auto & tcu : tddInfo.TagCloseUps ) {
		auto tagID = tcu->TagValue();
		tagCounts.insert({tagID,0});
		tagCounts[tagID]++;
	}

	bridge->universe()->addTrackingDataDirectoryToSpace("foo",foo);

	ASSERT_EQ(model->rowCount(),tagCounts.size());
	for ( const auto & [tagID,count] : tagCounts ) {
		QVariant displayVariant = model->data(model->index(tagID,1));

		// Ensuring that the display variant is not a string but an
		// integer ensure sorting with a proxy model. see issue #200
		EXPECT_EQ(displayVariant.type(),QVariant::ULongLong);
		EXPECT_EQ(displayVariant.toInt(),count);
	}

}
