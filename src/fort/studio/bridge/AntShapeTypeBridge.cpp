#include "AntShapeTypeBridge.hpp"
#include "fort/myrmidon/utils/Exception.hpp"

#include <QStandardItemModel>

#include <fort/studio/Format.hpp>

#include <fort/studio/MyrmidonTypes/AntShapeType.hpp>
#include <fort/studio/MyrmidonTypes/Conversion.hpp>
#include <fort/studio/MyrmidonTypes/Experiment.hpp>

AntShapeTypeBridge::AntShapeTypeBridge(QObject *parent)
    : GlobalBridge(parent)
    , d_model(new QStandardItemModel(this))
    , d_logger{slog::With(slog::String("module", "AntShapeTypeBridge"))} {
	qRegisterMetaType<fmp::AntShapeType::Ptr>();

	connect(d_model,
	        &QStandardItemModel::itemChanged,
	        this,
	        &AntShapeTypeBridge::onTypeItemChanged);
}

AntShapeTypeBridge::~AntShapeTypeBridge() {
}


void AntShapeTypeBridge::initialize(ExperimentBridge * experiment) {
}

void AntShapeTypeBridge::tearDownExperiment() {
	d_model->clear();
	d_model->setHorizontalHeaderLabels({tr("Name"),tr("TypeID")});
}


void AntShapeTypeBridge::setUpExperiment() {
	if ( isActive() == false ) {
		return;
	}

	for ( const auto & [typeID,shapeType] : d_experiment->AntShapeTypes() ) {
		d_model->appendRow(buildTypeItem(shapeType));
	}
}

QAbstractItemModel * AntShapeTypeBridge::shapeModel() const {
	return d_model;
}

void AntShapeTypeBridge::addType(const QString &name) {
	if (!d_experiment) {
		return;
	}
	fmp::AntShapeTypePtr shapeType;
	QString              actualName = name;
	if (name.isEmpty() == true) {
		actualName = QString("body part %1")
		                 .arg(d_experiment->AntShapeTypes().size() + 1);
	}

	auto logger = d_logger.With(slog::String("name", name.toStdString()));

	try {
		logger.Debug("calling fmp::Experiment::CreateAntShapeType()");
		shapeType = d_experiment->CreateAntShapeType(ToStdString(actualName));
	} catch (const std::exception &e) {
		logger.Error(
		    "could not create AntShapeType",
		    fort::myrmidon::utils::Err(e)
		);
		return;
	}
	logger.Info("created ant shape type");
	d_model->appendRow(buildTypeItem(shapeType));
	setModified(true);
	emit typeModified(shapeType->TypeID(), ToQString(shapeType->Name()));
}

void AntShapeTypeBridge::deleteType(quint32 typeID) {
	if (!d_experiment) {
		return;
	}
	auto items =
	    d_model->findItems(QString::number(typeID), Qt::MatchExactly, 1);
	auto logger = d_logger.With(slog::Int("typeID", typeID));
	if (items.isEmpty() == true) {
		logger.Warn("could not find type");
		return;
	}
	try {
		logger.Debug("calling fmp::Experiment::DeleteAntShapeType()");
		d_experiment->DeleteAntShapeType(typeID);
	} catch (const std::exception &e) {
		logger.Error(
		    "could not delete AntShapeType",
		    fort::myrmidon::utils::Err(e)
		);
		return;
	}
	d_model->removeRows(items[0]->row(), 1);
	setModified(true);
	emit typeDeleted(typeID);
}

void AntShapeTypeBridge::onTypeItemChanged(QStandardItem *item) {
	if (item->column() != 0) {
		return;
	}

	auto shapeType = item->data().value<fmp::AntShapeType::Ptr>();
	if (!shapeType) {
		return;
	}
	auto logger = d_logger.With(
	    slog::Int("shapeTypeID", shapeType->TypeID()),
	    slog::String("value", item->text().toStdString())
	);

	try {
		logger.Debug("calling fort::myrmidon::AntShapeType::SetName()");
		shapeType->SetName(ToStdString(item->text()));
	} catch (const std::exception &e) {
		logger.Error(
		    "could not set ShapeTypeID",
		    fort::myrmidon::utils::Err(e)
		);
		item->setText(ToQString(shapeType->Name()));
		return;
	}
	logger.Info("set ant shape type");
	setModified(true);
	emit typeModified(shapeType->TypeID(), item->text());
}

QList<QStandardItem*> AntShapeTypeBridge::buildTypeItem(const fmp::AntShapeType::Ptr & shapeType) {
	auto data = QVariant::fromValue(shapeType);
	auto id = new QStandardItem(QString::number(shapeType->TypeID()));
	id->setEditable(false);
	id->setData(data);
	auto icon = Conversion::iconFromFM(fm::DefaultPaletteColor(shapeType->TypeID()));
	auto name = new QStandardItem(ToQString(shapeType->Name()));
	name->setEditable(true);
	name->setData(data);
	name->setIcon(icon);

	return {name,id};
}


AntShapeTypeBridge::AntShapeTypesByID AntShapeTypeBridge::types() const {
	AntShapeTypesByID res;
	for ( const auto & [ stID,type ] : d_experiment->AntShapeTypes() ) {
		res.insert(std::make_pair(stID,type));
	}
	return res;
}
