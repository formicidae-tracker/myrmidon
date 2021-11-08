#include "TrackingVideoWidgetUTest.hpp"

#include <QPainter>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/widget/TrackingVideoWidget.hpp>

#include <fort/myrmidon/TestSetup.hpp>

void TrackingVideoWidgetUTest::SetUp() {
	experiment = new ExperimentBridge();

	widget = new TrackingVideoWidget();

	auto e = fmp::Experiment::Create(TestSetup::UTestData().Basedir() / "tracking-video-widget.myrmidon");


	widget->setup(experiment->antDisplay());

	experiment->setExperiment(e);

	frame = TrackingVideoFrame();
	frame.Image = std::make_shared<QImage>(1920,1080,QImage::Format_Grayscale8);
	frame.StartPos = 0;
	frame.EndPos = 250 * fort::Duration::Millisecond;
	frame.FrameID = 0;
	frame.Image->fill(QColor(0,0,0));
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
	EXPECT_TRUE(res.pixelColor(0,0) == QColor(0,0,0));
	widget->hideLoadingBanner(false);
	widget->display(frame);
	Paint(&painter);
	EXPECT_FALSE(res.pixelColor(0,0) == QColor(0,0,0));
	widget->hideLoadingBanner(true);
	Paint(&painter);
	EXPECT_TRUE(res.pixelColor(0,0) == QColor(0,0,0));
}
