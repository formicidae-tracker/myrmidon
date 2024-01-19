#include "TrackingVideoWidgetUTest.hpp"

#include <QPainter>

#include <fort/studio/MyrmidonTypes/Conversion.hpp>
#include <fort/studio/bridge/AntDisplayBridge.hpp>
#include <fort/studio/bridge/ConcurrentFrameLoader.hpp>
#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/widget/TrackingVideoWidget.hpp>

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

void TrackingVideoWidgetUTest::SetUp() {
	experiment = new ExperimentBridge();

	widget = new TrackingVideoWidget();

	auto e = fmp::Experiment::Create(
	    TestSetup::UTestData().Basedir() / "tracking-video-widget.myrmidon"
	);

	ants[0] = e->CreateAnt();
	ants[1] = e->CreateAnt();
	ants[2] = e->CreateAnt();

	ants[1]->SetDisplayColor(fm::DefaultPaletteColor(1));
	ants[2]->SetDisplayColor(fm::DefaultPaletteColor(2));

	widget->setup(experiment->antDisplay());
	widget->resize(1920, 1080);
	experiment->setExperiment(e);

	frame = TrackingVideoFrame();
	frame.Data =
	    std::make_shared<fort::video::Frame>(1920, 1080, AV_PIX_FMT_ARGB, 32);
	frame.StartPos = 0;
	frame.EndPos   = 250 * fort::Duration::Millisecond;
	frame.FrameID  = 0;
	frame.AsQImage().fill(QColor(0, 0, 0));

	auto i       = std::make_shared<fm::IdentifiedFrame>();
	i->FrameTime = fort::Time::Now();
	i->Width     = 3840;
	i->Height    = 2160;
	i->Space     = 1;
	i->Positions = fm::IdentifiedFrame::PositionMatrix(2, 5);
	i->Positions << 1, 200, 400, 0.5, 1, 2, 600, 800, -0.5, 1;

	auto c = std::make_shared<fm::CollisionFrame>();

	c->FrameTime  = i->FrameTime;
	c->Space      = 1;
	c->Collisions = {
	    fm::Collision{
	        .IDs   = {1, 2},
	        .Types = fm::InteractionTypes(1, 2),
	        .Zone  = 1,
	    },
	};
	c->Collisions.front().Types << 1, 1;
	frame.TrackingFrame  = i;
	frame.CollisionFrame = c;
	widget->setFocus(0);
	widget->setZoom(1.0);
	widget->setROI(0);
	widget->setOpacity(255);
}

void TrackingVideoWidgetUTest::TearDown() {
	delete widget;
	delete experiment;
}

void TrackingVideoWidgetUTest::Paint(QPainter * painter) {
	widget->paint(painter);
}

TEST_F(TrackingVideoWidgetUTest,BannerIsShown) {
	QImage res(1920,1080,QImage::Format_RGB888);
	QPainter painter(&res);
	Paint(&painter);
	EXPECT_EQ(res.pixelColor(0,0),QColor(0,0,0));
	widget->hideLoadingBanner(false);
	widget->display(frame);
	Paint(&painter);
	EXPECT_EQ(res.pixelColor(0,0),QColor(255,255,255));
	widget->hideLoadingBanner(true);
	Paint(&painter);
	EXPECT_EQ(res.pixelColor(0,0),QColor(0,0,0));
}



TEST_F(TrackingVideoWidgetUTest,AntAreDisplayedCorrectly) {
	QImage res(1920,1080,QImage::Format_RGB888);
	QPainter painter(&res);
	widget->display(frame);
	widget->hideLoadingBanner(true);
	Paint(&painter);

	EXPECT_EQ(res.pixelColor(100,200),
	          Conversion::colorFromFM(ants[0]->DisplayColor()));
	EXPECT_EQ(res.pixelColor(300,400),
	          Conversion::colorFromFM(ants[1]->DisplayColor()));

}


TEST_F(TrackingVideoWidgetUTest,AntVisibilityIsRespected) {
	QImage res(1920,1080,QImage::Format_RGB888);
	QPainter painter(&res);
	widget->display(frame);
	widget->hideLoadingBanner(true);

	experiment->antDisplay()->setAntDisplayStatus(2,fm::Ant::DisplayState::HIDDEN);
	Paint(&painter);

	EXPECT_EQ(res.pixelColor(100,200),
	          Conversion::colorFromFM(ants[0]->DisplayColor()));
	EXPECT_EQ(res.pixelColor(300,400),
	          QColor(0,0,0));

	experiment->antDisplay()->setAntDisplayStatus(2,fm::Ant::DisplayState::SOLO);
	Paint(&painter);
	EXPECT_EQ(res.pixelColor(100,200),
	          QColor(0,0,0));
	EXPECT_EQ(res.pixelColor(300,400),
	          Conversion::colorFromFM(ants[1]->DisplayColor()));
}


TEST_F(TrackingVideoWidgetUTest,AntCollisionAreShown) {
	QImage res(1920,1080,QImage::Format_RGB888);
	QPainter painter(&res);
	widget->display(frame);
	widget->hideLoadingBanner(true);
	widget->setShowCollisions(true);
	Paint(&painter);

	EXPECT_EQ(res.pixelColor(200,300),
	          Conversion::colorFromFM(ants[0]->DisplayColor()));
}


TEST_F(TrackingVideoWidgetUTest,FrameConcurrentLoaderPosition) {
	fort::Duration expectedFrameDuration  = 500;
	std::vector<fort::Duration> ticks = {0,499,498,502,499};
	fort::Duration last = 0;
	for ( auto & t : ticks) {
		t = t + last;
		last = t;
	}

	fmp::DenseMap<fmp::MovieFrameID,fm::IdentifiedFrame::Ptr> frames;

	{
		// If no frame loaded, falls back to inferring position from frame position
		SCOPED_TRACE("NOTHING LOADED");
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,0),0);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,1),500);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,2),1000);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,3),1500);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,4),2000);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,5),2500);
	}

	{
		SCOPED_TRACE("FULLY LOADED");
		fmp::MovieFrameID movieID = 0;
		for ( const auto & t : ticks ) {
			auto time = fort::Time().Add(t);
			auto frame = std::make_shared<fm::IdentifiedFrame>(fm::IdentifiedFrame{.FrameTime = time});
			frames.insert({++movieID,
			               frame});
		}

		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,0),0);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,1),499);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,2),997);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,3),1499);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,4),1998);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,5),2498);

	}

	{
		SCOPED_TRACE("PARTIALLY LOADED");
		frames.erase(1);
		frames.erase(3);
		frames.erase(4);

		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,0),0);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,1),500);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,2),999);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,3),1498);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,4),1999);
		EXPECT_EQ(ConcurrentFrameLoader::moviePositionAt(frames,expectedFrameDuration,5),2499);

	}
}
