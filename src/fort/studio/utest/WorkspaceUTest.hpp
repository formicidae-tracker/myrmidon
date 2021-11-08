#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/priv/Experiment.hpp>

#include <fort/studio/workspace/AntGeometryWorkspace.hpp>

class ExperimentBridge;

namespace fmp = fort::myrmidon::priv;

class WorkspaceUTest {
protected:
	void SetUp(const fmp::Experiment::Ptr & experiment, Workspace * ws);
	void TearDown();

	fmp::Experiment::Ptr experiment;
	ExperimentBridge   * bridge;
	QMainWindow        * mainWindow;
};
