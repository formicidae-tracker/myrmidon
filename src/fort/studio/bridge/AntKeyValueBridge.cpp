#include "AntKeyValueBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>
#include <fort/studio/Format.hpp>

#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/studio/MyrmidonTypes/AntMetadata.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>


static std::vector<std::string> MetaDataTypeNames = { "Bool","Int","Double","String","Time" };

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
		auto fi = find(sname);
		if ( fi == d_keys.end() ) {
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
		auto fi = find(sname);
		if ( fi == d_keys.end() ) {
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
		beginRemoveRows(QModelIndex(), pos, pos);
		d_keys.erase(fi);
		endRemoveRows();
		return true;
	}


	bool renameKey(const QString & oldKey, const QString & newKey) {
		auto sOldKey = ToStdString(oldKey);
		auto sNewKey = ToStdString(newKey);
		auto fi = find(sOldKey);
		if ( fi == d_keys.end() ) {
			return false;
		}
		auto dest = lower_bound(sNewKey);
		try {
			qInfo() << "[AntKeyValueBridge]: Renaming key '" << oldKey
			        <<"' to '" << newKey << "'";
			d_experiment->RenameMetaDataKey(sOldKey,sNewKey);
		} catch ( const std::exception & e ) {
			qCritical() << "[AntKeyValueBridge]: Could not rename key '" << oldKey
			            <<"' to '" << newKey << "': " << e.what();
			return false;
		}

		int posSrc = fi - d_keys.begin();
		int posDest = dest - d_keys.end();
		if ( posDest < posSrc || posDest > posSrc + 1 ) {
			beginMoveRows(QModelIndex(),
			              posSrc,
			              posSrc,
			              QModelIndex(),
			              posDest);
			std::sort(d_keys.begin(),d_keys.end(),
			          [](const fmp::AntMetadata::Key::Ptr & a,const fmp::AntMetadata::Key::Ptr & b) {
				          return a->Name() < b->Name();
			          });
			endMoveRows();
		} else {
			emit dataChanged(index(posSrc,0),index(posSrc,0));
		}
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
		beginInsertRows(QModelIndex(),0,keys.size()-1);
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
		if ( parent.isValid() ) {
			return 0;
		}
		return d_keys.size();
	}

	int columnCount(const QModelIndex & child) const override {
		return 3;
	};

	QVariant data(const QModelIndex & index, int role) const override {
		if ( check(index.row(),index.column(),index.parent()) == false ) {
			return QVariant();
		}
		const auto & key = d_keys[index.row()];

		if ( role == Qt::DisplayRole || role == Qt::EditRole ) {
			return displayData(key,index.column());
		}

		if ( role == AntKeyValueBridge::KeyTypeRole ) {
			return QVariant::fromValue(int(key->Type()));
		}

		return QVariant();
	}

	Qt::ItemFlags flags(const QModelIndex &index) const override {
		if ( check(index.row(),index.column(),index.parent() ) == false ) {
			return 0;
		}
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	}

	bool setData(const QModelIndex & index,const QVariant & value, int role ) {
		if ( check(index.row(),index.column(),index.parent()) == false ) {
			return false;
		}
		const auto & key = d_keys[index.row()];
		if ( index.column() == 0 ) {
			return renameKey(key->Name().c_str(),value.toString());
		}
		if ( index.column() == 2 ) {
			return setDefaultValue(*key,value.toString());
		}
		return false;
	}

	bool setDefaultValue(const fmp::AntMetadata::Key & key,
	                     const QString & value) {
		try {
			auto v = fmp::AntMetadata::FromString(key.Type(),ToStdString(value));
			return setKey(key.Name().c_str(),v);
		} catch ( const std::exception & e) {
			qCritical() << "Could not parse " << value << ": " << e.what();
			return false;
		}
		return true;
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
		if ( role != Qt::DisplayRole
		     || orientation != Qt::Horizontal ) {
			return QVariant();
		}
		switch ( section ) {
		case 0 :
			return tr("Key");
		case 1:
			return tr("Type");
		case 2:
			return tr("Default");
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
		beginInsertRows(QModelIndex(),pos,pos);
		d_keys.insert(lower,res);
		endInsertRows();
		return true;
	}

	QVariant displayData(const fmp::AntMetadata::Key::Ptr & k,
	                     int column) const {
		switch (column) {
		case 0:
			return k->Name().c_str();
		case 1:
			return MetaDataTypeNames[int(k->Type())].c_str();
		case 2:
			return ToQString(k->DefaultValue());
		}
		return QVariant();
	}

	std::vector<fmp::AntMetadata::Key::Ptr>::const_iterator
	lower_bound(const std::string & name) {
		return std::lower_bound(d_keys.cbegin(),
		                        d_keys.cend(),
		                        name,
		                        [](const fmp::AntMetadata::Key::Ptr & k,
		                           const std::string & name) {
			                        return k->Name() < name;
		                        });
	}

	std::vector<fmp::AntMetadata::Key::Ptr>::const_iterator
	find(const std::string & name) {
		auto fi = lower_bound(name);
		if ( fi != d_keys.end()
		     && (*fi)->Name() == name ) {
			return fi;
		}
		return d_keys.cend();
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
