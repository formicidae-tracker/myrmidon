#pragma once

#include <gtest/gtest.h>

class ExperimentBridge;
class TrackingVideoWidget;

#include <fort/studio/widget/TrackingVideoFrame.hpp>

class TrackingVideoWidgetUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	void Paint(QPainter * painter);

	ExperimentBridge    * experiment;
	TrackingVideoWidget * widget;
	TrackingVideoFrame    frame;
};
