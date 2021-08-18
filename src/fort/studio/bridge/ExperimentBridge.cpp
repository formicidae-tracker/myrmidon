#include "ExperimentBridge.hpp"

#include <QFileInfo>
#include <QDir>

#include <QDebug>

#include <fort/myrmidon/priv/Identifier.hpp>

#include <fort/studio/widget/TrackingDataDirectoryLoader.hpp>

#include "UniverseBridge.hpp"
#include "MeasurementBridge.hpp"
#include "GlobalPropertyBridge.hpp"
#include "IdentifierBridge.hpp"
#include "IdentifiedFrameConcurrentLoader.hpp"
#include "AntShapeTypeBridge.hpp"
#include "AntMetadataBridge.hpp"
#include "MovieBridge.hpp"
#include "ZoneBridge.hpp"
#include "StatisticsBridge.hpp"
#include "TagCloseUpBridge.hpp"
#include "AntDisplayBridge.hpp"
#include "AntMeasurementBridge.hpp"
#include "AntShapeBridge.hpp"

namespace fm=fort::myrmidon;
namespace fmp=fm::priv;

ExperimentBridge::~ExperimentBridge() {}

ExperimentBridge::ExperimentBridge(QObject * parent)
	: Bridge(parent)
	, d_universe(new UniverseBridge(this))
	, d_measurements(new MeasurementBridge(this))
	, d_identifier(new IdentifierBridge(this))
	, d_antDisplay(new AntDisplayBridge(this))
	, d_globalProperties(new GlobalPropertyBridge(this))
	, d_identifiedFrameLoader(new IdentifiedFrameConcurrentLoader(this))
	, d_antShapeTypes(new AntShapeTypeBridge(this))
	, d_antMetadata(new AntMetadataBridge(this))
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
	              d_antMetadata,
	              d_movies,
	              d_zones,
	              d_statistics,
	              d_tagCloseUps,
	              d_antMeasurements,
	              d_antShapes,
		})
	, d_selectedID(0) {


	for ( const auto & child : d_children ) {
		connect(child,&Bridge::modified,
		        this,&ExperimentBridge::onChildModified);
		child->initialize(this);
	}

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
		resetChildModified();
		qInfo() << "Saved experiment to '" << path << "'";
	} catch (const std::exception & e ) {
		qCritical() << "Could not save experiment to '"
		            << path << "': " << e.what();
		return false;
	}
	return true;
}


bool ExperimentBridge::open(const QString & path,QWidget * parent) {
	fmp::Experiment::Ptr experiment;
	try {
		qDebug() << "[ExperimentBridge]: Calling fort::myrmidon::priv::Experiment::Open('" << path << "')";
		experiment = fmp::Experiment::Open(path.toUtf8().constData());
	} catch ( const std::exception & e ) {
		qCritical() << "Could not open '" << path
		            << "': " << e.what();
		return false;
	}

	try {
		std::vector<fmp::TrackingDataDirectory::Ptr> tdds;
		for(const auto & [tddURI,tdd] : experiment->TrackingDataDirectories() ) {
			tdds.push_back(tdd);
		}
		TrackingDataDirectoryLoader::EnsureLoaded(tdds,parent);
	} catch ( const std::exception & e ) {
		qCritical() << "Could not open '"
		            << path
		            << "': could not load computed data: "
		            << e.what();
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
		fs::path fpath = path.toUtf8().constData();
		experiment = fmp::Experiment::Create(fpath);
		experiment->Save(fpath);
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

AntDisplayBridge * ExperimentBridge::antDisplay() const {
	return d_antDisplay;
}

GlobalPropertyBridge * ExperimentBridge::globalProperties() const {
	return d_globalProperties;
}

IdentifiedFrameConcurrentLoader * ExperimentBridge::identifiedFrameLoader() const {
	return d_identifiedFrameLoader;
}

AntShapeTypeBridge *  ExperimentBridge::antShapeTypes() const {
	return d_antShapeTypes;
}

AntMetadataBridge *  ExperimentBridge::antMetadata() const {
	return d_antMetadata;
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

void ExperimentBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	qDebug() << "[ExperimentBridge]: setting new fort::myrmidon::priv::Experiment in children";
	d_experiment = experiment;

	for ( const auto & child : d_children ) {
		child->setExperiment(experiment);
	}
	d_identifiedFrameLoader->setExperiment(experiment);
	resetChildModified();

	selectAnt(0);
	emit activated(d_experiment.get() != NULL);
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
	if ( !d_experiment ) {
		return nullptr;
	}
	fmp::Ant::Ptr ant;
	try {
		qDebug() << "[ExperimentBridge]: Calling fort::myrmidon::priv::Experiment::CreateAnt()";
		ant = d_experiment->CreateAnt();
	} catch ( const std::exception & e) {
		qCritical() << "Could not create Ant: " << e.what();
		return nullptr;
	}

	qInfo() << "Created new Ant" << ant->FormattedID().c_str();

	setModified(true);
	emit antCreated(ant->AntID());
	return ant;
}

void ExperimentBridge::deleteAnt(fm::AntID antID) {
	if ( !d_experiment ) {
		qWarning() << "Not removing Ant " << fm::FormatAntID(antID).c_str();
		return;
	}

	try {
		qDebug() << "[ExperimentBridge]: Calling fort::myrmidon::priv::Identifier::DeleteAnt("
		         << fm::FormatAntID(antID).c_str() << ")";
		d_experiment->Identifier()->DeleteAnt(antID);
	} catch (const std::exception & e) {
		qCritical() << "Could not delete Ant '" <<  fm::FormatAntID(antID).c_str()
		            << "': " << e.what();
		return;
	}

	qInfo() << "Deleted Ant " << fm::FormatAntID(antID).c_str();

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

fmp::Ant::ConstPtr ExperimentBridge::ant(fm::AntID aID) const {
	if ( !d_experiment == true ) {
		return fmp::Ant::ConstPtr();
	}
	const auto & ants = d_experiment->Identifier()->Ants();
	auto fi = ants.find(aID);
	if ( fi == ants.cend() ) {
		return fmp::Ant::ConstPtr();
	}
	return fi->second;
}
