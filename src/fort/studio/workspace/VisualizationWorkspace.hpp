#pragma once

#include "Workspace.hpp"
#include <fort/time/Time.hpp>
class QAction;

class QDockWidget;
class AntDisplayListWidget;

namespace Ui {
class VisualizationWorkspace;
}

class ExperimentBridge;
class TrackingVideoPlayer;
class QTreeView;

class VisualizationWorkspace : public Workspace {
	Q_OBJECT
public:
	explicit VisualizationWorkspace(QWidget *parent = 0);
	virtual ~VisualizationWorkspace();

	void jumpToTime(uint32_t spaceID,
	                const fort::Time & time);

protected:
	void initialize(QMainWindow * main,ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions) override;
	void tearDown(const NavigationAction & actions) override;



private slots :
	void onCopyTimeActionTriggered();

	void onMovieSegmentActivated(const QModelIndex & index);

	void jumpToTimeAction();
private:
	friend class VisualizationWorkspaceUTest;

	void setUpUI();

	ExperimentBridge           * d_experiment;
	Ui::VisualizationWorkspace * d_ui;
	TrackingVideoPlayer        * d_videoPlayer;
	AntDisplayListWidget       * d_antDisplay;
	QDockWidget                * d_antDisplayDock;
	QTreeView                  * d_treeView;
	QDockWidget                * d_segmentListDock;
};
