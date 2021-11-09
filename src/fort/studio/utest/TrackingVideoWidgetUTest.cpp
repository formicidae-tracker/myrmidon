#include "TrackingVideoWidgetUTest.hpp"

#include <QPainter>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/widget/TrackingVideoWidget.hpp>
#include <fort/studio/MyrmidonTypes/Conversion.hpp>

#include <fort/myrmidon/TestSetup.hpp>

void TrackingVideoWidgetUTest::SetUp() {
	experiment = new ExperimentBridge();

	widget = new TrackingVideoWidget();

	auto e = fmp::Experiment::Create(TestSetup::UTestData().Basedir() / "tracking-video-widget.myrmidon");

	ants[0] = e->CreateAnt();
	ants[1] = e->CreateAnt();
	ants[2] = e->CreateAnt();

	ants[1]->SetDisplayColor(fm::DefaultPaletteColor(1));
	ants[2]->SetDisplayColor(fm::DefaultPaletteColor(2));

	widget->setup(experiment->antDisplay());
	widget->resize(1920,1080);
	experiment->setExperiment(e);

	frame = TrackingVideoFrame();
	frame.Image = std::make_shared<QImage>(1920,1080,QImage::Format_RGB888);
	frame.StartPos = 0;
	frame.EndPos = 250 * fort::Duration::Millisecond;
	frame.FrameID = 0;
	frame.Image->fill(QColor(0,0,0));


	auto i = std::make_shared<fm::IdentifiedFrame>();
	i->FrameTime = fort::Time::Now();
	i->Width = 3840;
	i->Height = 2160;
	i->Space = 1;
	i->Positions = fm::IdentifiedFrame::PositionMatrix(2,5);
	i->Positions << 1, 200 , 400, 0.5, 1,
		2 , 600, 800, -0.5,1;

	auto c = std::make_shared<fm::CollisionFrame>();

	c->FrameTime = i->FrameTime;
	c->Space = 1;
	c->Collisions = {
	                 fm::Collision{
	                               .IDs = {1,2},
	                               .Types = fm::InteractionTypes(1,2),
	                               .Zone = 1,
	                 },
	};
	c->Collisions.front().Types << 1 ,1;
	frame.TrackingFrame = i;
	frame.CollisionFrame = c;
	widget->setZoomFocus(0,1.0);
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



TEST_F(TrackingVideoWidgetUTest,AntAreDisplayedAtTheRightLocation) {
	QImage res(1920,1080,QImage::Format_RGB888);
	QPainter painter(&res);
	Paint(&painter);
	widget->display(frame);
	widget->hideLoadingBanner(true);
	Paint(&painter);

	EXPECT_EQ(res.pixelColor(100,200),
	          Conversion::colorFromFM(ants[0]->DisplayColor()));
	EXPECT_EQ(res.pixelColor(300,400),
	          Conversion::colorFromFM(ants[1]->DisplayColor()));
}
