#pragma once

#include "Bridge.hpp"

#include <QList>

#include <fort/studio/MyrmidonTypes/Experiment.hpp>
#include <fort/studio/MyrmidonTypes/Zone.hpp>

#include <fort/myrmidon/priv/FrameReference.hpp>
#include <fort/myrmidon/priv/Space.hpp>

class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;

class ZoneDefinitionBridge : public Bridge {
	Q_OBJECT
public:
	ZoneDefinitionBridge(const fmp::Zone::Ptr & zone,
	                     const fmp::ZoneDefinition::Ptr & ptr);
	virtual ~ZoneDefinitionBridge();
	bool isActive() const override;

	const fm::Shape::List & shapes() const;

	void setShapes(const fm::Shape::List & shapes);

	const fmp::Zone & zone() const;
signals:
	void countUpdated(int i);

private:
	fmp::ZoneDefinition::Ptr d_definition;
	fmp::Zone::ConstPtr      d_zone;
};


class ZoneBridge : public GlobalBridge {
	Q_OBJECT
public :
	ZoneBridge(QObject * parent);
	virtual ~ZoneBridge();

	QAbstractItemModel * spaceModel() const;
	QAbstractItemModel * fullFrameModel() const;

	void selectTime(const fort::Time & time);

	bool canAddItemAt(const QModelIndex & index);
	bool canRemoveItemAt(const QModelIndex & index);

	struct FullFrame {
		fmp::FrameReference Reference;
		QString             AbsoluteFilePath;
	};

	std::pair<bool,FullFrame> fullFrameAtIndex(const QModelIndex & index) const;

	void initialize(ExperimentBridge * experiment) override;

signals:
	void newZoneDefinitionBridge(QList<ZoneDefinitionBridge*>);

	void definitionUpdated();
public slots:
	void addItemAtIndex(const QModelIndex & index);
	void removeItemAtIndex(const QModelIndex & index);



	void activateItem(QModelIndex index);

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;


private slots:
	void onItemChanged(QStandardItem * item);

	void rebuildSpaces();
	void onTrackingDataDirectoryChange(const QString & uri);

private:
	const static int TypeRole;
	const static int DataRole;

	const static int SpaceType       = 1;
	const static int ZoneType        = 2;
	const static int DefinitionType  = 3;

	void clearSpaces();

	void clearFullFrames();

	void rebuildFullFrameModel();

	void rebuildChildBridges();

	QStandardItem * getSibling(QStandardItem * item,int column);

	void addZone(QStandardItem * spaceRootItem);
	void addDefinition(QStandardItem * zoneRootItem);

	void removeZone(QStandardItem * zoneItem);
	void removeDefinition(QStandardItem * zoneItem);

	void changeZoneName(QStandardItem * zoneNameItem);
	void changeDefinitionTime(QStandardItem * definitionStartItem, bool start);

	QList<QStandardItem*> buildSpace(const fmp::SpacePtr & space) const;
	QList<QStandardItem*> buildZone(const fmp::ZonePtr & zone) const;
	QList<QStandardItem*> buildDefinition(const std::shared_ptr<fmp::ZoneDefinition> & pdefinition) const;

	QStandardItemModel  * d_spaceModel;
	QStandardItemModel  * d_fullFrameModel;
	fmp::Space::Ptr       d_selectedSpace;
	fort::Time            d_selectedTime;

	std::vector<std::shared_ptr<ZoneDefinitionBridge>> d_childBridges;
};
