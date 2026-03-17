#include "ZoneBridge.hpp"

#include <QStandardItemModel>
#include <QtConcurrent>
#include <fort/studio/Format.hpp>

#include <fort/studio/MyrmidonTypes/Space.hpp>
#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>
#include <fort/studio/MyrmidonTypes/Zone.hpp>

#include "ExperimentBridge.hpp"
#include "UniverseBridge.hpp"
#include "fort/myrmidon/utils/Exception.hpp"
#include "fort/myrmidon/utils/Slogpp.hpp"

const int ZoneBridge::TypeRole = Qt::UserRole + 1;
const int ZoneBridge::DataRole = Qt::UserRole + 2;

Q_DECLARE_METATYPE(ZoneBridge::FullFrame)

ZoneBridge::~ZoneBridge() {}

ZoneBridge::ZoneBridge(QObject *parent)
    : GlobalBridge(parent)
    , d_spaceModel(new QStandardItemModel(this))
    , d_fullFrameModel(new QStandardItemModel(this))
    , d_logger{slog::With(slog::String("module", "ZoneBridge"))} {

	qRegisterMetaType<fmp::Space::Ptr>();
	qRegisterMetaType<fmp::Zone::Ptr>();
	qRegisterMetaType<fmp::ZoneDefinition::Ptr>();
	qRegisterMetaType<ZoneBridge::FullFrame>();

	connect(d_spaceModel,&QStandardItemModel::itemChanged,
	        this,&ZoneBridge::onItemChanged);

	connect(this,&Bridge::modified,
	        this,[this](bool isModified) {
		             if ( isModified == true ) {
			             return;
		             }
		             for ( const auto & c : d_childBridges ) {
			             c->setModified(false);
		             }
	             });
}

void ZoneBridge::initialize(ExperimentBridge * experiment) {
	connect(experiment->universe(),&UniverseBridge::spaceDeleted,
	        this,&ZoneBridge::rebuildSpaces);

	connect(experiment->universe(),&UniverseBridge::spaceAdded,
	        this,&ZoneBridge::rebuildSpaces);

	connect(experiment->universe(),&UniverseBridge::spaceChanged,
	        this,&ZoneBridge::rebuildSpaces);

	connect(experiment->universe(),
	        &UniverseBridge::trackingDataDirectoryAdded,
	        this,
	        [this](const fmp::TrackingDataDirectory::Ptr & tdd) {
		        onTrackingDataDirectoryChange(tdd->URI().c_str());
	        });
	connect(experiment->universe(),
	        &UniverseBridge::trackingDataDirectoryDeleted,
	        this,&ZoneBridge::onTrackingDataDirectoryChange);

}

void ZoneBridge::tearDownExperiment() {
	clearSpaces();
	clearFullFrames();
}

void ZoneBridge::setUpExperiment() {
	d_selectedSpace.reset();
	d_selectedTime = fort::Time::Forever();

	rebuildSpaces();
}


QAbstractItemModel * ZoneBridge::spaceModel() const {
	return d_spaceModel;
}


QAbstractItemModel * ZoneBridge::fullFrameModel() const {
	return d_fullFrameModel;
}


void ZoneBridge::clearSpaces() {
	d_spaceModel->clear();
	d_spaceModel->setHorizontalHeaderLabels({tr("ID"),tr("Name"),tr("Size")});
	d_selectedSpace.reset();
	d_selectedTime = fort::Time::Forever();
}

void ZoneBridge::rebuildSpaces() {
	clearSpaces();
	rebuildFullFrameModel();
	rebuildChildBridges();
	if ( !d_experiment ) {
		return;
	}
	for ( const auto & [spaceID,space] : d_experiment->Spaces() ) {
		d_spaceModel->appendRow(buildSpace(space));
	}
}

void ZoneBridge::onTrackingDataDirectoryChange(const QString & uri) {
	if ( !d_experiment == true || !d_selectedSpace == true) {
		return;
	}
	auto located = d_experiment->LocateTrackingDataDirectory(ToStdString(uri));
	if ( !located.first == true || located.first != d_selectedSpace ) {
		return;
	}
	rebuildFullFrameModel();
}


QStandardItem * ZoneBridge::getSibling(QStandardItem * item, int column) {
	return d_spaceModel->itemFromIndex(d_spaceModel->sibling(item->row(),column,item->index()));
}

void ZoneBridge::addItemAtIndex(const QModelIndex & index) {
	auto item = d_spaceModel->itemFromIndex(index);
	if (item == nullptr ) {
		return;
	}
	//selects the first column
	item = getSibling(item,0);
	switch ( item->data(TypeRole).toInt() ) {
	case SpaceType:
		addZone(item);
		break;
	case ZoneType:
		addDefinition(item);
		break;
	case DefinitionType:
		addDefinition(item->parent());
		break;
	default:
		break;
	}
}

void ZoneBridge::removeItemAtIndex(const QModelIndex & index) {
	auto item = d_spaceModel->itemFromIndex(index);
	if (item == nullptr ) {
		return;
	}
	//selects the first column
	item = getSibling(item,0);
	switch ( item->data(TypeRole).toInt() ) {
	case SpaceType:
		return;
	case ZoneType:
		removeZone(item);
		break;
	case DefinitionType:
		removeDefinition(item);
		break;
	default:
		break;
	}
}

void ZoneBridge::addDefinition(QStandardItem *zoneRootItem) {
	fmp::ZoneDefinition::Ptr definition;
	auto       z = zoneRootItem->data(DataRole).value<fmp::Zone::Ptr>();
	fort::Time start, end;
	if (!z == true || z->NextFreeTimeRegion(start, end) == false) {
		return;
	}

	fm::Shape::List shapes;
	if (start.IsInfinite() == false) {
		shapes = z->AtTime(start.Add(-1));
	} else if (end.IsInfinite() == false) {
		shapes = z->AtTime(end.Add(1));
	}

	auto logger = d_logger.With(
	    slog::String("zone_name", z->Name()),
	    slog::Int("zone_ID", z->ID()),
	    slog::Int("shapes_count", shapes.size()),
	    slog::FortTime("start", start),
	    slog::FortTime("end", end)
	);

	try {
		logger.Debug("calling fmp::Zone::AddDefinition()");
		z->AddDefinition(shapes, start, end);
	} catch (const std::exception &e) {
		logger.Error(
		    "could not create definition",
		    fort::myrmidon::utils::Err(e)
		);
		return;
	}
	logger.Info("created new zone definition");

	setModified(true);
	zoneRootItem->removeRows(0, zoneRootItem->rowCount());
	for (const auto &definition : z->Definitions()) {
		zoneRootItem->appendRow(buildDefinition(definition));
	}
	getSibling(zoneRootItem, 2)
	    ->setText(QString::number(zoneRootItem->rowCount()));
	rebuildChildBridges();
	emit definitionUpdated();
}

void ZoneBridge::addZone(QStandardItem *spaceRootItem) {
	fmp::Zone::Ptr z;
	auto space = spaceRootItem->data(DataRole).value<fmp::Space::Ptr>();
	if (!space == true) {
		return;
	}
	auto logger = d_logger.With(
	    slog::String("space_name", space->Name()),
	    slog::Int("space_ID", space->ID())
	);

	try {
		logger.Debug("fort::myrmidon::priv::Space::CreateZone('new-zone')");
		z = space->CreateZone(ToStdString(tr("new-zone")));
		logger.Debug(
		    "calling fort::myrmidon::priv::Zone::AddDefinition({},-∞,+∞)"
		);
		z->AddDefinition({}, fort::Time::SinceEver(), fort::Time::Forever());
	} catch (const std::exception &e) {
		logger.Error("could not create Zone", fort::myrmidon::utils::Err(e));
		return;
	}

	setModified(true);
	int insertionRow = 0;
	for (; insertionRow < spaceRootItem->rowCount(); ++insertionRow) {
		if (fort::myrmidon::ZoneID(spaceRootItem->child(insertionRow)
		                               ->data(Qt::DisplayRole)
		                               .toInt()) > z->ID()) {
			break;
		};
	}
	spaceRootItem->insertRow(insertionRow, buildZone(z));
	getSibling(spaceRootItem, 2)
	    ->setText(QString::number(spaceRootItem->rowCount()));

	logger.Info(
	    "created new zone",
	    slog::String("zone_name", z->Name()),
	    slog::Int("zone_ID", z->ID())
	);

	if (space == d_selectedSpace) {
		rebuildChildBridges();
	}
}

void ZoneBridge::removeZone(QStandardItem *zoneItem) {
	auto spaceItem = zoneItem->parent();
	auto zone      = zoneItem->data(DataRole).value<fmp::Zone::Ptr>();
	auto space     = spaceItem->data(DataRole).value<fmp::Space::Ptr>();
	if (!space || !zone) {
		return;
	}
	auto logger = d_logger.With(
	    slog::String("space_name", space->Name()),
	    slog::Int("space_ID", space->ID()),
	    slog::String("zone_name", zone->Name()),
	    slog::Int("zone_ID", zone->ID())
	);

	try {
		logger.Debug("calling fmp::Space::DeleteZone()");
		space->DeleteZone(zone->ID());
	} catch (const std::exception &e) {
		logger.Error("could not delete zone", fort::myrmidon::utils::Err(e));
		return;
	}

	logger.Info("deleted zone");

	setModified(true);
	spaceItem->removeRows(zoneItem->row(), 1);
	getSibling(spaceItem, 2)->setText(QString::number(spaceItem->rowCount()));
	if (space == d_selectedSpace) {
		rebuildChildBridges();
	}
}

void ZoneBridge::removeDefinition(QStandardItem *definitionItem) {
	auto zoneItem  = definitionItem->parent();
	auto spaceItem = zoneItem->parent();
	auto zone      = zoneItem->data(DataRole).value<fmp::Zone::Ptr>();
	auto space     = spaceItem->data(DataRole).value<fmp::Space::Ptr>();

	auto z = zoneItem->data(DataRole).value<fmp::Zone::Ptr>();
	if (!z == true) {
		return;
	}

	auto logger = d_logger.With(
	    slog::String("space_name", space->Name()),
	    slog::Int("space_ID", space->ID()),
	    slog::String("zone_name", zone->Name()),
	    slog::Int("zone_ID", zone->ID()),
	    slog::Int("index", definitionItem->row())
	);

	try {
		logger.Debug("calling fmp::Zone::EraseDefinition()");
		z->EraseDefinition(definitionItem->row());
	} catch (const std::exception &e) {
		logger.Error(
		    "could not remove definition",
		    fort::myrmidon::utils::Err(e)
		);
		return;
	}
	setModified(true);
	zoneItem->removeRows(definitionItem->row(), 1);
	getSibling(zoneItem, 2)->setText(QString::number(zoneItem->rowCount()));

	logger.Info("removed Zone definition");

	rebuildChildBridges();
	emit definitionUpdated();
}

QList<QStandardItem*> ZoneBridge::buildSpace(const fmp::Space::Ptr & space) const {
	auto typeData = QVariant(SpaceType);
	auto data = QVariant::fromValue(space);
	QList<QStandardItem*> res;
	res.push_back(new QStandardItem(space->URI().c_str()));
	res.push_back(new QStandardItem(ToQString(space->Name())));
	res.push_back(new QStandardItem(QString::number(space->Zones().size())));
	for ( const auto & i : res ) {
		i->setEditable(false);
		i->setData(typeData,TypeRole);
		i->setData(data,DataRole);
	}
	for ( const auto & [zoneID,zone] : space->Zones() ) {
		res[0]->appendRow(buildZone(zone));
	}
	return res;
}

QList<QStandardItem*> ZoneBridge::buildZone(const fmp::Zone::Ptr & zone) const {
	auto typeData = QVariant(ZoneType);
	auto data = QVariant::fromValue(zone);
	QList<QStandardItem*> res;
	res.push_back(new QStandardItem(zone->URI().c_str()));
	res.push_back(new QStandardItem(ToQString(zone->Name())));
	res.push_back(new QStandardItem(QString::number(zone->Definitions().size())));
	for ( const auto & i : res ) {
		i->setEditable(false);
		i->setData(typeData,TypeRole);
		i->setData(data,DataRole);
	}
	res[1]->setEditable(true);
	for ( const auto & definition : zone->Definitions() ) {
		res[0]->appendRow(buildDefinition(definition));
	}
	return res;
}

QList<QStandardItem*> ZoneBridge::buildDefinition(const fmp::ZoneDefinition::Ptr & definition) const {
	auto typeData = QVariant(DefinitionType);
	auto data = QVariant::fromValue(definition);
	QList<QStandardItem*> res;
	res.push_back(new QStandardItem(ToQString(definition->Start())));
	res.push_back(new QStandardItem(ToQString(definition->End())));
	res.push_back(new QStandardItem(QString::number(definition->Shapes().size())));
	for ( const auto & i : res ) {
		i->setEditable(true);
		i->setData(typeData,TypeRole);
		i->setData(data,DataRole);
	}
	res[2]->setEditable(false);
	return res;
}


bool ZoneBridge::canAddItemAt(const QModelIndex & index) {
	auto item = d_spaceModel->itemFromIndex(index);
	if ( item == nullptr ) {
		return false;
	}
	item = getSibling(item,0);
	fort::Time start,end;
	switch(item->data(TypeRole).toInt()) {
	case SpaceType:
		return true;
	case DefinitionType:
		item = item->parent();
	case ZoneType:
		return item->data(DataRole).value<fmp::Zone::Ptr>()->NextFreeTimeRegion(start,end);
	}
	return false;
}

bool ZoneBridge::canRemoveItemAt(const QModelIndex & index) {
	auto item = d_spaceModel->itemFromIndex(index);
	if ( item == nullptr ) {
		return false;
	}
	item = getSibling(item,0);
	switch(item->data(TypeRole).toInt()) {
	case SpaceType:
		return false;
	case DefinitionType:
	case ZoneType:
		return true;
	}
	return false;
}


void ZoneBridge::onItemChanged(QStandardItem * item) {
	if (item->data(TypeRole).toInt() == ZoneType && item->column() == 1 ) {
		changeZoneName(item);
		return;
	}

	if (item->data(TypeRole).toInt() == DefinitionType) {
		if ( item->column() == 0 ) {
			changeDefinitionTime(item,true);
			return;
		}
		if ( item->column() == 1 ) {
			changeDefinitionTime(item,false);
			return;
		}
	}
}

void ZoneBridge::changeZoneName(QStandardItem *zoneNameItem) {
	auto z = zoneNameItem->data(DataRole).value<fmp::Zone::Ptr>();

	std::string newName = ToStdString(zoneNameItem->text());
	if (!z == true || z->Name() == newName) {
		return;
	}
	auto oldName = ToQString(z->Name());

	auto logger = d_logger.With(
	    slog::String("old_name", z->Name()),
	    slog::String("new_name", newName),
	    slog::Int("zone_ID", z->ID())
	);

	try {
		logger.Debug("Calling fmp::Zone::SetName()");
		z->SetName(newName);
	} catch (const std::exception &e) {
		logger.Error("could not set zone name", fort::myrmidon::utils::Err(e));
		zoneNameItem->setText(oldName);
		return;
	}

	logger.Info("changed zone name");

	setModified(true);
	auto space = getSibling(zoneNameItem, 0)
	                 ->parent()
	                 ->data(DataRole)
	                 .value<fmp::Space::Ptr>();
	if (!space == false && space == d_selectedSpace) {
		rebuildChildBridges();
	}
}

void ZoneBridge::changeDefinitionTime(
    QStandardItem *definitionTimeItem, bool start
) {
	auto d =
	    definitionTimeItem->data(DataRole).value<fmp::ZoneDefinition::Ptr>();
	if (!d == true) {
		return;
	}

	auto oldTime    = start == true ? d->Start() : d->End();
	auto oldTimeStr = ToQString(oldTime);
	if (oldTimeStr == definitionTimeItem->text()) {
		return;
	}

	auto logger = d_logger.With(
	    slog::String("old_time", oldTimeStr.toStdString()),
	    slog::String("new_time", definitionTimeItem->text().toStdString())
	);

	fort::Time newTime =
	    start == true ? fort::Time::SinceEver() : fort::Time::Forever();
	if (definitionTimeItem->text().isEmpty() == false) {
		try {
			newTime =
			    fort::Time::Parse(ToStdString(definitionTimeItem->text()));
		} catch (const std::exception &e) {
			logger.Error("could not parse time", fort::myrmidon::utils::Err(e));
			definitionTimeItem->setText(oldTimeStr);
			return;
		}
	}
	auto newTimeStr = ToQString(newTime);

	try {
		if (start == true) {
			logger.Debug("Calling fmp::Zone::Definition::SetStart()");
			d->SetStart(newTime);
		} else {
			logger.Debug("Calling fmp::Zone::Definition::SetEnd()");
			d->SetEnd(newTime);
		}
	} catch (std::exception &e) {
		logger.Error(
		    "could not set definition time",
		    fort::myrmidon::utils::Err(e)
		);
		definitionTimeItem->setText(oldTimeStr);
		return;
	}

	setModified(true);
	definitionTimeItem->setText(newTimeStr);

	logger.Info(
	    start == true ? "changed definition start" : "changed definition end"
	);

	rebuildChildBridges();
	emit definitionUpdated();
}

void ZoneBridge::activateItem(QModelIndex index) {
	fmp::Space::Ptr newSpace;
	if ( index.isValid() != false ){
		index = d_spaceModel->sibling(index.row(),0,index);

		while(index.parent().isValid() == true ) {
			index = index.parent();
		}
		newSpace = d_spaceModel->itemFromIndex(index)->data(DataRole).value<fmp::Space::Ptr>();
	}

	if ( newSpace == d_selectedSpace ) {
		return;
	}
	d_selectedSpace = newSpace;
	rebuildFullFrameModel();
	rebuildChildBridges();
}

void ZoneBridge::clearFullFrames() {
	d_fullFrameModel->clear();
	d_fullFrameModel->setHorizontalHeaderLabels({tr("URI")});
}

void ZoneBridge::rebuildFullFrameModel() {
	clearFullFrames();

	if (!d_selectedSpace == true) {
		return;
	}
	std::vector<std::pair<std::string, FullFrame>> fullframes;

	for (const auto &tdd : d_selectedSpace->TrackingDataDirectories()) {
		if (tdd->FullFramesComputed() == false) {
			auto loaders = tdd->PrepareFullFramesLoaders();
			QtConcurrent::blockingMap(
			    loaders.begin(),
			    loaders.end(),
			    [](const fmp::TrackingDataDirectory::Loader &l) { l(); }
			);
		}
		for (const auto &[ref, path] : tdd->FullFrames()) {
			fullframes.push_back(
			    std::make_pair(ref.URI(), FullFrame{ref, path.c_str()})
			);
		}
	}

	for (const auto &[uri, ff] : fullframes) {
		auto roundedTime =
		    ff.Reference.Time().Round(fort::Duration::Millisecond);
		auto item = new QStandardItem(ToQString(roundedTime));
		item->setEditable(false);
		item->setData(QVariant::fromValue(ff));
		d_fullFrameModel->appendRow({item});
	}
}

std::pair<bool,ZoneBridge::FullFrame> ZoneBridge::fullFrameAtIndex(const QModelIndex & index) const {
	auto item = d_fullFrameModel->itemFromIndex(index);
	if ( item == nullptr ) {
		return std::make_pair(false,FullFrame());
	}

	return std::make_pair(true,item->data().value<FullFrame>());
}

void ZoneBridge::rebuildChildBridges() {
	d_childBridges.clear();

	if ( !d_selectedSpace == true ) {
		emit newZoneDefinitionBridge({});
		return;
	}
	auto items = d_spaceModel->findItems(d_selectedSpace->URI().c_str());
	if ( items.isEmpty() == true ) {
		emit newZoneDefinitionBridge({});
		return;
	}
	auto spaceRootItem = items[0];

	auto addChildBridge
		= [this](const fmp::Zone::Ptr & zone,
		         const fmp::ZoneDefinition::Ptr & definition,
		         QStandardItem * countItem) {
			  auto c = std::make_shared<ZoneDefinitionBridge>(zone,definition);
			  connect(c.get(),&Bridge::modified,
			          this,[this](bool nowModified) {
				               if ( nowModified == true ) {
					               setModified(true);
				               }
			               });
			  connect(c.get(),&ZoneDefinitionBridge::countUpdated,
			          this,
			          [this,countItem](int count) {
				          countItem->setText(QString::number(count));
			          });
			  d_childBridges.push_back(c);
		  };
	for( int i = 0; i < spaceRootItem->rowCount(); ++i) {
		auto zoneRootItem = spaceRootItem->child(i,0);
		auto zone = zoneRootItem->data(DataRole).value<fmp::Zone::Ptr>();
		if ( d_selectedTime.IsInfinite() == true ) {
			if ( zone->Definitions().empty() == false
			     && zone->Definitions().front()->Start().IsInfinite() == true ) {
				addChildBridge(zone,zone->Definitions().front(),zoneRootItem->child(0,2));
			}
			continue;
		}

		for ( int j = 0; j < zoneRootItem->rowCount(); ++j ) {
			auto d = zoneRootItem->child(j,0)->data(DataRole).value<fmp::ZoneDefinition::Ptr>();
			if ( d->IsValid(d_selectedTime) == true ) {
				addChildBridge(zone,d,zoneRootItem->child(j,2));
				break;
			}
		}
	}
	QList<ZoneDefinitionBridge*> res;
	res.reserve(d_childBridges.size());
	for ( const auto & c : d_childBridges) {
		res.push_back(c.get());
	}
	emit newZoneDefinitionBridge(res);
}

void ZoneBridge::selectTime(const fort::Time & time) {
	d_selectedTime = time;
	rebuildChildBridges();
}

ZoneDefinitionBridge::~ZoneDefinitionBridge() {}

ZoneDefinitionBridge::ZoneDefinitionBridge(
    const fmp::Zone::Ptr &zone, const fmp::ZoneDefinition::Ptr &ptr
)
    : Bridge(nullptr)
    , d_definition(ptr)
    , d_zone(zone) {}

bool ZoneDefinitionBridge::isActive() const {
	return true;
}

const fm::Shape::List & ZoneDefinitionBridge::shapes() const {
	return d_definition->Shapes();
}

void ZoneDefinitionBridge::setShapes(const fm::Shape::List & shapes) {
	d_definition->SetShapes(shapes);
	setModified(true);
	emit countUpdated(shapes.size());
}

const fmp::Zone & ZoneDefinitionBridge::zone() const {
	return *d_zone;
}
