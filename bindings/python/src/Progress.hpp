#pragma once

#include <string>

#include <pybind11/pybind11.h>

#include <fort/myrmidon/Experiment.hpp>

class ItemProgress {
public:
	ItemProgress(const std::string & description,
	             bool verbose);
	~ItemProgress();

	void Update(int current, int total);

private:
	void ensureTqdm(int total);

	pybind11::object d_progress;
	bool d_verbose;
	std::string d_description;
	int d_last;
};

class TimeProgress {
public:
	TimeProgress(const fort::myrmidon::Experiment & e,
	              fort::Time start,
	              fort::Time end,
	              const std::string & description,
	              bool verbose = true);
	~TimeProgress();

	void Update(const fort::Time & t);

private:
	pybind11::object d_progress;
	fort::Time d_start;
	int64_t d_lastMinuteReported;

};

#define check_py_interrupt() do { \
		if ( PyErr_CheckSignals() != 0 ) { \
			throw py::error_already_set(); \
		} \
	} while(0)
