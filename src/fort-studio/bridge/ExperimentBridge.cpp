#include "ExperimentBridge.hpp"

#include <QFileInfo>
#include <QDir>

#include <QDebug>

namespace fm=fort::myrmidon;
namespace fmp=fm::priv;

ExperimentBridge::ExperimentBridge(QObject * parent)
	: Bridge(parent)
	, d_universe(new UniverseBridge(this))
	, d_measurements(new MeasurementBridge(this))
	, d_identifier(new IdentifierBridge(this))
	, d_globalProperties(new GlobalPropertyBridge(this))
	, d_identifiedFrameLoader(new IdentifiedFrameConcurrentLoader(this))
	, d_antShapeTypes(new AntShapeTypeBridge(this)) {

	connectModifications();


	connect(d_globalProperties,
	        &GlobalPropertyBridge::detectionSettingChanged,
	        d_measurements,
	        &MeasurementBridge::onDetectionSettingChanged);

	connect(d_universe,
	        &UniverseBridge::trackingDataDirectoryAdded,
	        d_measurements,
	        &MeasurementBridge::onTDDAdded);
	connect(d_universe,
	        &UniverseBridge::trackingDataDirectoryDeleted,
	        d_measurements,
	        &MeasurementBridge::onTDDDeleted);

}

bool ExperimentBridge::isActive() const {
	return d_experiment.get() != NULL;
}

const fs::path & ExperimentBridge::absoluteFilePath() const {
	if ( !d_experiment ) {
		static fs::path empty;
		return empty;
	}
	return d_experiment->AbsoluteFilePath();
}


bool ExperimentBridge::save() {
	if ( !d_experiment ) {
		return false;
	}
	return saveAs(d_experiment->AbsoluteFilePath().c_str());
}


bool ExperimentBridge::saveAs(const QString & path ) {
	if ( !d_experiment ) {
		qDebug() << "[ExperimentBridge]: Ignoring ExperimentBridge::saveAs(): no experience loaded";
		return false;
	}
	try {
		qDebug() << "[ExperimentBridge]: Calling fort::myrmidon::priv::Experiment::Save('" << path << "')";
		d_experiment->Save(path.toUtf8().constData());
		setModified(false);
		qInfo() << "Saved experiment to '" << path << "'";
	} catch (const std::exception & e ) {
		qCritical() << "Could not save experiment to '"
		            << path << "': " << e.what();
		return false;
	}
	return true;
}


bool ExperimentBridge::open(const QString & path) {
	fmp::Experiment::Ptr experiment;
	try {
		qDebug() << "[ExperimentBridge]: Calling fort::myrmidon::priv::Experiment::Open('" << path << "')";
		experiment = fmp::Experiment::Open(path.toUtf8().constData());
	} catch ( const std::exception & e ) {
		qCritical() << "Could not open '" << path
		            << "': " << e.what();
		return false;
	}
	qInfo() << "Opened experiment file '" << path << "'";
	setExperiment(experiment);
	return true;
}


bool ExperimentBridge::create(const QString & path) {
	fmp::Experiment::Ptr experiment;
	try {
		qDebug() << "[ExperimentBridge]: Calling fort::myrmidon::priv::Experiment::NewFile('" << path << "')";
		experiment = fmp::Experiment::NewFile(path.toUtf8().constData());
	} catch ( const std::exception & e ) {
		qCritical() << "Could not create file '" << path
		            << "': " << e.what();
		return false;
	}
	qInfo() << "Created new experiment file '" << path  << "'";
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

GlobalPropertyBridge * ExperimentBridge::globalProperties() const {
	return d_globalProperties;
}

SelectedAntBridge * ExperimentBridge::selectedAnt() const {
	return d_identifier->selectedAnt();
}

SelectedIdentificationBridge * ExperimentBridge::selectedIdentification() const {
	return selectedAnt()->selectedIdentification();
}

IdentifiedFrameConcurrentLoader * ExperimentBridge::identifiedFrameLoader() const {
	return d_identifiedFrameLoader;
}

AntShapeTypeBridge *  ExperimentBridge::antShapeTypes() const {
	return d_antShapeTypes;
}


void ExperimentBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	qDebug() << "[ExperimentBridge]: setting new fort::myrmidon::priv::Experiment in children";
	d_experiment = experiment;
	d_universe->setExperiment(experiment);
	d_measurements->setExperiment(experiment);
	d_identifier->setExperiment(experiment);
	d_identifier->selectedAnt()->setExperiment(experiment);
	d_identifier->selectedAnt()->selectedIdentification()->setExperiment(experiment);
	d_globalProperties->setExperiment(experiment);
	d_identifiedFrameLoader->setExperiment(experiment);
	d_antShapeTypes->setExperiment(experiment);
	setModified(false);
	emit activated(d_experiment.get() != NULL);
}

void ExperimentBridge::onChildModified(bool modified) {
	qWarning() << "Modified" << modified;
	if ( modified == false ) {
		return;
	}
	setModified(true);
}

void ExperimentBridge::connectModifications() {
	connect(d_universe,
	        &UniverseBridge::modified,
	        this,
	        &ExperimentBridge::onChildModified);

	connect(d_measurements,
	        &MeasurementBridge::modified,
	        this,
	        &ExperimentBridge::onChildModified);

	connect(d_identifier,
	        &IdentifierBridge::modified,
	        this,
	        &ExperimentBridge::onChildModified);

	connect(d_globalProperties,
	        &GlobalPropertyBridge::modified,
	        this,
	        &ExperimentBridge::onChildModified);

	connect(d_identifier->selectedAnt(),
	        &SelectedAntBridge::modified,
	        this,
	        &ExperimentBridge::onChildModified);

	connect(d_identifier->selectedAnt()->selectedIdentification(),
	        &SelectedIdentificationBridge::modified,
	        this,
	        &ExperimentBridge::onChildModified);

	connect(d_antShapeTypes,
	        &AntShapeTypeBridge::modified,
	        this,
	        &ExperimentBridge::onChildModified);

}