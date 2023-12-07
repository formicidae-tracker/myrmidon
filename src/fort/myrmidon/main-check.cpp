#include <gtest/gtest.h>

#include "TestSetup.hpp"

#include <cpptrace/cpptrace.hpp>

#include <thread>

static std::optional<std::tuple<int, cpptrace::raw_trace>> signalTrace;

void handler(int sig) {
	signalTrace = std::make_tuple(sig, cpptrace::generate_raw_trace());
	std::thread go([]() {
		std::get<1>(signalTrace.value()).resolve().print();
		exit(1);
	});
	go.detach();
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);

	signal(SIGSEGV, handler);

	cpptrace::register_terminate_handler();

	auto &listeners = ::testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new TestSetup());

	return RUN_ALL_TESTS();
}
