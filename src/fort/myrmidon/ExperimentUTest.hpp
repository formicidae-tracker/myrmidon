#pragma once

#include <gtest/gtest.h>

#include "Experiment.hpp"

namespace fort {
namespace myrmidon {

class PublicExperimentUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	Experiment::Ptr experiment;
};

} // namespace myrmidon
} // namespace fort
