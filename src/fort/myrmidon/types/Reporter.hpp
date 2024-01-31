#pragma once

#include <fort/time/Time.hpp>

namespace fort {
namespace myrmidon {
class ErrorReporter {
public:
	virtual ~ErrorReporter() = default;

	virtual void ReportError(const std::string &error) = 0;
};

class ProgressReporter : public ErrorReporter {
public:
	using Ptr = std::unique_ptr<ProgressReporter>;

	virtual void Update(size_t index, size_t total) = 0;
};

class TimeProgressReporter : public ErrorReporter {
public:
	using Ptr = std::unique_ptr<TimeProgressReporter>;

	virtual void Update(const fort::Time &time) = 0;
};

} // namespace myrmidon
} // namespace fort
