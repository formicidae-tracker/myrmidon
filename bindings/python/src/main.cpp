#include "BindMethods.hpp"

#ifndef VERSION_INFO
#include <fort/myrmidon/myrmidon-version.h>
#else
#define STRINGIFY(x)       #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
#endif

#ifndef FM_PYTHON_PACKAGE_NAME
#error "Must define FM_PYTHON_PACKAGE_NAME"
#endif

namespace py = pybind11;

PYBIND11_MODULE(FM_PYTHON_PACKAGE_NAME, m) {
	m.doc() = "Bindings for libfort-myrmidon"; // optional module docstring

	BindTypes(m);
	BindShapes(m);

	BindIdentification(m);
	BindAnt(m);

	BindZone(m);
	BindSpace(m);
	BindTrackingSolver(m);
	BindVideoSegment(m);
	BindExperiment(m);

	BindMatchers(m);
	BindQuery(m);

#ifdef VERSION_INFO
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
	m.attr("__version__") = MYRMIDON_VERSION;
#endif
}
