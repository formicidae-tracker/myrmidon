#include "Progress.hpp"

#include <fort/myrmidon/Query.hpp>
#include <pybind11/cast.h>

namespace py = pybind11;

using namespace pybind11::literals;

ItemProgress::ItemProgress(const std::string &description)
    : d_progress(py::none())
    , d_description(description) {}

ItemProgress::~ItemProgress() {
	if (d_progress.is_none() == true) {
		return;
	}
	d_progress.attr("close")();
}

void ItemProgress::AddTotal(size_t delta) {
	check_py_interrupt();
	d_total += delta;
	if (ensureTqdm() == false) {
		d_progress.attr("total") = d_total;
		d_progress.attr("refresh")();
	}
}

void ItemProgress::Add(size_t delta) {
	check_py_interrupt();
	if (d_progress.is_none() == true) {
		return;
	}
	d_progress.attr("update")("n"_a = delta);
}

bool ItemProgress::ensureTqdm() {
	if (d_progress.is_none() == false) {
		return false;
	}

	if (d_description.empty() == true) {
		d_progress =
		    py::module_::import("tqdm.auto").attr("tqdm")("total"_a = d_total);
	} else {
		d_progress =
		    py::module_::import("tqdm.auto")
		        .attr("tqdm")("total"_a = d_total, "desc"_a = d_description);
	}
	return true;
}

TimeProgress::TimeProgress(const std::string &description)
    : d_progress{py::none()}
    , d_description{description} {}

TimeProgress::~TimeProgress() {
	if (d_progress.is_none() == false) {
		d_progress.attr("close")();
	}
}

void TimeProgress::SetBound(const fort::Time &start, const fort::Time &end) {
	check_py_interrupt();
	if (d_progress.is_none() == false) {
		return;
	}
	d_start              = start;
	d_lastMinuteReported = 0;
	int64_t minutes      = std::ceil(end.Sub(start).Minutes());
	d_progress =
	    py::module_::import("tqdm.auto")
	        .attr("tqdm"
	        )("total"_a = minutes, "desc"_a = d_description, "unit"_a = "min");
}

void TimeProgress::Update(const fort::Time &t) {
	check_py_interrupt();
	if (d_progress.is_none() == true) {
		return;
	}
	using namespace pybind11::literals;

	int64_t minuteEllapsed = std::floor(t.Sub(d_start).Minutes());
	if (minuteEllapsed > d_lastMinuteReported) {
		d_progress.attr("update"
		)("n"_a = minuteEllapsed - d_lastMinuteReported);
		d_lastMinuteReported = minuteEllapsed;
	}
}

void TimeProgress::ReportError(const std::string &error) {
	std::cerr << error << std::endl;
}

void ItemProgress::ReportError(const std::string &error) {
	std::cerr << error << std::endl;
}
