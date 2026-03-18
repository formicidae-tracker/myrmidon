#include <cstdlib>
#include <gtest/gtest.h>

#include "TestSetup.hpp"

#include <cpptrace/cpptrace.hpp>

#include <slog++/Config.hpp>
#include <slog++/Level.hpp>
#include <slog++/slog++.hpp>
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

	auto level = slog::Level::Fatal;

	auto dbg = std::getenv("MYRMIDON_DEBUG");
	if (dbg != nullptr) {
		switch (std::atoi(dbg)) {
		case 2:
			level = slog::Level::Error;
			break;
		case 3:
			level = slog::Level::Warn;
			break;
		case 4:
			level = slog::Level::Info;
			break;
		case 5:
			level = slog::Level::Debug;
			break;
		case 6:
			level = slog::Level::Trace;
			break;
		}
	}

	slog::DefaultLogger().SetSink(slog::BuildSink(slog::WithProgramOutput(
	    slog::FromLevel(level),
	    slog::WithFormat(slog::OutputFormat::TEXT)
	)));

	signal(SIGSEGV, handler);

	cpptrace::register_terminate_handler();

	auto &listeners = ::testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new TestSetup());

	return RUN_ALL_TESTS();
}
