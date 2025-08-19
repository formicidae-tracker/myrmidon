#include "BindMethods.hpp"
#include <atomic>
#include <cpptrace/cpptrace.hpp>

#include <cpptrace/exceptions.hpp>
#include <cstring>
#include <exception>
#include <execinfo.h>
#include <mutex>
#include <pybind11/pybind11.h>
#include <pyerrors.h>
#include <signal.h>
#include <unistd.h>

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

static std::once_flag handler_installed;

void printBacktrace(int signo, siginfo_t *info, void *context) {
	fprintf(stderr, "got SIGSEGV signal:\n");

	constexpr size_t MAX_STACKSIZE = 128;

	void *stack[MAX_STACKSIZE];
	auto  depth = backtrace(stack, MAX_STACKSIZE);
	auto  msg   = backtrace_symbols(stack, depth);
	for (size_t i = 0; i < depth; ++i) {
		fprintf(stderr, "[%zu]: %s\n", i, msg[i]);
	}
	_exit(1);
}

void installCpptraceHandler() {
	cpptrace::register_terminate_handler();

	struct sigaction action = {0};
	action.sa_flags         = 0;
	action.sa_sigaction     = &printBacktrace;
	if (sigaction(SIGSEGV, &action, NULL) == -1) {
		perror("sigaction");
		_exit(1);
	}
}

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

	std::call_once(handler_installed, installCpptraceHandler);

	py::register_exception_translator([](std::exception_ptr p) {
		if (p == nullptr) {
			return;
		}
		try {
			std::rethrow_exception(p);
		} catch (const cpptrace::overflow_error &e) {
			py::set_error(PyExc_OverflowError, e.what());
		} catch (const cpptrace::range_error &e) {
			py::set_error(PyExc_ValueError, e.what());
		} catch (const cpptrace::out_of_range &e) {
			py::set_error(PyExc_IndexError, e.what());
		} catch (const cpptrace::length_error &e) {
			py::set_error(PyExc_ValueError, e.what());
		} catch (const cpptrace::invalid_argument &e) {
			py::set_error(PyExc_ValueError, e.what());
		} catch (const cpptrace::domain_error &e) {
			py::set_error(PyExc_ValueError, e.what());
		}
	});

#ifdef VERSION_INFO
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
	m.attr("__version__") = MYRMIDON_VERSION;
#endif
}
