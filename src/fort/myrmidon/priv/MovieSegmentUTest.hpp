#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/utils/FileSystem.hpp>

class MovieSegmentUTest : public ::testing::Test {
protected:
	static void SetUpTestSuite();
	static void TearDownTestSuite();

	static fs::path s_basedir;
};
