#pragma once

#include <gtest/gtest.h>

#include "Experiment.hpp"

namespace fort {
namespace myrmidon {

class PublicIdentificationUTest : public ::testing::Test {
protected:
	void SetUp();
	void TearDown();

	Experiment::Ptr     experiment;
	Ant::Ptr            ant;
	Identification::Ptr i;
};

} // namespace myrmidon
} // namespace fort
