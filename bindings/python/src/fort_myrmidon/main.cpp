#include "BindMethods.hpp"
#include <atomic>
#include <cpptrace/cpptrace.hpp>

#include <cstring>
#include <execinfo.h>
#include <mutex>
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

#ifdef VERSION_INFO
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
	m.attr("__version__") = MYRMIDON_VERSION;
#endif
}
