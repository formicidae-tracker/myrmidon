#pragma once

#include <gtest/gtest.h>

#include "Experiment.hpp"

namespace fort {
namespace myrmidon {

class QueryUTest : public ::testing::Test {
protected:
	Experiment::Ptr experiment;
	void SetUp();
	void TearDown();
};


} // namespace myrmidon
} // namespace fort
