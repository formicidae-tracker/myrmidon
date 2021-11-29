#include "AntMetadataWorkspace.hpp"
#include "ui_AntMetadataWorkspace.h"

#include <fort/studio/Format.hpp>
#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/AntKeyValueBridge.hpp>


#include <fort/studio/MyrmidonTypes/AntMetadata.hpp>



#include <QDebug>

AntMetadataWorkspace::AntMetadataWorkspace(QWidget *parent)
	: Workspace(true,parent)
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

void AntMetadataWorkspace::initialize(AntKeyValueBridge * bridge) {
	d_keyValues = bridge;

	d_ui->keyTypeEditor->setup(bridge);

	d_ui->dataView->setModel(bridge->dataModel());
	d_ui->dataView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	d_ui->dataView->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(d_ui->dataView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        [this]() {
		        d_ui->removeButton->setEnabled(d_ui->dataView->selectionModel()->hasSelection());
	        });

	connect(d_ui->addButton,
	        &QToolButton::clicked,
	        this,
	        &AntMetadataWorkspace::onAddButtonClicked);

	connect(d_ui->removeButton,
	        &QToolButton::clicked,
	        this,
	        &AntMetadataWorkspace::onRemoveButtonClicked);

}



void AntMetadataWorkspace::onAddButtonClicked() {
}

void AntMetadataWorkspace::onRemoveButtonClicked() {
}


void AntMetadataWorkspace::setUp(const NavigationAction & actions) {

}

void AntMetadataWorkspace::tearDown(const NavigationAction & actions) {

}
