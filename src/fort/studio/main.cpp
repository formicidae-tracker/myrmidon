#include <cpptrace/basic.hpp>
#include <cpptrace/utils.hpp>
#include <cstring>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <filesystem>
#include <memory>

#include <QApplication>
#include <QPointer>
#include <QStandardPaths>

#include <fort/studio/MainWindow.hpp>
#include <fort/studio/widget/Logger.hpp>

#include <slog++/slog++.hpp>

#if defined(Q_OS_WIN)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#elif defined(Q_OS_MACOS)
#include <limits.h>
#include <mach-o/dyld.h>
#include <stdlib.h>

#elif defined(Q_OS_LINUX)
#include <limits.h>
#include <unistd.h>
#endif

const char *get_executable_path() {
	// Thread-local fixed buffer as requested (1024 bytes).
	// NUL-terminated on success.
	thread_local char buf[1024];
	buf[0] = '\0';

#if defined(Q_OS_WIN)

	DWORD len =
	    GetModuleFileNameA(nullptr, buf, static_cast<DWORD>(sizeof(buf)));
	if (len == 0)
		return nullptr;
	if (len >= sizeof(buf)) {
		buf[sizeof(buf) - 1] = '\0';
		return nullptr;
	}
	return buf;

#elif defined(Q_OS_LINUX)

	// /proc/self/exe is a symlink to the executable; readlink does not
	// NUL-terminate.
	const ssize_t n = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
	if (n <= 0)
		return nullptr;
	buf[n] = '\0';
	return buf;

#elif defined(Q_OS_MACOS)

	uint32_t sz = static_cast<uint32_t>(sizeof(buf));
	if (_NSGetExecutablePath(buf, &sz) != 0) {
		// buffer too small (sz tells required size)
		buf[0] = '\0';
		return nullptr;
	}

	// Canonicalize (best effort). If realpath fails, return the raw value.
	char resolved[PATH_MAX];
	if (realpath(buf, resolved) == nullptr)
		return buf;

	// Copy back into thread-local buffer
	for (size_t i = 0; i + 1 < sizeof(buf) && resolved[i] != '\0'; ++i) {
		buf[i]     = resolved[i];
		buf[i + 1] = '\0';
	}
	buf[sizeof(buf) - 1] = '\0';
	return buf;

#else
	return nullptr;
#endif
}

void do_signal_safe_trace(cpptrace::frame_ptr *buffer, std::size_t count) {
	// Setup pipe and spawn child
	int                     input_pipe[2];
	static constexpr size_t READ_END  = 0;
	static constexpr size_t WRITE_END = 1;

	pipe(input_pipe);
	const pid_t pid = fork();
	if (pid == -1) {
		const char *fork_failure_message = "fork() failed\n";
		write(
		    STDERR_FILENO,
		    fork_failure_message,
		    strlen(fork_failure_message)
		);
		return;
	}
	if (pid == 0) { // child
		dup2(input_pipe[READ_END], STDIN_FILENO);
		close(input_pipe[READ_END]);
		close(input_pipe[WRITE_END]);
		const char *myself = get_executable_path();
		if (myself == nullptr) {
			_exit(2);
		}
		const char *env[] = {"FORT_STUDIO_SIGNAL_TRACE=1", nullptr};
		execle(myself, myself, nullptr, env);
		const char *exec_failure_message[2] = {
		    "exec(",
		    ") failed: Make sure the signal_tracer "
		    "executable is in "
		    "the current working directory and the binary's permissions are "
		    "correct.\n"
		};
		write(
		    STDERR_FILENO,
		    exec_failure_message[0],
		    strlen(exec_failure_message[0])
		);
		write(STDERR_FILENO, myself, strlen(myself));
		write(
		    STDERR_FILENO,
		    exec_failure_message[1],
		    strlen(exec_failure_message[1])
		);

		_exit(1);
	}
	// Resolve to safe_object_frames and write those to the pipe
	for (std::size_t i = 0; i < count; i++) {
		cpptrace::safe_object_frame frame;
		cpptrace::get_safe_object_frame(buffer[i], &frame);
		write(input_pipe[WRITE_END], &frame, sizeof(frame));
	}
	close(input_pipe[READ_END]);
	close(input_pipe[WRITE_END]);
	// Wait for child
	waitpid(pid, nullptr, 0);
}

void handler(int signo, siginfo_t *info, void *context) {
	// Print basic message
	const char *message = "SIGSEGV occurred:\n";
	write(STDERR_FILENO, message, strlen(message));
	// Generate trace
	constexpr std::size_t N = 100;
	cpptrace::frame_ptr   buffer[N];
	std::size_t           count = cpptrace::safe_generate_raw_trace(buffer, N);
	do_signal_safe_trace(buffer, count);
	// Up to you if you want to exit or continue or whatever
	_exit(1);
}

void warmup_cpptrace() {
	// This is done for any dynamic-loading shenanigans
	cpptrace::frame_ptr buffer[10];
	cpptrace::safe_generate_raw_trace(buffer, 10);
	cpptrace::safe_object_frame frame;
	cpptrace::get_safe_object_frame(buffer[0], &frame);
}

void handle_signal_trace() {
	cpptrace::object_trace trace;
	while (true) {
		cpptrace::safe_object_frame frame;
		std::size_t res = fread(&frame, sizeof(frame), 1, stdin);
		if (res == 0) {
			break;
		} else if (res != 1) {
			std::cerr << "Something went wrong while reading the pipe res="
			          << res << std::endl;
			break;
		} else {
			trace.frames.push_back(frame.resolve());
		}
	}
	trace.resolve().print();
}

void installFailureHandler() {
	if (getenv("FORT_STUDIO_SIGNAL_TRACE") != nullptr) {
		handle_signal_trace();
		exit(0);
	}

	cpptrace::register_terminate_handler();

	warmup_cpptrace();
	struct sigaction action = {};
	action.sa_flags         = 0;
	action.sa_sigaction     = &handler;
	if (sigaction(SIGSEGV, &action, nullptr) == -1) {
		perror("sigaction");
	}
}

std::shared_ptr<Logger> setupLogger() {

	auto logDir = std::filesystem::path{
	    QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
	        .toStdString()
	};
	if (logDir.string().empty()) {
		logDir = std::filesystem::path{
		    QStandardPaths::writableLocation(QStandardPaths::TempLocation)
		        .toStdString()
		};
	}

	std::filesystem::create_directories(logDir);
	auto logger = std::make_shared<Logger>();

#ifndef NDEBUG
	auto stderr    = slog::BuildSink(slog::WithProgramOutput(
        slog::FromLevel(slog::Level::Debug),
        slog::WithFormat(slog::OutputFormat::TEXT),
        slog::WithLocking()
    ));
	auto filedebug = slog::BuildSink(slog::WithFileOutput(
	    logDir / "log.DEBUG",
	    slog::FromLevel(slog::Level::Debug),
	    slog::WithFormat(slog::OutputFormat::JSON),
	    slog::WithLocking()
	));

#else

	auto stderr = slog::BuildSink(slog::WithProgramOutput(
	    slog::FromLevel(slog::Level::INFO),
	    slog::WithFormat(slog::OutputFormat::JSON),
	    slog::WithLocking()
	));
#endif

	auto fileinfo  = slog::BuildSink(slog::WithFileOutput(
        logDir / "log.INFO",
        slog::FromLevel(slog::Level::Info),
        slog::WithFormat(slog::OutputFormat::JSON)
    ));
	auto filewarn  = slog::BuildSink(slog::WithFileOutput(
        logDir / "log.WARNING",
        slog::FromLevel(slog::Level::Warn),
        slog::WithFormat(slog::OutputFormat::JSON),
        slog::WithLocking()
    ));
	auto fileerror = slog::BuildSink(slog::WithFileOutput(
	    logDir / "log.ERROR",
	    slog::FromLevel(slog::Level::Error),
	    slog::WithFormat(slog::OutputFormat::JSON),
	    slog::WithLocking()
	));

#ifndef NDEBUG
	auto sink =
	    slog::TeeSink(logger, stderr, fileerror, filewarn, fileinfo, filedebug);
#else
	auto sink = slog::TeeSink(logger, stderr, fileerror, filewarn, fileinfo);
#endif
	slog::DefaultLogger().SetSink(sink);
	slog::Info(
	    "logs destination",
	    slog::String("path", logDir),
	    slog::Bool("allocateOnStack", sink->AllocateOnStack())
	);
	return logger;
}

int main(int argc, char **argv) {
	installFailureHandler();

	QCoreApplication::setOrganizationName("io.github.formicidae_tracker");
	QCoreApplication::setOrganizationDomain("formicidae-tracker.github.io");
	QCoreApplication::setApplicationName("Studio");

	QApplication fortStudio(argc, argv);

	auto logger = setupLogger();

	MainWindow window{logger};
	window.show();

	if (fortStudio.arguments().size() >= 2 &&
	    fortStudio.arguments()[1] == "--debug-quit") {
		return 0;
	}

	return fortStudio.exec();
}
