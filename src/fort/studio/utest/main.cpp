#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fort/myrmidon/TestSetup.hpp>

#include <QApplication>
#include <QtGlobal>

#include <slog++/slog++.hpp>

void myHandler(QtMsgType, const QMessageLogContext &, const QString &) {}

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

	slog::DefaultLogger().SetSink(slog::BuildSink(slog::WithProgramOutput(
	    slog::FromLevel(level),
	    slog::WithFormat(slog::OutputFormat::JSON)
	)));

	auto &listeners = ::testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new TestSetup());

	//	qInstallMessageHandler(myHandler);
	QApplication app(argc, argv);

	return RUN_ALL_TESTS();
}
