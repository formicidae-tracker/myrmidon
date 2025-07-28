#include <fort/studio/MainWindow.hpp>

#include <QApplication>

#include <QPointer>

#include <glog/logging.h>

int main(int argc, char **argv) {

	FLAGS_logtostderr = false;
	FLAGS_log_prefix  = false;
	::google::InitGoogleLogging(argv[0]);
	::google::InstallFailureSignalHandler();

	QCoreApplication::setOrganizationName("FORmicidae Tracker");
	QCoreApplication::setOrganizationDomain("formicidae-tracker.github.io");
	QCoreApplication::setApplicationName("FORT Studio");

	QApplication fortStudio(argc, argv);

	MainWindow window;
	window.show();

	if (fortStudio.arguments().size() >= 2 &&
	    fortStudio.arguments()[1] == "--debug-quit") {
		return 0;
	}

	return fortStudio.exec();
}
