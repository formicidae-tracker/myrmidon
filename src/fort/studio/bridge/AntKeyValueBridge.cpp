#include "AntKeyValueBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>
#include <fort/studio/Format.hpp>



#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
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
		if ( oldKey == newKey || fi == d_keys.end() ) {
			return false;
		}
		auto dest = lower_bound(sNewKey);
		try {
			qInfo() << "[AntKeyValueBridge]: Renaming key " << oldKey
			        <<" to " << newKey;
			d_experiment->RenameMetaDataKey(sOldKey,sNewKey);
		} catch ( const std::exception & e ) {
			qCritical() << "[AntKeyValueBridge]: Could not rename key " << oldKey
			            <<" to " << newKey << ": " << e.what();
			return false;
		}

		int srcPos = fi - d_keys.begin();
		int destPos = dest - d_keys.begin();
		int finalPos = destPos > srcPos ? destPos - 1 : destPos;
		if ( finalPos != srcPos ) {
			qInfo() << " Moving " << srcPos << "-" << srcPos << "to" << destPos;
			beginMoveRows(QModelIndex(),
			              srcPos,
			              srcPos,
			              QModelIndex(),
			              destPos);
			std::sort(d_keys.begin(),d_keys.end(),
			          [](const fmp::AntMetadata::Key::Ptr & a,const fmp::AntMetadata::Key::Ptr & b) {
				          return a->Name() < b->Name();
			          });
			endMoveRows();
		}
		emit dataChanged(index(finalPos,0),index(finalPos,0));
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
			d_experiment = nullptr;
			return;
		}
		beginRemoveRows(QModelIndex(),0,d_keys.size());
		d_keys.clear();
		d_experiment = nullptr;
		endRemoveRows();
	}

	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex() ) const override {
		if ( hasIndex(row,column,parent) == false ) {
			return QModelIndex();
		}
		return createIndex(row,column,nullptr);
	}

	QModelIndex parent(const QModelIndex & child) const override {
		return QModelIndex();
	}

	int rowCount(const QModelIndex & parent = QModelIndex() ) const override {
		if ( parent.isValid() ) {
			return 0;
		}
		return d_keys.size();
	}

	int columnCount(const QModelIndex & parent = QModelIndex()) const override {
		return 3;
	};

	QVariant data(const QModelIndex & index, int role) const override {
		if ( hasIndex(index.row(),index.column(),index.parent()) == false ) {
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
		if ( hasIndex(index.row(),index.column(),index.parent() ) == false ) {
			return 0;
		}
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	}

	bool setData(const QModelIndex & index,const QVariant & value, int role ) {
		if ( hasIndex(index.row(),index.column(),index.parent()) == false ) {
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
	DataModel(KeyModel * keyModel, QObject * parent)
		: QAbstractItemModel(parent)
		, d_keyModel(keyModel) {
	}
	virtual ~DataModel() {
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
		if ( d_experiment != nullptr ) {
			tearDownExperiment();
		}
		if ( experiment == nullptr ) {
			return;
		}
		const auto & ants = experiment->Identifier()->Ants();
		auto antSize = ants.size();
		beginInsertRows(QModelIndex(),0,antSize - 1);
		d_experiment = experiment;
		d_ants.reserve(antSize);
		auto numberOfKeys = d_keyModel->rowCount();
		for ( const auto & [antID,ant] : ants ) {
			int antIndex = int(d_ants.size());
			d_ants.push_back(ant);
			d_pointers.insert({std::make_tuple(ant.get(),-1,-1),
			                   std::make_unique<Pointer>(Pointer{.Ant = antIndex,
			                                                     .Key = -1,
			                                                     .Value = -1})});
			for ( int k = 0; k < numberOfKeys; ++k ) {
				d_pointers.insert({std::make_tuple(ant.get(),k,-1),
				                   std::make_unique<Pointer>(Pointer{.Ant = antIndex,
				                                                     .Key = k,
				                                                     .Value = -1})});
				auto keyName = keyNameAt(k);
				try {
					int v = 0;
					for ( const auto & [time,value] : ant->DataMap().at(keyName) ) {
						d_pointers.insert({std::make_tuple(ant.get(),k,v),
						                   std::make_unique<Pointer>(Pointer{.Ant = antIndex,
						                                                     .Key = k,
						                                                     .Value = v})});
						++v;
					}
				} catch ( const std::exception &) {
				}
			}
		}
		endInsertRows();
	}

	void tearDownExperiment() {
		if ( d_experiment == nullptr ) {
			return;
		}
		if ( d_ants.empty() ) {
			d_experiment = nullptr;
			return;
		}
		beginRemoveRows(QModelIndex(),0,d_ants.size());
		d_pointers.clear();
		d_ants.clear();
		endRemoveRows();
	}

	QModelIndex index(int row, int column, const QModelIndex & parent) const override {
		if ( !hasIndex(row,column,parent) ) {
			return QModelIndex();
		}

		try {
			if ( parent.isValid() == false ) {
				return createIndex(row,column,d_pointers.at(std::make_tuple(d_ants.at(row).get(),-1,-1)).get());
			}

			auto pPointer = static_cast<Pointer*>(parent.internalPointer());
			auto ant = d_ants.at(pPointer->Ant).get();
			if ( pPointer->Key < 0 ) {
				return createIndex(row,column,
				                   d_pointers.at(std::make_tuple(ant,row,-1)).get());
			}
			if ( pPointer->Value < 0 ) {
				return createIndex(row,column,
				                   d_pointers.at(std::make_tuple(ant,pPointer->Key,row)).get());
			}
			return QModelIndex();
		} catch ( const std::exception & e ) {
		}

		return QModelIndex();
	}

	QModelIndex parent(const QModelIndex & index) const override {
		if ( !index.isValid() ) {
			return QModelIndex();
		}
		try  {
			auto p = static_cast<Pointer*>(index.internalPointer());
			auto ant = d_ants.at(p->Ant).get();
			if ( p->Value >= 0 ) {
				return createIndex(p->Key,0,
				                   d_pointers.at(std::make_tuple(ant,p->Key,-1)).get());
			}
			if ( p->Key >= 0 ) {
				return createIndex(p->Ant,0,
				                   d_pointers.at(std::make_tuple(ant,-1,-1)).get());
			}
			return QModelIndex();
		} catch ( const std::exception & ) {
		}
		return QModelIndex();
	}

	int rowCount(const QModelIndex & parent) const override {
		if ( parent.isValid() == false ) {
			return d_ants.size();
		}
		auto p = static_cast<Pointer*>(parent.internalPointer());
		if ( p->Value >= 0 ) {
			return 0;
		}
		if ( p->Key >= 0 ) {
			try {
				const auto & ant = *d_ants.at(p->Ant);
				const auto & keyName = keyNameAt(p->Key);
				ant.DataMap().at(keyName).size();
			} catch ( const std::exception & e) {
				return 0;
			}
		}
		return d_keyModel->rowCount();
	}

	int columnCount(const QModelIndex & parent) const override {
		return 4;
	}

	QVariant data(const QModelIndex & index, int role) const override {
		switch(role) {
		case Qt::DisplayRole:
			return displayData(index);
		case AntKeyValueBridge::KeyTypeRole:
			return keyTypeData(index);
		default:
			return QVariant();
		}
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
		if ( role != Qt::DisplayRole
		     || orientation != Qt::Horizontal ) {
			return QVariant();
		}
		switch ( section ) {
		case 0 :
			return tr("AntID");
		case 1:
			return tr("Key");
		case 2:
			return tr("Time");
		case 3:
			return tr("Value");
		}
		return QVariant();
	}


private:
	struct Pointer {
		int Ant;
		int Key;
		int Value;
	};

	QVariant keyTypeData(const QModelIndex & index) const {
		if ( index.isValid() == false ) {
			return QVariant();
		}
		auto p = static_cast<Pointer*>(index.internalPointer());
		if ( p->Key < 0 || p->Key >= d_keyModel->rowCount() ) {
			return QVariant();
		}
		return d_keyModel->index(p->Key,1).data(AntKeyValueBridge::KeyTypeRole);
	}

	QVariant displayData(const QModelIndex & index) const {
		if ( index.isValid() == false ) {
			return QVariant();
		}

		auto p = static_cast<Pointer*>(index.internalPointer());
		if ( p->Value >= 0 ) {
			return displayValue(p,index.column());
		}
		if ( p->Key >= 0 ) {
			return displayKey(p,index.column());
		}
		return displayAnt(p->Ant,index.column());
	}

	QVariant displayAnt(int ant,int column) const {
		if ( column > 0 ) {
			return QVariant();
		}
		try {
			return fm::FormatAntID(d_ants.at(ant)->AntID()).c_str();
		} catch ( const std::exception & ) {
			return QVariant();
		}
	}

	QVariant displayKey(Pointer * p,int column) const {
		try {
			const auto & ant = *d_ants.at(p->Ant);
			const auto & keyName = keyNameAt(p->Key);
			switch (column) {
			case 1:
				return keyName.c_str();
			case 3: {
				if ( ant.DataMap().count(keyName) > 0
				     && ant.DataMap().at(keyName).size() > 0
				     && ant.DataMap().at(keyName).front().first == fort::Time::SinceEver() ) {
					return ToQString(ant.DataMap().at(keyName).front().second);
				}
				return tr("default (%1)").arg(d_keyModel->index(p->Key,2).data(Qt::DisplayRole).toString());
			}
			}
		} catch (const std::exception & ) {
		}
		return QVariant();

	}

	QVariant displayValue(Pointer * p, int column) const {
		try {
			const auto & ant = *d_ants.at(p->Ant);
			const auto & keyName = keyNameAt(p->Key);
			const auto & timedValue = ant.DataMap().at(keyName).at(p->Value);
			switch (column) {
			case 2 :
				return ToQString(timedValue.first);
			case 3:
				return ToQString(timedValue.second);
			}
		} catch ( const std::exception & ) {
		}
		return QVariant();
	}

	std::string keyNameAt(int key) const {
		if ( key < 0 || key >= d_keyModel->rowCount() ) {
			throw std::out_of_range("key index is invalid");
		}
		return ToStdString(d_keyModel->index(key,0).data(Qt::DisplayRole).toString());
	}

	fmp::Experiment::Ptr                     d_experiment;
	KeyModel                               * d_keyModel;
	std::vector<fmp::Ant::Ptr>               d_ants;
	std::map<std::tuple<fmp::Ant*,int,int>,
	         std::unique_ptr<Pointer>>       d_pointers;
};


AntKeyValueBridge::AntKeyValueBridge(QObject * parent)
	: GlobalBridge(parent)
	, d_keyModel(new KeyModel(this))
	, d_dataModel(new DataModel(d_keyModel,this))
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

	if ( d_keyModel->setKey(name,defaultValue) == true ) {
		setModified(true);
	}
}

void AntKeyValueBridge::removeKey(const QString & name) {
	if ( !d_experiment ) {
		return;
	}

	if ( d_keyModel->removeKey(name) == true ) {
		setModified(true);
	}
}

void AntKeyValueBridge::setValue(quint32 antID,
                                 const QString & key,
                                 const fort::Time & time,
                                 const fm::AntStaticValue & value) {
}

void AntKeyValueBridge::clearValue(quint32 antID,
                                   const QString & key,
                                   const fort::Time & time) {
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
	d_dataModel->tearDownExperiment();
	d_keyModel->tearDownExperiment();
}

void AntKeyValueBridge::setUpExperiment() {
	if ( isActive() == false ) {
		return;
	}
	d_keyModel->setUpExperiment(d_experiment);
	d_dataModel->setUpExperiment(d_experiment);
}
