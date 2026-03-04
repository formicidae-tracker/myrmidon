#include "ExperimentBridge.hpp"

#include <slog++/slog++.hpp>

#include <QDir>
#include <QFileInfo>
#include <QProgressDialog>
#include <QtConcurrent>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/myrmidon/types/Reporter.hpp>
#include <fort/myrmidon/utils/Exception.hpp>

#include <fort/studio/widget/ProgressDialog.hpp>
#include <fort/studio/widget/TrackingDataDirectoryLoader.hpp>

#include "AntDisplayBridge.hpp"
#include "AntKeyValueBridge.hpp"
#include "AntMeasurementBridge.hpp"
#include "AntShapeBridge.hpp"
#include "AntShapeTypeBridge.hpp"
#include "ConcurrentFrameLoader.hpp"
#include "GlobalPropertyBridge.hpp"
#include "IdentifierBridge.hpp"
#include "MeasurementBridge.hpp"
#include "MovieBridge.hpp"
#include "StatisticsBridge.hpp"
#include "TagCloseUpBridge.hpp"
#include "UniverseBridge.hpp"
#include "ZoneBridge.hpp"

namespace fm  = fort::myrmidon;
namespace fmp = fm::priv;

ExperimentBridge::~ExperimentBridge() {}

ExperimentBridge::ExperimentBridge(QObject *parent)
    : Bridge(parent)
    , d_universe(new UniverseBridge(this))
    , d_measurements(new MeasurementBridge(this))
    , d_identifier(new IdentifierBridge(this))
    , d_antDisplay(new AntDisplayBridge(this))
    , d_globalProperties(new GlobalPropertyBridge(this))
    , d_frameLoader(new ConcurrentFrameLoader(this))
    , d_antShapeTypes(new AntShapeTypeBridge(this))
    , d_antKeyValues(new AntKeyValueBridge(this))
    , d_movies(new MovieBridge(this))
    , d_zones(new ZoneBridge(this))
    , d_statistics(new StatisticsBridge(this))
    , d_tagCloseUps(new TagCloseUpBridge(this))
    , d_antMeasurements(new AntMeasurementBridge(this))
    , d_antShapes(new AntShapeBridge(this))
    , d_children({
          d_universe,
          d_measurements,
          d_identifier,
          d_antDisplay,
          d_globalProperties,
          d_antShapeTypes,
          d_antKeyValues,
          d_movies,
          d_zones,
          d_statistics,
          d_tagCloseUps,
          d_antMeasurements,
          d_antShapes,
      })
    , d_selectedID(0)
    , d_logger{slog::With(slog::String("module", "ExperimentBridge"))} {

	for (const auto &child : d_children) {
		connect(
		    child,
		    &Bridge::modified,
		    this,
		    &ExperimentBridge::onChildModified
		);
		child->initialize(this);
	}
}

bool ExperimentBridge::isActive() const {
	return d_experiment.get() != NULL;
}

const QString & ExperimentBridge::absoluteFilePath() const {
	return d_absoluteFilePath;
}


bool ExperimentBridge::save() {
	if ( !d_experiment ) {
		return false;
	}
	return saveAs(d_experiment->AbsoluteFilePath().c_str());
}

bool ExperimentBridge::saveAs(const QString &path) {
	auto logger = d_logger.With(slog::String("path", path.toStdString()));
	if (!d_experiment) {
		logger.Debug("ignoring: no experience loaded");
		return false;
	}
	try {
		logger.Debug("calling fort::myrmidon::priv::Experiment::Save()");

		d_experiment->Save(path.toUtf8().constData());
		setModified(false);
		resetChildModified();
		logger.Info("saved experiment");
	} catch (const std::exception &e) {
		logger.Error(
		    "could not save experiment",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
		return false;
	}
	setAbsoluteFilePathProperty(path);
	return true;
}

fmp::Experiment::Ptr ExperimentBridge::tryOpen(
    const QString                                      &path,
    std::unique_ptr<fort::myrmidon::ProgressReporter> &&progress
) {
	auto logger = d_logger.With(slog::String("path", path.toStdString()));
	try {
		logger.Debug("calling fort::myrmidon::priv::Experiment::Open()");
		return fmp::Experiment::Open(
		    path.toUtf8().constData(),
		    {.Progress = std::move(progress)}
		);
	} catch (const std::exception &e) {
		logger.Error(
		    "could not open",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
	}

	return nullptr;
}

fmp::Experiment::Ptr
ExperimentBridge::openWithDialog(const QString &path, QWidget *parent) {
	fmp::Experiment::Ptr experiment;
	QFutureWatcher<void> watcher;
	QEventLoop           loop;
	connect(
	    &watcher,
	    &QFutureWatcher<void>::finished,
	    &loop,
	    &QEventLoop::quit
	);
	QProgressDialog	                  *dialog;
	fort::myrmidon::ProgressReporter::Ptr progress;

	std::tie(dialog, progress) = OpenItemProgressDialog(
	    tr("Loading %1 frame references").arg(path),
	    parent
	);

	auto openExperiment = [&experiment, dialog, &progress, &path, this]() {
		experiment = tryOpen(path, std::move(progress));
		dialog->close();
		dialog->reset();
	};

	watcher.setFuture(QtConcurrent::run(openExperiment));

	loop.exec();
	dialog->deleteLater();
	return experiment;
}

bool ExperimentBridge::open(const QString &path, QWidget *parent) {

	fmp::Experiment::Ptr experiment;
	if (parent != nullptr) {
		experiment = openWithDialog(path, parent);
	} else {
		experiment = tryOpen(path, nullptr);
	}

	if (experiment == nullptr) {
		return false;
	}
	auto logger = d_logger.With(slog::String("path", path.toStdString()));
	try {
		std::vector<fmp::TrackingDataDirectory::Ptr> tdds;
		for (const auto &[tddURI, tdd] :
		     experiment->TrackingDataDirectories()) {
			tdds.push_back(tdd);
		}
		TrackingDataDirectoryLoader::EnsureLoaded(tdds, parent);
	} catch (const std::exception &e) {
		logger.Error("could not open experiment file", slog::Err(e));
		return false;
	}
	logger.Info("opened experiment file");
	setExperiment(experiment);
	return true;
}

bool ExperimentBridge::create(const QString &path) {
	fmp::Experiment::Ptr experiment;
	auto logger = d_logger.With(slog::String("path", path.toStdString()));
	try {
		logger.Debug("calling fort::myrmidon::priv::Experiment::NewFile()");
		fs::path fpath = path.toUtf8().constData();
		experiment     = fmp::Experiment::Create(fpath);
		experiment->Save(fpath);
	} catch (const std::exception &e) {
		logger.Error(
		    "could not create file",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
		return false;
	}
	logger.Info("created new experiment file");
	setExperiment(experiment);
	return true;
}

UniverseBridge * ExperimentBridge::universe() const {
	return d_universe;
}

MeasurementBridge * ExperimentBridge::measurements() const {
	return d_measurements;
}

IdentifierBridge * ExperimentBridge::identifier() const {
	return d_identifier;
}

AntDisplayBridge * ExperimentBridge::antDisplay() const {
	return d_antDisplay;
}

GlobalPropertyBridge * ExperimentBridge::globalProperties() const {
	return d_globalProperties;
}

ConcurrentFrameLoader * ExperimentBridge::frameLoader() const {
	return d_frameLoader;
}

AntShapeTypeBridge *  ExperimentBridge::antShapeTypes() const {
	return d_antShapeTypes;
}

AntKeyValueBridge *  ExperimentBridge::antKeyValues() const {
	return d_antKeyValues;
}

MovieBridge *  ExperimentBridge::movies() const {
	return d_movies;
}

ZoneBridge * ExperimentBridge::zones() const {
	return d_zones;
}

StatisticsBridge * ExperimentBridge::statistics() const {
	return d_statistics;
}

TagCloseUpBridge * ExperimentBridge::tagCloseUps() const {
	return d_tagCloseUps;
}

AntMeasurementBridge * ExperimentBridge::antMeasurements() const {
	return d_antMeasurements;
}

AntShapeBridge * ExperimentBridge::antShapes() const {
	return d_antShapes;
}

void ExperimentBridge::setExperiment(const fmp::Experiment::Ptr &experiment) {
	d_logger.Debug("setting new fort::myrmidon::priv::Experiment in children");
	d_experiment = experiment;

	for (const auto &child : d_children) {
		child->setExperiment(experiment);
	}
	d_frameLoader->setExperiment(experiment);
	resetChildModified();

	selectAnt(0);
	if (d_experiment == nullptr) {
		setAbsoluteFilePathProperty("");
		emit activated(false);
	} else {
		setAbsoluteFilePathProperty(d_experiment->AbsoluteFilePath().c_str());
		emit activated(true);
	}
}

void ExperimentBridge::onChildModified(bool modified) {
	if ( modified == false ) {
		return;
	}
	setModified(true);
}

void ExperimentBridge::resetChildModified() {
	for ( const auto & child : d_children ) {
		child->setModified(false);
	}
}

fmp::Ant::Ptr ExperimentBridge::createAnt() {
	if (!d_experiment) {
		return nullptr;
	}
	fmp::Ant::Ptr ant;
	try {
		d_logger.Debug("calling fort::myrmidon::priv::Experiment::CreateAnt()");
		ant = d_experiment->CreateAnt();
	} catch (const std::exception &e) {
		d_logger.Error(
		    "could not create ant",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
		return nullptr;
	}

	d_logger.Info("created new Ant", slog::Int("antID", ant->AntID()));

	setModified(true);
	emit antCreated(ant->AntID());
	return ant;
}

void ExperimentBridge::deleteAnt(fm::AntID antID) {
	auto logger = d_logger.With(slog::Int("antID", antID));
	if (!d_experiment) {
		logger.Warn("not removing Ant: no experiment");
		return;
	}

	try {
		logger.Debug("calling fort::myrmidon::priv::Identifier::DeleteAnt()");
		d_experiment->Identifier()->DeleteAnt(antID);
	} catch (const std::exception &e) {
		logger.Error(
		    "could not delete Ant",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
		return;
	}
	logger.Info("deleted Ant");

	setModified(true);
	emit antDeleted(antID);
}

void ExperimentBridge::selectAnt(quint32 antID) {
	if ( d_selectedID == antID ) {
		return;
	}

	d_selectedID = antID;

	emit antSelected(antID);
}

quint32 ExperimentBridge::selectedAntID() const {
	return d_selectedID;
}

fmp::Ant::Ptr ExperimentBridge::ant(fm::AntID aID) const {
	if ( !d_experiment == true ) {
		return fmp::Ant::Ptr();
	}
	const auto & ants = d_experiment->Identifier()->Ants();
	auto fi = ants.find(aID);
	if ( fi == ants.cend() ) {
		return fmp::Ant::Ptr();
	}
	return fi->second;
}


void ExperimentBridge::setAbsoluteFilePathProperty(const QString & path) {
	if ( path == d_absoluteFilePath) {
		return;
	}
	d_absoluteFilePath = path;
	emit absoluteFilePathChanged(d_absoluteFilePath);
}
