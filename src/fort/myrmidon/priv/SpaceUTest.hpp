#pragma once

#include <gtest/gtest.h>

#include "TrackingDataDirectory.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class SpaceUTest : public ::testing::Test {
protected:

	static void SetUpTestSuite();
	static void TearDownTestSuite();

	static std::vector<TrackingDataDirectory::Ptr> s_nest;
};

} //namespace priv
} //namespace myrmidon
} //namespace fort
