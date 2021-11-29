#include "AntKeyValueBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>
#include <fort/studio/Format.hpp>

#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/studio/MyrmidonTypes/AntMetadata.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>


class KeyModel : public QAbstractItemModel {
public:
	KeyModel(QObject * parent)
		: QAbstractItemModel(parent) {
	}
	virtual ~KeyModel() {
	}

	bool setKey(const QString & name,
	            const fm::AntStaticValue & defaultValue) {
		auto sname = ToStdString(name);
		auto fi = lower_bound(sname);
		if ( fi == d_keys.end() || (*fi)->Name() != sname ) {
			return addKey(sname,defaultValue);
		}

		try {
			qInfo() << "[AntKeyValueBridge]: Setting default value of '"
			        << name << "' to '"
			        << ToQString(defaultValue) << "'";
			(*fi)->SetDefaultValue(defaultValue);
		} catch ( const std::exception & e ) {
			qCritical() << "[AntKeyValueBridge]: Could not set default value of '"
			            << name << "' to '"
			            << ToQString(defaultValue)
			            << "': " << e.what();
			return false;
		}

		int pos = std::distance(d_keys.cbegin(),fi);
		emit dataChanged(index(pos,1),index(pos,2));
		return true;
	}

	bool removeKey(const QString & name) {
		auto sname = ToStdString(name);
		auto fi = lower_bound(sname);
		if ( fi == d_keys.end() || (*fi)->Name() != sname) {
			return false;
		}
		try {
			qInfo() << "[AntKeyValueBridge]: Removing key '" << name << "'";
			d_experiment->DeleteMetaDataKey(sname);
		} catch ( const std::exception & e ) {
			qCritical() << "[AntKeyValueBridge]: Could not remove key '" << name
			            << "': " << e.what();
			return false;
		}

		int pos = std::distance(d_keys.cbegin(),fi);
		beginRemoveRows(QModelIndex(), pos, pos+1);
		d_keys.erase(fi);
		endRemoveRows();
		return true;
	}

	void setUpExperiment(const fmp::Experiment::Ptr & experiment) {
		if ( d_experiment != nullptr ) {
			tearDownExperiment();
		}
		if ( experiment == nullptr ) {
			return;
		}
		const auto & keys = experiment->AntMetadataPtr()->Keys();
		beginInsertRows(QModelIndex(),0,keys.size());
		for ( const auto & [name,k] : keys ) {
			d_keys.push_back(k);
		}
		d_experiment = experiment;
		endInsertRows();
	}

	void tearDownExperiment() {
		if ( d_experiment == nullptr || d_experiment->AntMetadataPtr()->Keys().empty() ) {
			return;
		}
		beginRemoveRows(QModelIndex(),0,d_keys.size());
		d_keys.clear();
		d_experiment = nullptr;
		endRemoveRows();
	}

	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex() ) const override {
		if ( check(row,column,parent) == false ) {
			return QModelIndex();
		}
		return createIndex(row,column,nullptr);
	}

	QModelIndex parent(const QModelIndex & child) const override {
		return QModelIndex();
	}

	int rowCount(const QModelIndex & parent) const override {
		return d_keys.size();
	}
	int columnCount(const QModelIndex & child) const override {
		return 3;
	};

	QVariant data(const QModelIndex & index, int role) const override {
		if ( check(index.row(),index.column(),index.parent()) == false || role != Qt::DisplayRole ) {
			return QVariant();
		}

		auto key = d_keys[index.row()];
		switch ( index.column() ) {
		case 0:
			return QVariant(key->Name().c_str());
		case 1:
			return QVariant("");
		case 2:
			return QVariant(ToQString(key->DefaultValue()));
		}
		return QVariant();
	}
private:
	bool check(int row,int column, const QModelIndex & parent) const {
		return !parent.isValid() && column < 3 && row < d_keys.size();
	}

	bool addKey(const std::string & name,
	            const fm::AntStaticValue & defaultValue) {
		fmp::AntMetadata::Key::Ptr res;
		try {
			qInfo() << "[AntKeyValueBridge]: Adding key '" << name.c_str()
			        << "' with defaultValue '" << ToQString(defaultValue) << "'";
			res = d_experiment->SetMetaDataKey(name,defaultValue);
		} catch ( const std::exception & e ) {
			qCritical() << "[AntKeyValueBridge]: Could not set key '" << name.c_str()
			            << "' to value '" << ToQString(defaultValue) << "':"
			            << e.what();
			return false;
		}
		auto lower = lower_bound(name);
		auto pos = std::distance(d_keys.cbegin(),lower);
		beginInsertRows(QModelIndex(),pos,pos+1);
		d_keys.insert(lower,res);
		endInsertRows();
		return true;
	}

	std::vector<fmp::AntMetadata::Key::Ptr>::const_iterator lower_bound(const std::string & name) {
		return std::lower_bound(d_keys.cbegin(),
		                        d_keys.cend(),
		                        name,
		                        [](const fmp::AntMetadata::Key::Ptr & k, const std::string & name) {
			                        return k->Name() < name;
		                        });
	}

	fmp::Experiment::Ptr                    d_experiment;
	std::vector<fmp::AntMetadata::Key::Ptr> d_keys;
};

class DataModel : public QAbstractItemModel {
public:
	DataModel(QObject * parent)
		: QAbstractItemModel(parent) {
	}
	virtual ~DataModel() {
	}

	void setKey(const QString & name,
	            const fm::AntStaticValue & defaultValue) {
	}
	void removeKey(const QString & name) {
	}

	void SetValue(quint32 antID,
	              const QString & key,
	              const fort::Time & time,
	              const fm::AntStaticValue & value) {
	}

	void DeleteValue(quint32 antID,
	                 const QString & value,
	                 const fort::Time & time) {
	}

	void setUpExperiment(const fmp::Experiment::Ptr & experiment) {
	}
	void tearDownExperiment() {
	}

	QModelIndex index(int row, int column, const QModelIndex & parent) const override {
		return QModelIndex();
	}
	QModelIndex parent(const QModelIndex & child) const override {
		return QModelIndex();
	}

	int rowCount(const QModelIndex & parent) const override {
		return 0;
	}
	int columnCount(const QModelIndex & child) const override {
		return 0;
	}

	QVariant data(const QModelIndex & index, int role) const override {
		return "";
	}
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

	if ( d_keyModel->setKey(name,defaultValue) == false ) {
		return;
	}
	d_dataModel->setKey(name,defaultValue);

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
