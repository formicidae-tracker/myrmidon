#include "TestSetup.hpp"

#include <cpptrace/cpptrace.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>
#include <gtest/gtest.h>
extern "C" {
#include <libavutil/log.h>
}

void TestSetup::OnTestProgramStart(const ::testing::UnitTest &unit_test) {
	av_log_set_level(AV_LOG_QUIET);
	std::ostringstream oss;
	oss << "myrmidon-test-" << getpid();
	auto tmppath = fort::myrmidon::UTestData::TempDirName();
	fs::remove_all(tmppath);
	s_utestdata = std::make_unique<fort::myrmidon::UTestData>(tmppath);
}

// Called after all test activities have ended.
void TestSetup::OnTestProgramEnd(const ::testing::UnitTest &unit_test) {
	if (unit_test.Passed()) {
		s_utestdata->CleanUpFilesystem();
	}
	s_utestdata.reset();
}

std::unique_ptr<fort::myrmidon::UTestData> TestSetup::s_utestdata;

const fort::myrmidon::UTestData &TestSetup::UTestData() {
	return *s_utestdata;
}
