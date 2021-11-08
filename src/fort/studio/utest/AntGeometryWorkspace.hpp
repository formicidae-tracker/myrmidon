#pragma once

#include <gtest/gtest.h>

#include "WorkspaceUTest.hpp"


class AntShapeWorkspaceUTest : public testing::Test, public WorkspaceUTest {
protected:
	void SetUp();
	void TearDown();

	AntShapeWorkspace * shapeWs;
};
