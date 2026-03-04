#include "MeasurementBridge.hpp"

#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/studio/Format.hpp>

#include <QtConcurrent>

#include <fort/myrmidon/utils/Defer.hpp>

#include <fort/studio/MyrmidonTypes/Conversion.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>
#include <slog++/Attribute.hpp>
#include <slog++/slog++.hpp>

#include "ExperimentBridge.hpp"
#include "UniverseBridge.hpp"
#include "fort/myrmidon/utils/Exception.hpp"
#include "fort/studio/Slogpp.hpp"

MeasurementBridge::MeasurementBridge(QObject *parent)
    : GlobalBridge(parent)
    , d_typeModel(new QStandardItemModel(this))
    , d_logger{slog::With(slog::String("module", "MeasurementBridge"))} {

	connect(
	    d_typeModel,
	    &QStandardItemModel::itemChanged,
	    this,
	    &MeasurementBridge::onTypeItemChanged
	);
}

MeasurementBridge::~MeasurementBridge() {}

void MeasurementBridge::initialize(ExperimentBridge *experiment) {}

void MeasurementBridge::tearDownExperiment() {
	d_typeModel->clear();
	d_typeModel->setHorizontalHeaderLabels({tr("Name"), tr("TypeID")});
}

void MeasurementBridge::setUpExperiment() {
	if (isActive() == false) {
		return;
	}

	for (const auto &[mtID, type] : d_experiment->MeasurementTypes()) {
		d_typeModel->appendRow(buildType(type));
	}
}

QAbstractItemModel *MeasurementBridge::typeModel() const {
	return d_typeModel;
}

bool MeasurementBridge::setMeasurement(
    const fmp::TagCloseUp::ConstPtr &tcu,
    fmp::MeasurementType::ID         mtID,
    QPointF                          start,
    QPointF                          end
) {
	if (!d_experiment) {
		return false;
	}

	auto tddURI = tcu->Frame().ParentURI();

	Eigen::Vector2d startFromTag =
	    tcu->ImageToTag() * Eigen::Vector2d(start.x(), start.y());
	Eigen::Vector2d endFromTag =
	    tcu->ImageToTag() * Eigen::Vector2d(end.x(), end.y());
	auto m = std::make_shared<fmp::Measurement>(
	    tcu->URI(),
	    mtID,
	    startFromTag,
	    endFromTag,
	    tcu->TagSizePx()
	);
	auto logger = d_logger.With(
	    slog::String("measurement", m->URI()),
	    slog::QPointF("start", start),
	    slog::QPointF("end", end)
	);
	try {
		logger.Debug(
		    "calling fort::myrmidon::priv::Experiment::SetMeasurement()"
		);
		d_experiment->SetMeasurement(m);
	} catch (const std::exception &e) {
		logger.Error(
		    "could not set measurement",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
		return false;
	}
	logger.Info("set measurement");
	setModified(true);
	emit measurementCreated(m);
	return true;
}

void MeasurementBridge::deleteMeasurement(const fmp::Measurement::ConstPtr &m) {
	if (isActive() == false || m == nullptr) {
		return;
	}

	auto logger = d_logger.With(slog::String("measurement", m->URI()));
	try {
		logger.Debug(
		    "calling fort::myrmidon::priv::Experiment::DeleteMeasurement()"
		);
		d_experiment->DeleteMeasurement(m->URI());
	} catch (const std::exception &e) {
		logger.Error(
		    "could not delete measurement",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
		return;
	}
	logger.Info("deleted measurement");
	setModified(true);
	emit measurementDeleted(m);
}

void MeasurementBridge::setMeasurementType(quint32 mtID, const QString &name) {
	if (!d_experiment) {
		return;
	}
	auto logger = d_logger.With(
	    slog::Int("ID", mtID),
	    slog::String("name", name.toStdString())
	);
	try {
		auto fi = d_experiment->MeasurementTypes().find(mtID);
		if (fi == d_experiment->MeasurementTypes().end()) {
			logger.Debug(
			    "calling fort::myrmidon::priv::Experiment::CreateMeasurement()"
			);
			auto type = d_experiment->CreateMeasurementType(ToStdString(name));
			mtID      = type->MTID();
			d_typeModel->appendRow(buildType(type));
		} else {
			auto items = d_typeModel->findItems(
			    QString::number(mtID),
			    Qt::MatchExactly,
			    0
			);
			if (items.size() != 1) {
				throw std::logic_error("Internal type model error");
			}
			logger.Debug(
			    "calling fort::myrmidon::priv::MeasurementType::SetName()"
			);
			fi->second->SetName(name.toUtf8().data());
			d_typeModel->item(items[0]->row(), 1)->setText(name);
		}
	} catch (const std::exception &e) {
		logger.Error(
		    "could not set MeasurementType",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
		return;
	}
	logger.Info("set measurement type name");
	setModified(true);
	emit measurementTypeModified(mtID, name);
}

void MeasurementBridge::deleteMeasurementType(const QModelIndex &index) {
	auto item   = d_typeModel->itemFromIndex(index);
	auto logger = d_logger.With(slog::QModelIndex("index", index));
	if (item == NULL) {
		logger.Warn("could not delete measurement type");
		return;
	}
	auto mtype = item->data().value<fmp::MeasurementType::Ptr>();
	deleteMeasurementType(mtype->MTID());
}

void MeasurementBridge::deleteMeasurementType(quint32 mtID) {
	if (!d_experiment) {
		return;
	}
	auto logger = d_logger.With(slog::Int("ID", mtID));
	try {
		auto items =
		    d_typeModel->findItems(QString::number(mtID), Qt::MatchExactly, 1);
		if (items.size() != 1) {
			throw std::logic_error("Internal type model error");
		}
		logger.Debug(
		    "calling fort::myrmidon::Experiment::DeleteMeasurementType()"
		);
		d_experiment->DeleteMeasurementType(mtID);
		d_typeModel->removeRows(items[0]->row(), 1);
	} catch (const std::exception &e) {
		logger.Error(
		    "could not delete MeasurementType",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
		return;
	}
	logger.Info("deleted MeasurementType");
	setModified(true);
	emit measurementTypeDeleted(mtID);
}

QList<QStandardItem *>
MeasurementBridge::buildType(const fmp::MeasurementType::Ptr &type) const {
	auto mtID = new QStandardItem(QString::number(type->MTID()));
	mtID->setEditable(false);
	mtID->setData(QVariant::fromValue(type));
	auto name = new QStandardItem(type->Name().c_str());
	auto icon = Conversion::iconFromFM(fm::DefaultPaletteColor(type->MTID()));
	name->setIcon(icon);
	name->setData(QVariant::fromValue(type));
	name->setEditable(true);
	return {name, mtID};
}

void MeasurementBridge::onTypeItemChanged(QStandardItem *item) {

	if (item->column() != 0) {
		d_logger.Debug(
		    "ignoring measurement type item change",
		    slog::Int("column", item->column())
		);
		return;
	}

	auto        type    = item->data().value<fmp::MeasurementType::Ptr>();
	std::string newName = ToStdString(item->text());
	auto        logger  = d_logger.With(
        slog::String("old_name", type->Name()),
        slog::String("new_name", newName)
    );

	if (newName == type->Name()) {
		logger.Debug("ignoring MeasurementType item change: same name");
		return;
	}

	try {
		logger.Debug("calling fort::myrmidon::priv::MeasurementType::SetName()"
		);
		type->SetName(newName);
	} catch (const std::exception &e) {
		logger.Error(
		    "could not change measurement type",
		    slog::Err(fort::myrmidon::utils::What(e))
		);
		item->setText(type->Name().c_str());
		return;
	}
	logger.Info("changed MeasurementType name");
	setModified(true);
	emit measurementTypeModified(type->MTID(), type->Name().c_str());
}

fmp::Measurement::ConstPtr MeasurementBridge::measurementForCloseUp(
    const std::string &tcuURI, fmp::MeasurementTypeID type
) {
	if (!d_experiment) {
		return fmp::Measurement::ConstPtr();
	}
	auto tcufi = d_experiment->Measurements().find(tcuURI);
	if (tcufi == d_experiment->Measurements().end()) {
		return fmp::Measurement::ConstPtr();
	}
	auto fi = tcufi->second.find(type);
	if (fi == tcufi->second.end()) {
		return fmp::Measurement::ConstPtr();
	}
	return fi->second;
}
