#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace py = pybind11;

#ifndef VERSION_INFO
#include <fort/myrmidon/myrmidon-version.h>
#else
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
#endif


static std::shared_ptr<fort::myrmidon::UTestData> s_utestdata;

void BindUTestData(py::module_ & m) {
	using namespace fort::myrmidon;

	py::object PurePath = py::module_::import("pathlib").attr("PurePath");

	py::class_<UTestData,std::shared_ptr<fort::myrmidon::UTestData>> utestdata(m,"UTestData");



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
		.def_property_readonly("Family",
		                       [=](const UTestData::TDDInfo & i) {
			                       return i.Family;
		                       })
		;


	utestdata.def(py::init<std::string>());
	utestdata.def_property_readonly("Basedir",
	                                [=](const UTestData & ud) {
		                                return PurePath(ud.Basedir().string());
	                                });
	utestdata.def_property_readonly("CurrentVersionFile",
	                                &UTestData::CurrentVersionFile);
	utestdata.def_property_readonly("NestDataDirs",
	                                &UTestData::NestDataDirs);
	utestdata.def_property_readonly("ForagingDataDirs",
	                                &UTestData::ForagingDataDirs);


}

PYBIND11_MODULE(py_fort_myrmidon_utestdata, m) {
	m.doc() = "Unit test data generation for py_fort_myrmidon";


	py::module_::import("py_fort_myrmidon");

	BindUTestData(m);

	m.def("UData",
	      []() {
		      if ( s_utestdata == nullptr ) {
			      s_utestdata = std::make_shared<fort::myrmidon::UTestData>("/tmp/test/foo");
		      }
		      return s_utestdata;
	      });


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = MYRMIDON_VERSION;
#endif
}
