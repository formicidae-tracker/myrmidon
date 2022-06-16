#include "Progress.hpp"

#include <fort/myrmidon/Query.hpp>

namespace py = pybind11;

using namespace pybind11::literals;



ItemProgress::ItemProgress(const std::string & description,
                           bool verbose)
	: d_progress(py::none())
	, d_description(description)
	, d_verbose(verbose)
	, d_last(0) {

}

ItemProgress::~ItemProgress() {
	if ( d_progress.is_none() == true ) {
		return;
	}
	d_progress.attr("close")();
}

void ItemProgress::Update(int current, int total) {
	check_py_interrupt();
	if ( d_verbose == false ) {
		return;
	}
	ensureTqdm(total);
	d_progress.attr("update")("n"_a = current - d_last);
	d_last = current;
}


void ItemProgress::ensureTqdm(int total) {
	if ( d_verbose == false || d_progress.is_none() == false ) {
		return;
	}
	if ( d_description.empty() == true ) {
		d_progress = py::module_::import("tqdm").attr("tqdm")("total"_a = total,
		                                                      "ncols"_a = 80);

	} else {
		d_progress = py::module_::import("tqdm").attr("tqdm")("total"_a = total,
		                                                      "ncols"_a = 80,
		                                                      "desc"_a = d_description);
	}
	d_last = 0;
}


TimeProgress::TimeProgress(const fort::myrmidon::Experiment & e,
                           fort::Time start,
                           fort::Time end,
                           const std::string & description,
                           bool verbose)
	: d_progress(py::none()) {

	if ( verbose == false) {
		return;
	}

	if ( start.IsInfinite() || end.IsInfinite() ) {
		auto dataInfo = fort::myrmidon::Query::GetDataInformations(e);
		if ( start.IsInfinite() ) {
			start = dataInfo.Start;
		}
		if ( end.IsInfinite() ) {
			end = dataInfo.End;
		}
	}

	using namespace pybind11::literals;

	d_start = start;
	int64_t minutes =  std::ceil(end.Sub(start).Minutes());
	d_lastMinuteReported = 0;

	d_progress = py::module_::import("tqdm").attr("tqdm")("total"_a = minutes,
	                                                      "desc"_a = description,
	                                                      "ncols"_a = 80,
	                                                      "unit"_a = "tracked min");
}

TimeProgress::~TimeProgress() {
	if ( d_progress.is_none() == false ) {
		d_progress.attr("close")();
	}
}

void TimeProgress::Update(const fort::Time & t) {
	check_py_interrupt();
	if ( d_progress.is_none() == true ) {
		return;
	}
	using namespace pybind11::literals;

	int64_t minuteEllapsed = std::floor(t.Sub(d_start).Minutes());
	if ( minuteEllapsed > d_lastMinuteReported) {
		d_progress.attr("update")("n"_a = minuteEllapsed - d_lastMinuteReported);
		d_lastMinuteReported = minuteEllapsed;
	}
}
