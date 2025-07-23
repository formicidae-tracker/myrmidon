#pragma once

#include <slog++/slog++.hpp>

#include <fort/myrmidon/types/Reporter.hpp>

namespace fort {
namespace myrmidon {
struct OpenArguments {
	ProgressReporter::Ptr       Progress         = nullptr;
	std::shared_ptr<slog::Sink> LogSink          = nullptr;
	bool                        LogToStderr      = false;
	bool                        FixCorruptedData = false;
};
} // namespace myrmidon
} // namespace fort
