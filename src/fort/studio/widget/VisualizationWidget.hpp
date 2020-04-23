#pragma once

#include <QWidget>

#include "Navigatable.hpp"

class QAction;

namespace Ui {
class VisualizationWidget;
}

class ExperimentBridge;
class TrackingVideoPlayer;

class VisualizationWidget : public QWidget , public Navigatable {
	Q_OBJECT
public:
	explicit VisualizationWidget(QWidget *parent = 0);
	~VisualizationWidget();

	void setup(ExperimentBridge * experiment);
protected:
	void setUp(const NavigationAction & actions) override;
	void tearDown(const NavigationAction & actions) override;

private slots :
	void onCopyTimeActionTriggered();

private:
	Ui::VisualizationWidget * d_ui;
	TrackingVideoPlayer     * d_videoPlayer;
};
