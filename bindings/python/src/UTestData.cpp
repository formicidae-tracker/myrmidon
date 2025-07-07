#include "BindTypes.hpp"

#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace py = pybind11;

#ifndef VERSION_INFO
#include <fort/myrmidon/myrmidon-version.h>
#else
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
#endif


static std::unique_ptr<fort::myrmidon::UTestData> s_utestdata;

void BindUTestData(py::module_ & m) {
	using namespace fort::myrmidon;

	py::object PurePath = py::module_::import("pathlib").attr("PurePath");

	py::class_<UTestData> utestdata(m,"UTestData");



	py::class_<UTestData::ExperimentInfo>(utestdata,"ExperimentInfo")
		.def_property_readonly("AbsoluteFilePath",
		                       [=](const UTestData::ExperimentInfo & i) {
			                       return PurePath(i.AbsoluteFilePath.string());
		                       })
		;

	py::class_<UTestData::TDDInfo>(utestdata,"TDDInfo")
		.def_property_readonly("AbsoluteFilePath",
		                       [=](const UTestData::TDDInfo & i) {
			                       return PurePath(i.AbsoluteFilePath.string());
		                       })
		.def_readonly("Family",
		              &UTestData::TDDInfo::Family)
		.def_readonly("Start",
		              &UTestData::TDDInfo::Start)
		.def_readonly("End",
		              &UTestData::TDDInfo::End)
		;

	py::class_<UTestData::ExpectedResult>(utestdata,"ExpectedResult")
		.def_readonly("Start",
		              &UTestData::ExpectedResult::Start)
		.def_readonly("End",
		              &UTestData::ExpectedResult::End)
		.def_readonly("MaximumGap",
		              &UTestData::ExpectedResult::MaximumGap)
		.def_readonly("Matches",
		              &UTestData::ExpectedResult::Matches)
		.def_readonly("Trajectories",
		              &UTestData::ExpectedResult::Trajectories)
		.def_readonly("Interactions",
		              &UTestData::ExpectedResult::Interactions)
		.def_readonly("InteractionTrajectories",
		              &UTestData::ExpectedResult::InteractionTrajectories)
		.def("Summarized",
		     &UTestData::ExpectedResult::Summarized)
		.def_readonly("VideoSegments",
		              &UTestData::ExpectedResult::VideoSegments)
		;


	utestdata.def(py::init<std::string>());
	utestdata.def_property_readonly("Basedir",
	                                [=](const UTestData & ud) {
		                                return PurePath(ud.Basedir().string());
	                                });
	utestdata.def_property_readonly("CurrentVersionFile",
	                                &UTestData::CurrentVersionFile,
	                                py::return_value_policy::reference);
	utestdata.def_property_readonly("NestDataDirs",
	                                &UTestData::NestDataDirs,
	                                py::return_value_policy::reference);
	utestdata.def_property_readonly("ForagingDataDirs",
	                                &UTestData::ForagingDataDirs,
	                                py::return_value_policy::reference);

	utestdata.def_property_readonly("ExpectedTagStatistics",
	                                &UTestData::ExpectedTagStatistics,
	                                py::return_value_policy::reference);

	utestdata.def_property_readonly("ExpectedFrames",
	                                &UTestData::ExpectedFrames,
	                                py::return_value_policy::reference);

	utestdata.def_property_readonly("ExpectedResults",
	                                &UTestData::ExpectedResults,
	                                py::return_value_policy::reference);

	utestdata.def_property_readonly("WithVideoDataDir",
	                                &UTestData::WithVideoDataDir,
	                                py::return_value_policy::reference);

	utestdata.def_property_readonly("CorruptedDataDir",
	                                &UTestData::CorruptedDataDir,
	                                py::return_value_policy::reference);



}

PYBIND11_MODULE(fort_myrmidon_utestdata, m) {
	m.doc() = "Unit test data generation for fort_myrmidon";

	py::module_::import("fort_myrmidon");

	BindUTestData(m);

	m.def(
	    "UData",
	    []() {
		    if (!s_utestdata) {
			    auto tmpPath = fort::myrmidon::UTestData::TempDirName();
			    s_utestdata =
			        std::make_unique<fort::myrmidon::UTestData>(tmpPath);
		    }
		    return s_utestdata.get();
	    },
	    py::return_value_policy::reference
	);

	m.add_object("_cleanup", py::capsule([]() { s_utestdata.reset(); }));

#ifdef VERSION_INFO
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
	m.attr("__version__") = MYRMIDON_VERSION;
#endif
}
