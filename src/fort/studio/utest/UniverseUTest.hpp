#pragma once


#include <gtest/gtest.h>

#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/myrmidon/priv/Experiment.hpp>

class UniverseBridge;

class UniverseUTest : public ::testing::Test {
protected:
	static void SetUpTestSuite();

	void SetUp();
	void TearDown();

	static std::vector<fort::myrmidon::priv::TrackingDataDirectory::Ptr> s_nest;

	fort::myrmidon::priv::Experiment::Ptr experiment;
	UniverseBridge                      * universe;
};
