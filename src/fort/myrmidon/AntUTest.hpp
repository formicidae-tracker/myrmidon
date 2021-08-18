#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/Experiment.hpp>

namespace fort {
namespace myrmidon {

class PublicAntUTest : public ::testing::Test {
protected:

	void SetUp();
	void TearDown();

	Experiment::Ptr e;

};

} // namespace myrmidon
} // namespace fort
