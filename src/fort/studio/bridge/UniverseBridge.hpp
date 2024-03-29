#pragma once

#include "Bridge.hpp"
#include <QStandardItemModel>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/types/Typedefs.hpp>

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>
#include <fort/studio/MyrmidonTypes/Space.hpp>

class QAbstractItemModel;

class UniverseBridge : public GlobalBridge {
	Q_OBJECT

public:

	UniverseBridge(QObject * parent);
	virtual ~UniverseBridge();

	QAbstractItemModel * model();

	bool isDeletable(const QModelIndexList & ) const;

	QString basepath() const;

	const fmp::TrackingDataDirectoryByURI & trackingDataDirectories() const;

	bool spaceExists(const QString & spaceName) const;

	std::map<quint32,QString> spaceNamesByID() const;

	void initialize(ExperimentBridge * experiment) override;

public slots:
	void addSpace(const QString & spaceName);
	void deleteSpace(const QString & spaceName);
	void addTrackingDataDirectoryToSpace(const QString & spaceName,
	                                     const fmp::TrackingDataDirectory::Ptr & tdd);
	void deleteTrackingDataDirectory(const QString & URI);


	void deleteSelection(const QModelIndexList & selected);

signals:
	void spaceDeleted(const QString & spaceName);
	void spaceAdded(const fmp::Space::Ptr & space);
	void spaceChanged(const fmp::Space::Ptr & space);


	void trackingDataDirectoryAdded(const fmp::TrackingDataDirectory::Ptr & tdd);
	void trackingDataDirectoryDeleted(const QString & URI);

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;

private slots:
	void onItemChanged(QStandardItem * item);

private:

	const static std::vector<fmp::Space::Ptr> s_emptySpaces;
	const static fmp::TrackingDataDirectoryByURI s_emptyTDDs;

	enum ObjectType {
	                 SPACE_TYPE = 1,
	                 TDD_TYPE  = 2,
	};

	QStandardItem * locateSpace(const QString & URI);
	void rebuildSpaceChildren(QStandardItem * item, const fmp::Space::Ptr & z);

	QList<QStandardItem*> buildTDD(const fmp::TrackingDataDirectory::Ptr & tdd);
	QList<QStandardItem*> buildSpace(const fmp::Space::Ptr & z);

	void rebuildAll(const fmp::SpaceByID & spaces);

	QStandardItemModel   * d_model;
};
