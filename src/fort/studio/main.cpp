
#include <filesystem>
#include <memory>

#include <QApplication>
#include <QPointer>
#include <QStandardPaths>

#include <fort/studio/MainWindow.hpp>
#include <fort/studio/widget/Logger.hpp>

#include <slog++/slog++.hpp>

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

	std::cerr << "will write log to " << logDir << std::endl;
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
