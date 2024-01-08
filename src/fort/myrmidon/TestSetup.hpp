#pragma once

#include <memory>

#include <gtest/gtest.h>

namespace fort {
namespace myrmidon {
class UTestData;
}
} // namespace fort

class TestSetup : public ::testing::EmptyTestEventListener {
public:
	static const fort::myrmidon::UTestData &UTestData();

private:
	void OnTestProgramStart(const ::testing::UnitTest &unit_test) override;

	void OnTestProgramEnd(const ::testing::UnitTest &unit_test) override;

	static std::unique_ptr<fort::myrmidon::UTestData> s_utestdata;
}; // class TestSetup
