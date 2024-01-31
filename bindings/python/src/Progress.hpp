#pragma once

#include <string>

#include <pybind11/pybind11.h>

#include <fort/myrmidon/Experiment.hpp>
#include <fort/myrmidon/types/Reporter.hpp>

class ItemProgress : public fort::myrmidon::ProgressReporter {
public:
	ItemProgress(const std::string &description);
	virtual ~ItemProgress();

	void ReportError(const std::string &error) override;

	void SetTotal(size_t total) override;

	void Update(size_t current) override;

private:
	void ensureTqdm(int total);

	pybind11::object d_progress;
	bool             d_verbose;
	std::string      d_description;
	size_t           d_last;
};

class TimeProgress : public fort::myrmidon::TimeProgressReporter {
public:
	TimeProgress(const std::string &description);
	virtual ~TimeProgress();

	void ReportError(const std::string &error) override;
	void SetBound(const fort::Time &start, const fort::Time &end) override;
	void Update(const fort::Time &t) override;

private:
	pybind11::object d_progress;
	fort::Time       d_start;
	int64_t          d_lastMinuteReported;
};

#define check_py_interrupt() do { \
		if ( PyErr_CheckSignals() != 0 ) { \
			throw py::error_already_set(); \
		} \
	} while(0)
