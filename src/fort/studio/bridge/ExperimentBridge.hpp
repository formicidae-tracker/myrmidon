#pragma once

#include "Bridge.hpp"

class UniverseBridge;
class MeasurementBridge;
class IdentifierBridge;
class AntDisplayBridge;
class GlobalPropertyBridge;
class ConcurrentFrameLoader;
class AntShapeTypeBridge;
class AntKeyValueBridge;
class MovieBridge;
class ZoneBridge;
class StatisticsBridge;
class TagCloseUpBridge;
class AntMeasurementBridge;
class AntShapeBridge;

class QWidget;

namespace fmp = fort::myrmidon::priv;


class ExperimentBridge : public Bridge {
	Q_OBJECT
	Q_PROPERTY(QString absoluteFilePath
	           READ absoluteFilePath()
	           NOTIFY absoluteFilePathChanged)

public:

	ExperimentBridge(QObject * parent = NULL);
	virtual ~ExperimentBridge();

	const QString & absoluteFilePath() const;

	bool isActive() const override;

	bool open(const QString & path,
	          QWidget * parent = nullptr);

	bool create(const QString & path);

	UniverseBridge * universe() const;

	MeasurementBridge * measurements() const;

	IdentifierBridge * identifier() const;

	AntDisplayBridge * antDisplay() const;

	GlobalPropertyBridge * globalProperties() const;

	ConcurrentFrameLoader * frameLoader() const;

	AntShapeTypeBridge * antShapeTypes() const;

	AntKeyValueBridge * antKeyValues() const;

	MovieBridge * movies() const;

	ZoneBridge * zones() const;

	StatisticsBridge * statistics() const;

	TagCloseUpBridge * tagCloseUps() const;

	AntMeasurementBridge * antMeasurements() const;

	AntShapeBridge * antShapes() const;

	fmp::Ant::Ptr ant(fmp::AntID antID) const;

	void setExperiment(const fmp::Experiment::Ptr & );

	quint32 selectedAntID() const;

public slots:
	bool save();
	bool saveAs(const QString & path);

	fmp::Ant::Ptr createAnt();

	void deleteAnt(quint32 antID);

	void selectAnt(quint32 antID);

signals:
	void absoluteFilePathChanged(const QString & filepath);

	void antCreated(quint32);
	void antDeleted(quint32);

	void antSelected(quint32 antID);

private slots:
	void onChildModified(bool);
private:
	friend class ExperimentBridgeUTest_ActiveModifiedState_Test;

	void resetChildModified();
	void setAbsoluteFilePathProperty(const QString & path);

	fmp::Experiment::Ptr              d_experiment;
	UniverseBridge                  * d_universe;
	MeasurementBridge               * d_measurements;
	IdentifierBridge                * d_identifier;
	AntDisplayBridge                * d_antDisplay;
	GlobalPropertyBridge            * d_globalProperties;
	ConcurrentFrameLoader           * d_frameLoader;
	AntShapeTypeBridge              * d_antShapeTypes;
	AntKeyValueBridge               * d_antKeyValues;
	MovieBridge                     * d_movies;
	ZoneBridge                      * d_zones;
	StatisticsBridge                * d_statistics;
	TagCloseUpBridge                * d_tagCloseUps;
	AntMeasurementBridge            * d_antMeasurements;
	AntShapeBridge                  * d_antShapes;
	const std::vector<GlobalBridge*>  d_children;
	quint32                           d_selectedID;
	QString                           d_absoluteFilePath;
};
