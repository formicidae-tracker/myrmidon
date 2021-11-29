#include "AntKeyValueBridge.hpp"

#include <QStandardItemModel>

#include <fort/studio/Format.hpp>

#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/studio/MyrmidonTypes/AntMetadata.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>


class KeyModel : public QAbstractItemModel {
public:
	KeyModel(QObject * parent);
	virtual ~KeyModel();

	bool addKey(const QString & name,
	            const fm::AntStaticValue & defaultValue);
	bool removeKey(const QString & name);

	void setUpExperiment(const fmp::Experiment::Ptr & experiment);
	void tearDownExperiment();

	QModelIndex index(int row, int column, const QModelIndex & parent) const override;
	QModelIndex parent(const QModelIndex & child) const override;

	int rowCount(const QModelIndex & parent) const override;
	int columnCount(const QModelIndex & child) const override;

	QVariant data(const QModelIndex & index, int role) const override;
};

class DataModel : public QAbstractItemModel {
public:
	DataModel(QObject * parent);
	virtual ~DataModel();

	void addKey(const QString & name,
	            const fm::AntStaticValue & defaultValue);
	void removeKey(const QString & name);

	void SetValue(quint32 antID,
	              const QString & key,
	              const fort::Time & time,
	              const fm::AntStaticValue & value);

	void DeleteValue(quint32 antID,
	                 const QString & value,
	                 const fort::Time & time);

	void setUpExperiment(const fmp::Experiment::Ptr & experiment);
	void tearDownExperiment();

	QModelIndex index(int row, int column, const QModelIndex & parent) const override;
	QModelIndex parent(const QModelIndex & child) const override;

	int rowCount(const QModelIndex & parent) const override;
	int columnCount(const QModelIndex & child) const override;

	QVariant data(const QModelIndex & index, int role) const override;
};


AntKeyValueBridge::AntKeyValueBridge(QObject * parent)
	: GlobalBridge(parent)
	, d_keyModel(new KeyModel(this))
	, d_dataModel(new DataModel(this))
	, d_typeModel(new QStandardItemModel(parent)){
	qRegisterMetaType<fort::Time>();
	qRegisterMetaType<fm::AntStaticValue>();

#define add_item_type(t,T,tT) do {	  \
		auto  item = new QStandardItem(#tT); \
		item->setData(quint32(fm::AntMetaDataType::T),KeyTypeRole); \
		d_typeModel->appendRow(item); \
	}while(0)
	add_item_type(bool,BOOL,Bool);
	add_item_type(int,INT,Int);
	add_item_type(double,DOUBLE,Double);
#undef add_item_type

	auto stringItem = new QStandardItem("String");
	stringItem->setData(quint32(fm::AntMetaDataType::STRING),KeyTypeRole);
	d_typeModel->appendRow(stringItem);

	auto timeItem = new QStandardItem("Time");
	timeItem->setData(quint32(fm::AntMetaDataType::TIME),KeyTypeRole);
	d_typeModel->appendRow(timeItem);
}

AntKeyValueBridge::~AntKeyValueBridge() {
}

QAbstractItemModel * AntKeyValueBridge::keyModel() {
	return d_keyModel;
}

QAbstractItemModel * AntKeyValueBridge::dataModel() {
	return d_dataModel;
}

QAbstractItemModel * AntKeyValueBridge::typeModel() {
	return d_typeModel;
}

void AntKeyValueBridge::setKey(const QString & name, const fm::AntStaticValue & defaultValue) {
	if ( !d_experiment ) {
		return;
	}

	if ( d_keyModel->addKey(name,defaultValue) == false ) {
		return;
	}
	d_dataModel->addKey(name,defaultValue);

	setModified(true);
}

void AntKeyValueBridge::removeKey(const QString & name) {
	if ( !d_experiment ) {
		return;
	}

	if ( d_keyModel->removeKey(name) == false ) {
		return;
	}
	d_dataModel->removeKey(name);

	setModified(true);
}

void AntKeyValueBridge::setValue(quint32 antID,
                                 const QString & key,
                                 const fort::Time & time,
                                 const fm::AntStaticValue & value) {

	setModified(true);
}

void AntKeyValueBridge::clearValue(quint32 antID,
                                   const QString & key,
                                   const fort::Time & time) {

	setModified(true);
}

void AntKeyValueBridge::onAntCreated(quint32 antID) {

}

void AntKeyValueBridge::onAntDeleted(quint32 antID) {

}

void AntKeyValueBridge::initialize(ExperimentBridge * experiment) {
	connect(experiment,
	        &ExperimentBridge::antCreated,
	        this,
	        &AntKeyValueBridge::onAntCreated);

	connect(experiment,
	        &ExperimentBridge::antDeleted,
	        this,
	        &AntKeyValueBridge::onAntDeleted);
}

void AntKeyValueBridge::tearDownExperiment() {
	d_keyModel->tearDownExperiment();
	d_dataModel->tearDownExperiment();
}

void AntKeyValueBridge::setUpExperiment() {
	if ( isActive() == false ) {
		return;
	}
	d_keyModel->setUpExperiment(d_experiment);
	d_dataModel->setUpExperiment(d_experiment);
}
