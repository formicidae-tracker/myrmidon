#pragma once

#include "Workspace.hpp"
#include <fort/studio/MyrmidonTypes/Time.hpp>

class QAction;

class QDockWidget;
class AntDisplayListWidget;

namespace Ui {
class VisualizationWorkspace;
}

class ExperimentBridge;
class TrackingVideoPlayer;
class QTreeView;
class QToolBar;
class QAction;
class SetAntValueDialog;

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

	void updateActionsStates();
	void onActionSetInTime();
	void onActionSetOutTime();
	void onActionClearMarkers();
	void onActionSetValue();

private:
	friend class VisualizationWorkspaceUTest;

	void setUpUI();
	void setUpActions();

	ExperimentBridge           * d_experiment;
	Ui::VisualizationWorkspace * d_ui;
	TrackingVideoPlayer        * d_videoPlayer;
	AntDisplayListWidget       * d_antDisplay;
	QDockWidget                * d_antDisplayDock;
	QTreeView                  * d_treeView;
	QDockWidget                * d_segmentListDock;
	QToolBar                   * d_toolbar;
	QAction                    * d_markIn, *d_markOut, *d_clearMarkers, *d_setValue;
	SetAntValueDialog          * d_setValueDialog;
};
