#include "AntKeyValueBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>
#include <QRegularExpressionValidator>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QCompleter>

#include <fort/studio/Format.hpp>


#include <fort/myrmidon/types/ValueUtils.hpp>
#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/studio/MyrmidonTypes/Value.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>


class KeyModel : public QAbstractItemModel {
	Q_OBJECT
public:
	KeyModel(QObject * parent)
		: QAbstractItemModel(parent) {
	}
	virtual ~KeyModel() {
	}

	bool setKey(const QString & name,
	            const fm::Value & defaultValue) {
		auto sname = ToStdString(name);
		auto fi = find(sname);
		if ( fi == d_keys.end() ) {
			return addKey(sname,defaultValue);
		}

		try {
			qInfo() << "[AntKeyValueBridge]: Setting default value of '"
			        << name << "' to '"
			        << ToQString(defaultValue) << "'";
			d_experiment->SetMetaDataKey((*fi)->Name(),defaultValue);
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

	int find(const QString & key) const {
		auto fi = find(ToStdString(key));
		if ( fi == d_keys.cend() ) {
			return -1;
		}
		return fi - d_keys.cbegin();
	}

	const fm::Value & defaultValue(const QString & key) const {
		return defaultValue(find(key));
	}

	const fm::Value & defaultValue(int key) const {
		if ( key < 0 || key >= d_keys.size() ) {
			static fm::Value invalid = fort::Time::Forever();
			return invalid;
		}
		return d_keys.at(key)->DefaultValue();
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
		if ( d_experiment == nullptr || d_keys.empty() ) {
			d_experiment = nullptr;
			return;
		}
		beginRemoveRows(QModelIndex(),0,d_keys.size()-1);
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

		if ( role == AntKeyValueBridge::KeyNameRole ) {
			return ToQString(key->Name());
		}

		return QVariant();
	}

	Qt::ItemFlags flags(const QModelIndex &index) const override {
		if ( hasIndex(index.row(),index.column(),index.parent() ) == false ) {
			return 0;
		}
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
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
			auto v = fm::ValueUtils::Parse(key.Type(),ToStdString(value));
			return setKey(key.Name().c_str(),v);
		} catch ( const std::exception & e) {
			qCritical() << "Could not parse " << value << ": " << e.what();
			return false;
		}
		return true;
	}


private:
	bool addKey(const std::string & name,
	            const fm::Value & defaultValue) {
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
			return ToQString(fm::ValueUtils::TypeName(k->Type()));
		case 2:
			return ToQString(k->DefaultValue());
		}
		return QVariant();
	}

	std::vector<fmp::AntMetadata::Key::Ptr>::const_iterator
	lower_bound(const std::string & name) const {
		return std::lower_bound(d_keys.cbegin(),
		                        d_keys.cend(),
		                        name,
		                        [](const fmp::AntMetadata::Key::Ptr & k,
		                           const std::string & name) {
			                        return k->Name() < name;
		                        });
	}

	std::vector<fmp::AntMetadata::Key::Ptr>::const_iterator
	find(const std::string & name) const {
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
	Q_OBJECT
public:
	DataModel(KeyModel * keyModel, QObject * parent)
		: QAbstractItemModel(parent)
		, d_keyModel(keyModel) {

		connect(d_keyModel,&QAbstractItemModel::rowsAboutToBeInserted,
		        this,&DataModel::onKeyModel_layoutAboutToBeChanged);

		connect(d_keyModel,&QAbstractItemModel::rowsAboutToBeMoved,
		        this,&DataModel::onKeyModel_layoutAboutToBeChanged);

		connect(d_keyModel,&QAbstractItemModel::rowsAboutToBeRemoved,
		        this,&DataModel::onKeyModel_layoutAboutToBeChanged);


		connect(d_keyModel,&QAbstractItemModel::rowsInserted,
		        this,&DataModel::onKeyModel_layoutChanged);

		connect(d_keyModel,&QAbstractItemModel::rowsMoved,
		        this,&DataModel::onKeyModel_layoutChanged);

		connect(d_keyModel,&QAbstractItemModel::rowsRemoved,
		        this,&DataModel::onKeyModel_layoutChanged);

		connect(d_keyModel,&QAbstractItemModel::dataChanged,
		        this,&DataModel::onKeyModel_dataChanged);

	}
	virtual ~DataModel() {
	}

	void resetInternalModel() {
		beginResetModel();
		rebuildPointers();
		endResetModel();
	}

	bool appendDefaultValue(quint32 antID,
	                        const QString & key) {
		auto fi = find(antID);
		if ( fi == d_ants.cend() ) {
			return false;
		}
		const auto & ant = **fi;
		auto keyRow = d_keyModel->find(key);
		if ( keyRow < 0) {
			return false;
		}
		fort::Time time;
		try {
			const auto & values = ant.DataMap().at(ToStdString(key));
			if ( values.empty() == false
			     && values.back().first.IsInfinite() == false ) {
				time = values.back().first.Add(fort::Duration::Minute);
			}
		} catch ( const std::exception &) {
		}
		return setValue(antID,keyRow,time,d_keyModel->defaultValue(keyRow));
	}

	bool setValue(quint32 antID,
	              const QString & key,
	              const fort::Time & time,
	              const fm::Value & value) {
		int keyIndex = d_keyModel->find(key);
		if ( keyIndex < 0 ) {
			return false;
		}
		return setValue(antID,keyIndex,time,value);
	}

	bool deleteValue(quint32 antID,
	                 const QString & key,
	                 const fort::Time & time) {
		int keyIndex = d_keyModel->find(key);
		if ( keyIndex < 0 ) {
			return false;
		}
		return deleteValue(antID,keyIndex,time);
	}

	void setUpExperiment(const fmp::Experiment::Ptr & experiment) {
		if ( d_experiment != nullptr ) {
			tearDownExperiment();
		}
		if ( experiment == nullptr ) {
			return;
		}
		beginResetModel();
		d_experiment = experiment;
		rebuildPointers();
		endResetModel();
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

	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex() ) const override {
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
				int value = row;
				if ( hasDefaultValue(*ant,keyNameAt(pPointer->Key)) == true ) {
					value += 1;
				}
				return createIndex(row,column,
				                   d_pointers.at(std::make_tuple(ant,pPointer->Key,value)).get());
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
				auto ant = d_ants[p->Ant].get();

				auto count = std::distance(d_pointers.upper_bound(std::make_tuple(ant,p->Key,-1)),
				                           d_pointers.lower_bound(std::make_tuple(ant,p->Key+1,-1)));
				return count;
			} catch (const std::exception &) {
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
		case Qt::EditRole:
			return displayData(index);
		case AntKeyValueBridge::KeyTypeRole:
			return keyTypeData(index);
		case AntKeyValueBridge::KeyNameRole:
			return keyNameData(index);
		case AntKeyValueBridge::AntIDRole:
			return antIDData(index);
		case AntKeyValueBridge::TimeRole:
			return timeData(index);
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

	Qt::ItemFlags flags(const QModelIndex &index) const override {
		if ( index.isValid() == false ) {
			return 0;
		}
		Qt::ItemFlags base = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
		auto p = static_cast<Pointer*>(index.internalPointer());
		if ( p->Value >= 0 && index.column() >= 2) {
			return base | Qt::ItemIsEditable;
		}
		if ( p->Key >= 0 && index.column() >= 3 ) {
			return base | Qt::ItemIsEditable;
		}
		return base;
	}

	bool setData(const QModelIndex & index,const QVariant & value, int role ) {
		if ( index.isValid() == false ) {
			return false;
		}
		auto p = static_cast<Pointer*>(index.internalPointer());
		if ( p->Key < 0 ) {
			return false;
		}
		if ( p->Value >= 0 && index.column() == 2 ) {
			return editValueTime(index,value.toString());
		}
		if ( p->Value >= 0 && index.column() == 3 ) {
			return editValue(index,value.toString());
		}

		if (p->Key >= 0 && index.column() == 3 ) {
			return editDefaultValue(index,value.toString());
		}
		return false;
	}

private slots:

	bool setValue(quint32 antID,
	              int key,
	              const fort::Time & time,
	              const fm::Value & value) {
		if ( key < 0 || key >= d_keyModel->rowCount() ) {
			return false;
		}
		auto fi = find(antID);
		if ( fi == d_ants.cend() ) {
			return false;
		}
		auto & ant = **fi;
		auto keyName = keyNameAt(key);
		bool added = hasValue(ant,keyName,time) == false;
		try {
			qInfo() << "[AntKeyValueBridge]: Ant{ID: " << fm::FormatAntID(antID).c_str()
			        << "}.SetValue(key =" << keyName.c_str()
			        << ",value = " << ToQString(value)
			        << ", time = " << ToQString(time)
			        << ")";
			ant.SetValue(keyName,value,time);
		} catch ( const std::exception & e ) {
			qCritical() << "[AntKeyValueBridge]: Ant{ID: " << fm::FormatAntID(antID).c_str()
			            << "}.SetValue(key =" << keyName.c_str()
			            << ",value = " << ToQString(value)
			            << ", time = " << ToQString(time)
			            << ") error:" << e.what();
			return false;
		}

		if ( time.IsInfinite() ) {
			auto keyIndex = index(key,0,index(fi-d_ants.cbegin(),0));
			auto defaultValueIndex = keyIndex.siblingAtColumn(3);
			emit dataChanged(defaultValueIndex,defaultValueIndex);
		} else if( added == false ) {
			const auto & data = ant.DataMap().at(keyName);
			int row = valueRow(ant,keyName,time);
			auto i = index(row,3,index(key,0,index(fi-d_ants.cbegin(),0)));
			emit dataChanged(i,i);
		}

		if ( added == true ) {
			resetValueModel(*fi,fi - d_ants.cbegin(),keyName,key);
		}
		return true;
	}

	bool deleteValue(quint32 antID,
	                 int key,
	                 const fort::Time & time) {
		if ( key < 0 || key >= d_keyModel->rowCount() ) {
			return false;
		}
		auto fi = find(antID);
		if ( fi == d_ants.cend() ) {
			return false;
		}
		auto & ant = **fi;
		auto keyName = keyNameAt(key);
		try {
			qInfo() << "[AntKeyValueBridge]:  Ant{ID:" << fm::FormatAntID(antID).c_str()
			        << "}.DeleteValue(key = " << keyName.c_str()
			        << ", time = " << ToQString(time)
			        << ")";
			ant.DeleteValue(keyName,time);
		} catch ( const std::exception & e ) {
			qCritical() << "[AntKeyValueBridge]:  Ant{ID:" << fm::FormatAntID(antID).c_str()
			            << "}.DeleteValue(key = " << keyName.c_str()
			            << ", time = " << ToQString(time)
			            << ") error:" << e.what();
			return false;
		}

		if ( time.IsInfinite() ) {
			auto defaultValueIndex = index(key,3,index(fi-d_ants.begin(),0));
			emit dataChanged(defaultValueIndex,defaultValueIndex);
		}
		resetValueModel(*fi,fi - d_ants.cbegin(),keyName,key);
		return true;
	}


	void onKeyModel_layoutAboutToBeChanged() {
		beginResetModel();
	}

	void onKeyModel_layoutChanged() {
		rebuildPointers();
		endResetModel();
	}

	void onKeyModel_dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight) {
		auto inRange = [&](int column) { return topLeft.column() <= column && column <= bottomRight.column(); };
		if ( inRange(0) ) {
			for ( size_t i = 0; i < d_ants.size(); ++i ) {
				auto keyIndex = index(topLeft.row(),1,index(i,0));
				emit dataChanged(keyIndex,keyIndex);
			}
		}
		if ( inRange(2) || inRange(1) ) {
			for ( size_t i = 0; i < d_ants.size();++i ) {
				auto keyIndex = index(topLeft.row(),0,index(i,0));
				auto defaultValueIndex = keyIndex.siblingAtColumn(3);
				emit dataChanged(defaultValueIndex,defaultValueIndex);
				auto childCount = rowCount(keyIndex);
				if ( childCount > 0 ) {
					emit dataChanged(index(0,3,keyIndex),index(childCount-1,3,keyIndex));
				}
			}
		}
	}



private:
	struct Pointer {
		int Ant;
		int Key;
		int Value;
	};

	bool editValueTime(const QModelIndex & index, const QString & value) {
		if ( index.isValid() == false ) {
			return false;
		}
		auto p = static_cast<Pointer*>(index.internalPointer());
		if ( p->Value < 0 ) {
			return false;
		}

		try {
			const auto & ant = *d_ants.at(p->Ant);
			int key = p->Key;
			auto newTime = fort::Time::Parse(ToStdString(value));
			auto [oldTime,currentValue] = ant.DataMap().at(keyNameAt(p->Key)).at(p->Value);

			qInfo() << "Changing time of {Ant=" << fm::FormatAntID(ant.AntID()).c_str()
			        << ", key=" << keyNameAt(p->Key).c_str()
			        << ", time=" << ToQString(oldTime)
			        << "} to" << ToQString(newTime);

			if ( deleteValue(ant.AntID(),key,oldTime) == false ) {
				return false;
			}
			return setValue(ant.AntID(),key,newTime,currentValue);
		} catch(const std::exception & e) {
			qCritical() << "[AntKeyValueBridge]: Could set new time " << value << ":" << e.what();
		}
		return false;
	}

	bool editValue(const QModelIndex & index, const QString & value) {
		if ( index.isValid() == false ) {
			return false;
		}
		auto p = static_cast<Pointer*>(index.internalPointer());
		if ( p->Value < 0 || p->Key >= d_keyModel->rowCount()) {
			return false;
		}
		try {
			auto keyType = fm::ValueType(d_keyModel->index(p->Key,0).data(AntKeyValueBridge::KeyTypeRole).toInt());
			auto v = fm::ValueUtils::Parse(keyType,ToStdString(value));
			auto & ant = *d_ants.at(p->Ant);
			auto time = ant.DataMap().at(keyNameAt(p->Key)).at(p->Value).first;
			return setValue(ant.AntID(),p->Key,time,v);
		} catch(const std::exception & ) {
		}
		return false;
	}

	bool editDefaultValue(const QModelIndex & index, const QString & value) {
		if ( index.isValid() == false ) {
			return false;
		}
		auto p = static_cast<Pointer*>(index.internalPointer());
		if ( p->Key >= d_keyModel->rowCount()) {
			return false;
		}
		try {
			auto & ant = *d_ants.at(p->Ant);
			if ( value.isEmpty() ) {
				return deleteValue(ant.AntID(),p->Key,fort::Time::SinceEver());
			}
			auto keyType = fm::ValueType(d_keyModel->index(p->Key,0).data(AntKeyValueBridge::KeyTypeRole).toInt());
			auto v = fm::ValueUtils::Parse(keyType,ToStdString(value));
			return setValue(ant.AntID(),p->Key,fort::Time::SinceEver(),v);
		} catch(const std::exception & e) {
		}
		return false;
	}

	void resetValueModel(const fmp::Ant::Ptr & ant,
	                     int antIndex,
	                     const std::string & keyName,
	                     int keyIndex) {
		auto qKeyIndex = index(keyIndex,0,index(antIndex,0));
		int count = rowCount(qKeyIndex);
		if ( count > 0 ) {
			beginRemoveRows(qKeyIndex,0,count-1);
			d_pointers.erase(d_pointers.upper_bound(std::make_tuple(ant.get(),keyIndex,-1)),
			                 d_pointers.lower_bound(std::make_tuple(ant.get(),keyIndex+1,-1)));
			endRemoveRows();
		}
		count = 0 ;
		try {
			const auto & values = ant->DataMap().at(keyName);
			count = values.size();
			if ( count > 0 && values.front().first.IsInfinite() ) {
				count -= 1;
			}
		} catch ( const std::exception & e) {
		}
		if ( count == 0 ) {
			return;
		}
		beginInsertRows(qKeyIndex,0,count-1);
		rebuildValuePointers(ant,antIndex,keyName,keyIndex);
		endInsertRows();
	}

	void rebuildValuePointers(const fmp::Ant::Ptr & ant,
	                          int antIndex,
	                          const std::string & keyName,
	                          int keyIndex) {
		try {
			int v = -1;
			for ( const auto & [time,value] : ant->DataMap().at(keyName) ) {
				++v;
				if ( time.IsInfinite() ) {
					continue;
				}
				d_pointers.insert({std::make_tuple(ant.get(),keyIndex,v),
				                   std::make_unique<Pointer>(Pointer{.Ant = antIndex,
				                                                     .Key = keyIndex,
				                                                     .Value = v})});
			}
		} catch ( const std::exception &) {
		}
	}

	void rebuildPointers() {
		d_ants.clear();
		d_pointers.clear();

		if ( d_experiment == nullptr ) {
			return;
		}

		const auto & ants = d_experiment->Identifier()->Ants();
		auto antSize = ants.size();
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
				rebuildValuePointers(ant,antIndex,keyName,k);
			}
		}
	}


	static int valueRow(const fmp::Ant & ant,
	                    const std::string & keyName,
	                    const fort::Time & time) {
		try {
			const auto & values = ant.DataMap().at(keyName);
			auto fi = std::lower_bound(values.begin(),
			                           values.end(),
			                           time,
			                           [](const fmp::TimedValue & v,
			                              const fort::Time & t) {
				                           return v.first < t;
			                           });
			if ( fi != values.cend()
			     && fi->first == time) {
				return fi - values.cbegin() + values.front().first.IsSinceEver() ? -1 : 0;
			}
		} catch ( const std::exception & ) {
		}
		return -1;
	}

	static bool hasValue(const fmp::Ant & ant,
	                     const std::string & keyName,
	                     const fort::Time & time) {
		try {
			const auto & values = ant.DataMap().at(keyName);
			auto fi = std::lower_bound(values.begin(),
			                           values.end(),
			                           time,
			                           [](const fmp::TimedValue & v,
			                              const fort::Time & t) {
				                           return v.first < t;
			                           });
			return fi != values.cend() && fi->first == time;
		} catch ( const std::exception &) {
		}
		return false;
	}

	static bool hasDefaultValue(const fmp::Ant & ant,
	                            const std::string & keyName ) {
		return hasValue(ant,keyName,fort::Time::SinceEver());
	}

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

	QVariant keyNameData(const QModelIndex & index) const {
		if ( index.isValid() == false ) {
			return QVariant();
		}
		auto p = static_cast<Pointer*>(index.internalPointer());
		if ( p->Key < 0 || p->Key >= d_keyModel->rowCount() ) {
			return QVariant();
		}
		return d_keyModel->index(p->Key,0).data(Qt::DisplayRole);
	}

	QVariant antIDData(const QModelIndex & index) const {
		if ( index.isValid() == false ) {
			return QVariant();
		}
		auto p = static_cast<Pointer*>(index.internalPointer());
		try {
			return d_ants.at(p->Ant)->AntID();
		} catch ( const std::exception &) {
		}
		return QVariant();
	}

	QVariant timeData(const QModelIndex & index) const {
		if ( index.isValid() == false ) {
			return QVariant();
		}
		auto p = static_cast<Pointer*>(index.internalPointer());
		if ( p->Value < 0 ) {
			return QVariant();
		}
		try {
			const auto & ant = d_ants.at(p->Ant);
			const auto & values = ant->DataMap().at(keyNameAt(p->Key));
			return ToQString(values.at(p->Value).first.Format());
		} catch ( const std::exception & ) {
		}
		return QVariant();
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

	std::vector<fmp::Ant::Ptr>::const_iterator lower_bound(quint32 antID) const {
		return std::lower_bound(d_ants.begin(),
		                        d_ants.end(),
		                        antID,
		                        [](const fmp::Ant::Ptr & a , quint32 antID) {
			                        return a->AntID() < antID;
		                        });
	}

	std::vector<fmp::Ant::Ptr>::const_iterator find(quint32 antID) const {
		auto fi = lower_bound(antID);
		if ( fi != d_ants.cend() && (*fi)->AntID() == antID ) {
			return fi;
		}
		return d_ants.cend();
	}

	fmp::Experiment::Ptr                     d_experiment;
	KeyModel                               * d_keyModel;
	std::vector<fmp::Ant::Ptr>               d_ants;
	std::map<std::tuple<fmp::Ant*,int,int>,
	         std::unique_ptr<Pointer>>       d_pointers;
};


#include "AntKeyValueBridge.moc"


AntKeyValueBridge::AntKeyValueBridge(QObject * parent)
	: GlobalBridge(parent)
	, d_keyModel(new KeyModel(this))
	, d_dataModel(new DataModel(d_keyModel,this))
	, d_typeModel(new QStandardItemModel(parent)){
	qRegisterMetaType<fort::Time>();
	qRegisterMetaType<fm::Value>();

#define add_item_type(t,T,tT) do {	  \
		auto  item = new QStandardItem(#tT); \
		item->setData(quint32(fm::ValueType::T),KeyTypeRole); \
		d_typeModel->appendRow(item); \
	}while(0)
	add_item_type(bool,BOOL,Bool);
	add_item_type(int,INT,Int);
	add_item_type(double,DOUBLE,Double);
#undef add_item_type

	auto stringItem = new QStandardItem("String");
	stringItem->setData(quint32(fm::ValueType::STRING),KeyTypeRole);
	d_typeModel->appendRow(stringItem);

	auto timeItem = new QStandardItem("Time");
	timeItem->setData(quint32(fm::ValueType::TIME),KeyTypeRole);
	d_typeModel->appendRow(timeItem);

	connect(d_keyModel,&QAbstractItemModel::dataChanged,
	        this,&AntKeyValueBridge::markModified);

	connect(d_dataModel,&QAbstractItemModel::dataChanged,
	        this,&AntKeyValueBridge::markModified);

	d_validators =
		{
		 new QRegularExpressionValidator(QRegularExpression("true|false"),this),
		 new QIntValidator(this),
		 new QDoubleValidator(this),
		 nullptr,
		 new QRegularExpressionValidator(QRegularExpression("\\d{4}-\\d{2}-\\d{2}T[0-2]\\d:[0-5]\\d:[0-5]\\d(\\.\\d+)?(Z|[+-]\\d{2}:\\d{2})"),this)
		};

	d_stringCompletion = new QStandardItemModel(this);
	d_completers =
		{
		 new QCompleter({"false","true"},this),
		 nullptr,
		 nullptr,
		 new QCompleter(d_stringCompletion,this),
		 nullptr,
		};

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

void AntKeyValueBridge::setKey(const QString & name, const fm::Value & defaultValue) {
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
                                 const fm::Value & value) {
	if ( !d_experiment ) {
		return;
	}

	if ( d_dataModel->setValue(antID,key,time,value) == true ) {
		setModified(true);
		pushCompletion(value);
	}
}

void AntKeyValueBridge::deleteValue(quint32 antID,
                                    const QString & key,
                                    const fort::Time & time) {
	if ( !d_experiment ) {
		return;
	}

	if ( d_dataModel->deleteValue(antID,key,time) == true ) {
		setModified(true);
	}
}

void AntKeyValueBridge::appendDefaultValue(quint32 antID,
                                           const QString & key) {
	if ( !d_experiment ) {
		return;
	}

	if ( d_dataModel->appendDefaultValue(antID,key) == true ) {
		setModified(true);
	}
}



void AntKeyValueBridge::onAntCreated(quint32 antID) {
	d_dataModel->resetInternalModel();
}

void AntKeyValueBridge::onAntDeleted(quint32 antID) {
	d_dataModel->resetInternalModel();
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
	clearCompletions();
	d_dataModel->tearDownExperiment();
	d_keyModel->tearDownExperiment();
}

void AntKeyValueBridge::setUpExperiment() {
	if ( isActive() == false ) {
		return;
	}
	d_keyModel->setUpExperiment(d_experiment);
	d_dataModel->setUpExperiment(d_experiment);
	initializeCompletions();
}

void AntKeyValueBridge::markModified() {
	setModified(true);
}

const fm::Value & AntKeyValueBridge::defaultValue(const QString & key) const {
	return d_keyModel->defaultValue(key);
}


QValidator * AntKeyValueBridge::validatorForType(fm::ValueType type) {
	try {
		return d_validators.at(int(type));
	} catch ( const std::exception & ) {
	}
	return nullptr;
}

QCompleter * AntKeyValueBridge::completerForType(fm::ValueType type) {
	try {
		return d_completers.at(int(type));
	} catch ( const std::exception & ) {
	}
	return nullptr;
}

void AntKeyValueBridge::pushCompletion(const fm::Value & value) {
	if ( fm::ValueUtils::Type(value) != fm::ValueType::STRING ) {
		return;
	}
	auto s = std::get<std::string>(value);
	if ( d_stringValues.count(s) != 0 ) {
		return;
	}
	d_stringValues.insert(s);
	d_stringCompletion->appendRow(new QStandardItem(ToQString(s)));
}
void AntKeyValueBridge::initializeCompletions() {
	if ( d_experiment == nullptr ) {
		return;
	}
	for ( const auto & [antID,ant] : d_experiment->Identifier()->Ants() ) {
		for ( const auto & [name,values] : ant->DataMap() ) {
			for ( const auto & [time, value] : values ) {
				pushCompletion(value);
			}
		}
	}
}

void AntKeyValueBridge::clearCompletions() {
	d_stringValues.clear();
	d_stringCompletion->removeRows(0,d_stringCompletion->rowCount());
}
