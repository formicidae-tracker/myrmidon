#include <csignal>
#include <execinfo.h>

#include <cpptrace/cpptrace.hpp>

#include "BindMethods.hpp"

void printStackHandler(int signo, siginfo_t *info, void *context) {
	fprintf(stderr, "SIGSEGV occured:\n");
	constexpr size_t MAX_STACKSIZE = 100;
	void	        *array[MAX_STACKSIZE];

	auto depth = backtrace(array, MAX_STACKSIZE);
	auto msg   = backtrace_symbols(array, depth);
	for (size_t i = 0; i < depth; ++i) {
		fprintf(stderr, "[%zu]: %s\n", i, msg[i]);
	}
	_exit(1);
}

void installSignalHandler(const std::string &installationPath) {
	static std::once_flag signal_installed;
	std::call_once(signal_installed, [&installationPath]() {
		cpptrace::register_terminate_handler();

		struct sigaction action = {0};
		action.sa_flags         = 0;
		action.sa_sigaction     = &printStackHandler;
		if (sigaction(SIGSEGV, &action, NULL) == -1) {
			throw std::runtime_error("could not set signal handler for SIGSEGV"
			);
		}
	});
}

void BindSignalHandling(py::module_ &m) {
	m.def(
	    "_installSignalHandler",
	    &installSignalHandler,
	    py::arg("installationPath"),
	    R"pydoc(Install signal handler for SIGSEGV and terminate.

These signal handler are installed to display nice C++ stacktrace when a SIGSEGV
is caught.
)pydoc"
	);
}
