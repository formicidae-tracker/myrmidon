#pragma once

#include <memory>

#include <gtest/gtest.h>


namespace fort {
namespace myrmidon {
class UTestData;
} // namespace myrmidon
} // namespace fort

class TestSetup : public ::testing::EmptyTestEventListener {
public:

	static const fort::myrmidon::UTestData & UTestData();

private:

	void OnTestProgramStart(const ::testing::UnitTest& /* unit_test */) override;

	void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override;

	void OnTestStart(const ::testing::TestInfo& test_info) override;

	// Called after a failed assertion or a SUCCEED() invocation.
	void OnTestPartResult(const ::testing::TestPartResult& test_part_result) override;

	// Called after a test ends.
	void OnTestEnd(const ::testing::TestInfo& test_info) override;


	static std::unique_ptr<fort::myrmidon::UTestData> s_utestdata;
};  // class TestSetup
