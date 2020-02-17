#pragma once

#include <gtest/gtest.h>

#include <myrmidon/priv/Experiment.hpp>
#include "MeasurementBridge.hpp"

class MeasurementUTest : public ::testing::Test {
protected:

	void SetUp();
	void TearDown();

	fmp::Experiment::Ptr experiment;
	MeasurementBridge  * measurements;

};
