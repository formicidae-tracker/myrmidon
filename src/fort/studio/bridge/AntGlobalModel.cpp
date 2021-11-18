#include "AntGlobalModel.hpp"

#include <fort/myrmidon/priv/Identifier.hpp>

#include "IdentifierBridge.hpp"

void AntGlobalModel::setItemUserData(QStandardItem * item,
                                     const fmp::Ant::Ptr & ant) {
	item->setData(QVariant::fromValue(ant));
}

fmp::Ant::Ptr AntGlobalModel::findAnt(const fmp::Experiment::Ptr & experiment,
                                      fm::AntID antID) {
	if ( experiment == nullptr ) {
		return nullptr;
	}
	try {
		return experiment->Identifier()->Ants().at(antID);
	} catch ( const std::exception & ) {
		return nullptr;
	}
}

QString AntGlobalModel::formatAntName(const fmp::Ant::Ptr & ant) {
	QString res = ant->FormattedID().c_str();
	if ( ant->Identifications().empty() ) {
		return res + " <no-tags>";
	}
	std::set<fmp::TagID> tags;
	for ( const auto & i : ant->Identifications() ) {
		tags.insert(i->TagValue());
	}
	QString prefix = " ↤ {";
	for ( const auto & t : tags ) {
		res += prefix + fmp::FormatTagID(t).c_str();
		prefix = ",";
	}
	return res + "}";
}


AntGlobalModel::AntGlobalModel(QObject * parent)
	: QStandardItemModel(parent) {
	qRegisterMetaType<fmp::Ant::Ptr>();

	connect(this,&QAbstractItemModel::modelReset,
	        this,&AntGlobalModel::clearIndex);

	connect(this,&QAbstractItemModel::rowsInserted,
	        this,&AntGlobalModel::clearIndex);

	connect(this,&QAbstractItemModel::rowsMoved,
	        this,&AntGlobalModel::clearIndex);

	connect(this,&QAbstractItemModel::rowsRemoved,
	        this,&AntGlobalModel::clearIndex);

}

AntGlobalModel::~AntGlobalModel() {
}

void AntGlobalModel::initialize(IdentifierBridge * identifier) {
	connect(identifier,&IdentifierBridge::identificationCreated,
	        this,&AntGlobalModel::onIdentificationModified);
	connect(identifier,&IdentifierBridge::identificationDeleted,
	        this,&AntGlobalModel::onIdentificationModified);

}

QStandardItem * AntGlobalModel::itemFromAntID(fm::AntID antID) const {
	if ( rowCount() == 0 ) {
		return nullptr;
	}
	if ( d_index.empty() ) {
		for ( size_t i = 0; i < rowCount(); ++i ) {
			auto it = item(i,0);
			const_cast<AntGlobalModel*>(this)->d_index[it->data().value<fmp::Ant::Ptr>()->AntID()] = it;
		}
	}
	auto fi = d_index.find(antID);

	if ( fi == d_index.cend() ) {
		return nullptr;
	}
	return fi->second;
}

fm::AntID AntGlobalModel::antIDFromIndex(const QModelIndex & index) const {
	const auto & ant = antFromIndex(index);
	if ( ant == nullptr ) {
		return 0;
	}
	return ant->AntID();
}

fmp::Ant::Ptr AntGlobalModel::antFromIndex(const QModelIndex & index) const {
	auto item = itemFromIndex(index);
	if ( item == nullptr ) {
		return nullptr;
	}
	return item->data().value<fmp::Ant::Ptr>();
}

void AntGlobalModel::onIdentificationModified(fmp::Identification::ConstPtr identification) {
	auto item = itemFromAntID(identification->Target()->AntID());
	if ( item == nullptr ) {
		return;
	}
	item->setText(formatAntName(item->data().value<fmp::Ant::Ptr>()));
}

void AntGlobalModel::clearIndex() {
	d_index.clear();
}
