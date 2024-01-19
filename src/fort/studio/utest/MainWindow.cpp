#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QSettings>

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/utest-data/UTestData.hpp>

#include <fort/studio/MainWindow.hpp>
#include <fort/studio/bridge/ExperimentBridge.hpp>

#include "ui_MainWindow.h"

class MainWindowUTest : public ::testing::Test {
protected:
	void SetUp() {
		QCoreApplication::setOrganizationName("FORmicidae Tracker");
		QCoreApplication::setOrganizationDomain("fortmicidae-tracker.github.io"
		);
		QCoreApplication::setApplicationName("FORT Studio");
		auto settingsPath = TestSetup::UTestData().Basedir() / "qt-settings";
		fs::remove_all(settingsPath);
		QSettings::setPath(
		    QSettings::NativeFormat,
		    QSettings::UserScope,
		    settingsPath.c_str()
		);

		mainWindow = new MainWindow();
		auto filepath =
		    TestSetup::UTestData().Basedir() / "test-main-window.myrmidon";
		Experiment()->create(filepath.c_str());

		recentFilesActions = {
		    mainWindow->d_ui->recentFile1,
		    mainWindow->d_ui->recentFile2,
		    mainWindow->d_ui->recentFile3,
		    mainWindow->d_ui->recentFile4,
		    mainWindow->d_ui->recentFile5,
		};
	}

	void TearDown() {
		recentFilesActions.clear();
		delete mainWindow;
	}

	ExperimentBridge *Experiment() const {
		return mainWindow->d_experiment;
	}

	MainWindow            *mainWindow;
	std::vector<QAction *> recentFilesActions;
};

std::string ToString(const QString & s ) {
	return s.toUtf8().constData();
}


TEST_F(MainWindowUTest,WindowTitleIsSetAccordingly) {
	std::string filepath = Experiment()->absoluteFilePath().toUtf8().constData();
	EXPECT_EQ(ToString(mainWindow->windowTitle()),
	          "FORmicidae Tracker Studio - " + filepath);

	Experiment()->setExperiment(nullptr);

	EXPECT_EQ(ToString(mainWindow->windowTitle()),
	          "FORmicidae Tracker Studio");

	auto currentVersionFilePath = TestSetup::UTestData().CurrentVersionFile().AbsoluteFilePath;

	Experiment()->open(currentVersionFilePath.c_str());

	EXPECT_EQ(ToString(mainWindow->windowTitle()),
	          "FORmicidae Tracker Studio - " + currentVersionFilePath.string());

	auto saveAsPath = TestSetup::UTestData().Basedir() / "test-main-window-save-as.myrmidon";

	Experiment()->saveAs(saveAsPath.c_str());

	EXPECT_EQ(ToString(mainWindow->windowTitle()),
	          "FORmicidae Tracker Studio - " + saveAsPath.string());
}


TEST_F(MainWindowUTest,RecentFilesEndToEnd) {
	for ( const auto action : recentFilesActions) {
		EXPECT_FALSE(action->isVisible());
	}

	auto basedir = TestSetup::UTestData().Basedir();

	for ( size_t i = 0; i < 10; ++i) {
		auto path = basedir / (std::to_string(i) + ".myrmidon");
		Experiment()->create(path.c_str());
	}

	// current is 9, so the recents are 8,7,6,5,4
	for ( size_t i = 0;i < 5; ++i) {
		auto action = recentFilesActions[i];
		EXPECT_TRUE(action->isVisible());
		EXPECT_TRUE(action->isEnabled());
		EXPECT_EQ(ToString(action->text()),
		          (basedir / (std::to_string(8 - i) + ".myrmidon")).string());
	}

	fs::remove_all(basedir / "7.myrmidon");
	Experiment()->open((basedir  / "6.myrmidon").c_str());
	Experiment()->open((basedir  / "0.myrmidon").c_str());
	struct TestData {
		QAction * Action;
		fs::path  Path;
		bool      Enabled;
	};

	std::vector<TestData> testdata
		= {
		   {
		    recentFilesActions[0],
		    basedir / "6.myrmidon",
		    true
		   },
		   {
		    recentFilesActions[1],
		    basedir / "9.myrmidon",
		    true
		   },
		   {
		    recentFilesActions[2],
		    basedir / "8.myrmidon",
		    true
		   },
		   {
		    recentFilesActions[3],
		    basedir / "7.myrmidon",
		    false
		   },
		   {
		    recentFilesActions[4],
		    basedir / "5.myrmidon",
		    true
		   },
	};
	for ( const auto & d : testdata ) {
		EXPECT_TRUE(d.Action->isVisible());
		EXPECT_EQ(d.Action->isEnabled(),d.Enabled);
		EXPECT_EQ(ToString(d.Action->text()),
		          d.Path.string());
	}

}
