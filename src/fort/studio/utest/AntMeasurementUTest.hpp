#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/priv/TagCloseUp.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>

#include <fort/myrmidon/utest-data/UTestData.hpp>

class AntMeasurementUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	ExperimentBridge d_experiment;
	std::map<fm::TagID,std::vector<fmp::TagCloseUp::ConstPtr>> d_closeUps;
};
