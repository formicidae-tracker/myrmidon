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

	virtual void SetTotal(size_t total) = 0;

	virtual void Update(size_t index) = 0;
};

class TimeProgressReporter : public ErrorReporter {
public:
	using Ptr = std::unique_ptr<TimeProgressReporter>;

	virtual void SetBound(const fort::Time &start, const fort::Time &end) = 0;

	virtual void Update(const fort::Time &time) = 0;
};

} // namespace myrmidon
} // namespace fort
