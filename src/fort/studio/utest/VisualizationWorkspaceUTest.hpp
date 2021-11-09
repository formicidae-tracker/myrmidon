#pragma once

#include "WorkspaceUTest.hpp"

#include <QModelIndex>



class VisualizationWorkspace;
class QTreeView;
class QAbstractItemModel;
class TrackingVideoPlayer;
class TrackingVideoControl;
class TrackingVideoWidget;


class VisualizationWorkspaceUTest : public ::testing::Test, WorkspaceUTest {
protected:
	void SetUp();
	void TearDown();

	QTreeView * TreeView() const;
	TrackingVideoPlayer * VideoPlayer() const;

	TrackingVideoControl * VideoControl() const;
	TrackingVideoWidget * VideoWidget() const;
	const NavigationAction * Actions() const;

	void StartMovie();

	VisualizationWorkspace * ws;
	QAbstractItemModel     * m;
	QModelIndex              nestIndex,forageIndex,movieIndex;

};
