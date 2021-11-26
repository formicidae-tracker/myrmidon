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

class VisualizationWorkspace : public Workspace {
	Q_OBJECT
	Q_PROPERTY(fort::Time inTime
	           READ inTime
	           WRITE setInTime
	           NOTIFY inTimeChanged)
	Q_PROPERTY(fort::Time outTime
	           READ outTime
	           WRITE setOutTime
	           NOTIFY outTimeChanged)
public:
	explicit VisualizationWorkspace(QWidget *parent = 0);
	virtual ~VisualizationWorkspace();

	void jumpToTime(uint32_t spaceID,
	                const fort::Time & time);

	const fort::Time & inTime() const;
	const fort::Time & outTime() const;

signals:
	void inTimeChanged(const fort::Time &);
	void outTimeChanged(const fort::Time &);

protected:
	void initialize(QMainWindow * main,ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions) override;
	void tearDown(const NavigationAction & actions) override;

public slots:
	void setInTime(const fort::Time & v);
	void setOutTime(const fort::Time & v);

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
	fort::Time                   d_inTime,d_outTime;
};
