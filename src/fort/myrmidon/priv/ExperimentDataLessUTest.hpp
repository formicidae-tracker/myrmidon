#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/priv/Experiment.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

class ExperimentDataLessUTest : public ::testing::Test {
protected:
	static void SetUpTestSuite();
	static void TearDownTestSuite();

	static fs::path experimentPath;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
