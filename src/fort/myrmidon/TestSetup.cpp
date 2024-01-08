#include "TestSetup.hpp"

#include <cpptrace/cpptrace.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>
#include <gtest/gtest.h>

void TestSetup::OnTestProgramStart(const ::testing::UnitTest &unit_test) {
	try {
		std::ostringstream oss;
		oss << "myrmidon-test-" << getpid();
		auto tmppath = fort::myrmidon::UTestData::TempDirName();
		fs::remove_all(tmppath);
		s_utestdata = std::make_unique<fort::myrmidon::UTestData>(tmppath);
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}

// Called after all test activities have ended.
void TestSetup::OnTestProgramEnd(const ::testing::UnitTest &unit_test) {
	s_utestdata.reset();
}

std::unique_ptr<fort::myrmidon::UTestData> TestSetup::s_utestdata;

const fort::myrmidon::UTestData &TestSetup::UTestData() {
	return *s_utestdata;
}
