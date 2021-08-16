#include "TestSetup.hpp"



#include <fort/myrmidon/utest-data/UTestData.hpp>



void TestSetup::OnTestProgramStart(const ::testing::UnitTest& /* unit_test */)  {
	std::ostringstream oss;
	oss << "myrmidon-test-" << getpid();
	auto tmppath = fs::temp_directory_path() / oss.str();
	s_utestdata = std::make_unique<fort::myrmidon::UTestData>(tmppath);

}

// Called after all test activities have ended.
void TestSetup::OnTestProgramEnd(const ::testing::UnitTest& unit_test) {
	s_utestdata.reset();
}

// Called before a test starts.
void TestSetup::OnTestStart(const ::testing::TestInfo& test_info) {}

// Called after a failed assertion or a SUCCEED() invocation.
void TestSetup::OnTestPartResult(const ::testing::TestPartResult& test_part_result) {}

// Called after a test ends.
void TestSetup::OnTestEnd(const ::testing::TestInfo& test_info) {}

std::unique_ptr<fort::myrmidon::UTestData> TestSetup::s_utestdata;
