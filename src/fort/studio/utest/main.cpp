#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fort/myrmidon/TestSetup.hpp>

#include <QApplication>
#include <QtGlobal>

#include <qglobal.h>
#include <slog++/Config.hpp>
#include <slog++/slog++.hpp>

void myHandler(
    QtMsgType type, const QMessageLogContext &context, const QString &msg
) {
	auto fileAttr =
	    context.file ? slog::String("file", context.file) : slog::Attribute{};

	auto lineAttr =
	    context.line ? slog::Int("line", context.line) : slog::Attribute{};

	auto location = lineAttr.empty() && fileAttr.empty()
	                    ? slog::Attribute{}
	                    : slog::Group("location", lineAttr, fileAttr);

	auto category = context.category
	                    ? slog::String("qt_category", context.category)
	                    : slog::Attribute{};

	switch (type) {
	case QtDebugMsg:
		slog::Debug(msg.toStdString(), category, location);
		break;

	case QtInfoMsg:
		slog::Info(msg.toStdString(), category, location);
		break;

	case QtWarningMsg:
		slog::Warn(msg.toStdString(), category, location);
		break;

	case QtCriticalMsg:
		slog::Error(msg.toStdString(), category, location);
		break;

	case QtFatalMsg:
		slog::Fatal(msg.toStdString(), category, location);
		break;
	};
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::InitGoogleMock(&argc, argv);

	auto level = slog::Level::Fatal;

	auto dbg = std::getenv("MYRMIDON_DEBUG");
	if (dbg != nullptr) {
		switch (std::atoi(dbg)) {
		case 2:
			level = slog::Level::Error;
			break;
		case 3:
			level = slog::Level::Warn;
			break;
		case 4:
			level = slog::Level::Info;
			break;
		case 5:
			level = slog::Level::Debug;
			break;
		case 6:
			level = slog::Level::Trace;
			break;
		}
	}

	auto format = slog::OutputFormat::TEXT;
	if (std::getenv("MYRMIDON_DEBUG_JSON") != nullptr) {
		format = slog::OutputFormat::JSON;
	}

	slog::DefaultLogger().SetSink(slog::BuildSink(slog::WithProgramOutput(
	    slog::FromLevel(level),
	    slog::WithFormat(format)
	)));

	auto &listeners = ::testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new TestSetup());

	//	qInstallMessageHandler(myHandler);
	QApplication app(argc, argv);
	qInstallMessageHandler(myHandler);
	return RUN_ALL_TESTS();
}
