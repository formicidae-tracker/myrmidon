#include "VisualizationWorkspaceUTest.hpp"

#include <QTreeView>
#include <QSignalSpy>

#include <fort/studio/workspace/VisualizationWorkspace.hpp>
#include <fort/studio/widget/TrackingDataDirectoryLoader.hpp>
#include <fort/studio/widget/TrackingVideoPlayer.hpp>
#include <fort/studio/widget/TrackingVideoControl.hpp>

#include <fort/myrmidon/TestSetup.hpp>

#include "ui_VisualizationWorkspace.h"

void VisualizationWorkspaceUTest::SetUp() {
	ws = new VisualizationWorkspace();
	auto e = fmp::Experiment::Open(TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath);

	std::vector<fmp::TrackingDataDirectory::Ptr> tdds;
	for ( const auto & [URI,tdd] : e->TrackingDataDirectories() ) {
		tdds.push_back(tdd);
	}
	TrackingDataDirectoryLoader::EnsureLoaded(tdds,nullptr);

	WorkspaceUTest::SetUp(e,ws);

	m = TreeView()->model();
	ASSERT_EQ(m->rowCount(),2);
	nestIndex = m->index(0,0);
	forageIndex = m->index(1,0);
	movieIndex = m->index(0,0,nestIndex);

}

void VisualizationWorkspaceUTest::TearDown() {
	delete ws;
	WorkspaceUTest::TearDown();
}

QTreeView * VisualizationWorkspaceUTest::TreeView() const {
	return ws->d_treeView;
}

TrackingVideoPlayer * VisualizationWorkspaceUTest::VideoPlayer() const {
	return ws->d_videoPlayer;
}

TrackingVideoControl * VisualizationWorkspaceUTest::VideoControl() const {
	return ws->d_ui->videoControl;
}

TrackingVideoWidget * VisualizationWorkspaceUTest::VideoWidget() const {
	return ws->d_ui->trackingVideoWidget;
}


void VisualizationWorkspaceUTest::StartMovie() {
	ws->onMovieSegmentActivated(movieIndex);
}


TEST_F(VisualizationWorkspaceUTest,DisplaysSegmentsCorrectly) {
	ASSERT_EQ(m->rowCount(nestIndex),1);
	ASSERT_EQ(m->rowCount(forageIndex),0);
	EXPECT_EQ(m->data(m->index(0,0,nestIndex)).toString().toUtf8().constData(),
	          TestSetup::UTestData().WithVideoDataDir().Start.Format());

}


TEST_F(VisualizationWorkspaceUTest,ShowsBanner) {
	QSignalSpy seekReady(VideoPlayer(),&TrackingVideoPlayer::seekReady);
	EXPECT_TRUE(VideoWidget()->showLoadingBanner());
	EXPECT_FALSE(VideoPlayer()->isSeekReady());
	StartMovie();
	seekReady.wait();
	EXPECT_TRUE(VideoPlayer()->isSeekReady());
	EXPECT_FALSE(VideoWidget()->showLoadingBanner());
}


TEST_F(VisualizationWorkspaceUTest,ShowsIDs) {
	QSignalSpy showID(VideoWidget(),&TrackingVideoWidget::showIDChanged);
	EXPECT_FALSE(VideoWidget()->showID());
	VideoControl()->setShowID(true);
	ASSERT_EQ(showID.count(),1);
	EXPECT_TRUE(showID.last().last().toBool());
	EXPECT_TRUE(VideoWidget()->showID());
}

TEST_F(VisualizationWorkspaceUTest,ShowsCollisions) {
	QSignalSpy showCollisions(VideoWidget(),&TrackingVideoWidget::showCollisionsChanged);
	EXPECT_FALSE(VideoWidget()->showCollisions());
	VideoControl()->setShowCollisions(true);
	ASSERT_EQ(showCollisions.count(),1);
	EXPECT_TRUE(showCollisions.last().last().toBool());
	EXPECT_TRUE(VideoWidget()->showCollisions());
}
