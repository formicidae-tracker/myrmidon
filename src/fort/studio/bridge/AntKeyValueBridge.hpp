#pragma once

#include "Bridge.hpp"

#include <QList>

#include <fort/studio/MyrmidonTypes/Experiment.hpp>
#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/studio/MyrmidonTypes/Value.hpp>

class QStandardItemModel;
class QAbstractItemModel;

class DataModel;
class KeyModel;
class QValidator;
class QCompleter;

class AntKeyValueBridge : public GlobalBridge {
	Q_OBJECT
public :
	AntKeyValueBridge(QObject * parent = nullptr);
	virtual ~AntKeyValueBridge();

	const static int KeyTypeRole = Qt::UserRole+1;
	const static int AntIDRole = Qt::UserRole+2;
	const static int KeyNameRole = Qt::UserRole+3;
	const static int TimeRole = Qt::UserRole+4;

	QAbstractItemModel * keyModel();
	QAbstractItemModel * dataModel();
	QAbstractItemModel * typeModel();

	const std::map<fort::Time,fm::Value> & getValues(quint32 antID,
	                                                 const QString & key);

	void initialize(ExperimentBridge * experiment) override;

	const fm::Value & defaultValue(const QString & key) const;

	QValidator * validatorForType(fm::ValueType type);
	QCompleter * completerForType(fm::ValueType type);

public slots:
	void setKey(const QString & key, const fm::Value & defaultValue);
	void removeKey(const QString & key);


	void setValue(quint32 antID,
	              const QString & key,
	              const fort::Time & time,
	              const fm::Value & value);

	void deleteValue(quint32 antID,
	                 const QString & key,
	                 const fort::Time & time);

	void appendDefaultValue(quint32 antID,
	                        const QString & key);
private slots:
	void onAntCreated(quint32);
	void onAntDeleted(quint32);
	void markModified();
protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;


private:
	void pushCompletion(const fm::Value & value);
	void initializeCompletions();
	void clearCompletions();

	QStandardItemModel                     * d_typeModel;
	KeyModel                               * d_keyModel;
	DataModel                              * d_dataModel;
	std::vector<QValidator*>                 d_validators;
	std::vector<QCompleter*>                 d_completers;
	QStandardItemModel                     * d_stringCompletion;
	std::set<std::string>                    d_stringValues;
};
