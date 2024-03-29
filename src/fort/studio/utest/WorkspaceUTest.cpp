#include "WorkspaceUTest.hpp"

#include <QMainWindow>
#include <QAction>
#include <fort/studio/bridge/ExperimentBridge.hpp>



void WorkspaceUTest::SetUp(const fmp::Experiment::Ptr & experiment_, Workspace * ws) {
	experiment = experiment_;

	bridge = new ExperimentBridge();
	mainWindow = new QMainWindow();

	actions = new NavigationAction(mainWindow);

	bridge->setExperiment(experiment);
	ws->initialize(mainWindow,bridge);

}

void WorkspaceUTest::TearDown() {
	delete mainWindow;
	delete bridge;
	bridge = nullptr;
}
