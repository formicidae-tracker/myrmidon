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

void ItemProgress::SetTotal(size_t total) {
	check_py_interrupt();
	ensureTqdm(total);
}

void ItemProgress::Update(size_t current) {
	check_py_interrupt();
	if (d_progress.is_none() == true) {
		return;
	}
	d_progress.attr("update")("n"_a = current - d_last);
	d_last = current;
}

void ItemProgress::ensureTqdm(int total) {
	if (d_progress.is_none() == false) {
		return;
	}
	if (d_description.empty() == true) {
		d_progress = py::module_::import("tqdm").attr("tqdm"
		)("total"_a = total, "ncols"_a = 80);

	} else {
		d_progress = py::module_::import("tqdm").attr("tqdm"
		)("total"_a = total, "ncols"_a = 80, "desc"_a = d_description);
	}
	d_last = 0;
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
	return;
	if (d_progress.is_none() == false) {
		return;
	}
	d_start              = start;
	d_lastMinuteReported = 0;
	int64_t minutes      = std::ceil(end.Sub(start).Minutes());
	d_progress           = py::module_::import("tqdm").attr("tqdm"
    )("total"_a = minutes,
      "desc"_a  = d_description,
      "ncols"_a = 80,
      "unit"_a  = "tracked min");
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
