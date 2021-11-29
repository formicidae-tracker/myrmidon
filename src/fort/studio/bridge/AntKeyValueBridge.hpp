#pragma once

#include "Bridge.hpp"

#include <QList>

#include <fort/studio/MyrmidonTypes/Experiment.hpp>
#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/studio/MyrmidonTypes/AntMetadata.hpp>

class QStandardItemModel;
class QAbstractItemModel;

class DataModel;
class KeyModel;

class AntKeyValueBridge : public GlobalBridge {
	Q_OBJECT
public :
	AntKeyValueBridge(QObject * parent = nullptr);
	virtual ~AntKeyValueBridge();

	const static int KeyTypeRole = Qt::UserRole+1;

	QAbstractItemModel * keyModel();
	QAbstractItemModel * dataModel();
	QAbstractItemModel * typeModel();

	void initialize(ExperimentBridge * experiment) override;

public slots:
	void setKey(const QString & key, const fm::AntStaticValue & defaultValue);
	void removeKey(const QString & key);

	void setValue(quint32 antID,
	              const QString & key,
	              const fort::Time & time,
	              const fm::AntStaticValue & value);

	void clearValue(quint32 antID,
	                const QString & key,
	                const fort::Time & time);

private slots:
	void onAntCreated(quint32);
	void onAntDeleted(quint32);

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;

private:
	QStandardItemModel                     * d_typeModel;
	DataModel                              * d_dataModel;
	KeyModel                               * d_keyModel;
};
