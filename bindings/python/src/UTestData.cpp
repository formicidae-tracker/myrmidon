#include <pybind11/pybind11.h>

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
	py::class_<UTestData,std::shared_ptr<fort::myrmidon::UTestData>>(m,"UTestData")
		.def(py::init<std::string>())
		.def_property_readonly("Basedir",
		                       [](const UTestData & ud) {
			                       return ud.Basedir().string();
		                       })
		;


}

PYBIND11_MODULE(py_fort_myrmidon_utestdata, m) {
	m.doc() = "Unit test data generation for py_fort_myrmidon";

	m.def("UData",
	      []() {
		      if ( s_utestdata == nullptr ) {
			      s_utestdata = std::make_shared<fort::myrmidon::UTestData>("/tmp/test/foo");
		      }
		      return s_utestdata;
	      });

	py::module_::import("py_fort_myrmidon");

	BindUTestData(m);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = MYRMIDON_VERSION;
#endif
}
