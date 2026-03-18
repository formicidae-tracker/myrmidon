#include "AntMetadataWorkspace.hpp"
#include "fort/myrmidon/utils/Exception.hpp"
#include "ui_AntMetadataWorkspace.h"

#include <fort/studio/Format.hpp>
#include <fort/studio/bridge/AntKeyValueBridge.hpp>
#include <fort/studio/bridge/ExperimentBridge.hpp>

#include <fort/studio/MyrmidonTypes/Value.hpp>

AntMetadataWorkspace::AntMetadataWorkspace(QWidget *parent)
    : Workspace(true, parent)
    , d_ui(new Ui::AntMetadataWorkspace) {
	d_ui->setupUi(this);
	d_ui->removeButton->setEnabled(false);
}

AntMetadataWorkspace::~AntMetadataWorkspace() {
	delete d_ui;
}

void AntMetadataWorkspace::initialize(QMainWindow * main,ExperimentBridge * experiment) {
	initialize(experiment->antKeyValues());
}

void AntMetadataWorkspace::initialize(AntKeyValueBridge *bridge) {
	d_keyValues = bridge;

	d_ui->keyTypeEditor->setup(bridge);

	d_ui->dataView->setModel(bridge->dataModel());
	d_ui->dataView->header()->setSectionResizeMode(QHeaderView::ResizeToContents
	);
	d_ui->dataView->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(
	    d_ui->dataView->selectionModel(),
	    &QItemSelectionModel::selectionChanged,
	    this,
	    &AntMetadataWorkspace::onSelectionChanged
	);

	connect(
	    d_ui->addButton,
	    &QToolButton::clicked,
	    this,
	    &AntMetadataWorkspace::onAddButtonClicked
	);

	connect(
	    d_ui->removeButton,
	    &QToolButton::clicked,
	    this,
	    &AntMetadataWorkspace::onRemoveButtonClicked
	);

	connect(
	    bridge->dataModel(),
	    &QAbstractItemModel::rowsInserted,
	    [this](const QModelIndex &parent, int first, int last) {
		    if (parent.isValid() == true) {
			    return;
		    }
		    for (; first <= last; ++first) {
			    d_ui->dataView->expand(d_keyValues->dataModel()->index(first, 0)
			    );
		    }
	    }
	);

	connect(bridge->dataModel(), &QAbstractItemModel::modelReset, [this]() {
		auto model = d_keyValues->dataModel();
		for (int i = 0; i < model->rowCount(); ++i) {
			d_ui->dataView->expand(model->index(i, 0));
		}
	});
	onSelectionChanged();
}

void AntMetadataWorkspace::onAddButtonClicked() {
	auto index = d_ui->dataView->selectionModel()->selectedRows().front();
	auto antID = index.data(AntKeyValueBridge::AntIDRole).toInt();
	auto key = index.data(AntKeyValueBridge::KeyNameRole).toString();
	d_keyValues->appendDefaultValue(antID,key);
}

void AntMetadataWorkspace::onRemoveButtonClicked() {
	auto index = d_ui->dataView->selectionModel()->selectedRows().front();
	auto antID = index.data(AntKeyValueBridge::AntIDRole).toInt();
	auto key   = index.data(AntKeyValueBridge::KeyNameRole).toString();

	auto logger = slog::With(
	    slog::String("module", "AntMetadataWorkspace"),
	    slog::Int("antID", antID),
	    slog::String("key", key.toStdString()),
	    slog::String(
	        "time",
	        index.data(AntKeyValueBridge::TimeRole).toString().toStdString()
	    )
	);
	try {
		auto time = fort::Time::Parse(
		    ToStdString(index.data(AntKeyValueBridge::TimeRole).toString())
		);
		d_keyValues->deleteValue(antID, key, time);
	} catch (const std::exception &e) {
		logger.Error(
		    "could not remove Ant metadata",
		    fort::myrmidon::utils::Err(e)
		);
	}
}

void AntMetadataWorkspace::setUp(const NavigationAction & actions) {

}

void AntMetadataWorkspace::tearDown(const NavigationAction & actions) {

}


void AntMetadataWorkspace::onSelectionChanged() {
	auto selectionModel = d_ui->dataView->selectionModel();

	if ( selectionModel->hasSelection() == false ) {
		d_ui->addButton->setEnabled(false);
		d_ui->removeButton->setEnabled(false);
		return;
	}

	auto index = selectionModel->selectedRows().front();
	if ( index.parent().isValid() == false ) {
		d_ui->addButton->setEnabled(false);
		d_ui->removeButton->setEnabled(false);
		return;
	}
	if ( index.parent().parent().isValid() == false ) {
		d_ui->addButton->setEnabled(true);
		d_ui->removeButton->setEnabled(false);
		return;
	}
	if ( index.parent().parent().parent().isValid() == false ) {
		d_ui->addButton->setEnabled(true);
		d_ui->removeButton->setEnabled(true);
		return;
	}

	d_ui->addButton->setEnabled(false);
	d_ui->removeButton->setEnabled(false);
}
