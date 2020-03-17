#include "AntIdentificationWidget.hpp"
#include "ui_AntIdentificationWidget.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/SelectedAntBridge.hpp>
#include <fort/studio/Format.hpp>

#include <QDebug>

AntIdentificationWidget::AntIdentificationWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::AntIdentificationWidget)
	, d_selectedAnt(nullptr) {
	d_ui->setupUi(this);
	auto hHeader = d_ui->tableView->horizontalHeader();
	hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
	d_ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	d_ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

AntIdentificationWidget::~AntIdentificationWidget() {
	delete d_ui;
}


void AntIdentificationWidget::setup(ExperimentBridge * experiment) {
	d_selectedAnt = experiment->selectedAnt();
	connect(d_selectedAnt,
	        &SelectedAntBridge::activated,
	        this,
	        &AntIdentificationWidget::onSelection);
	d_ui->tableView->setModel(d_selectedAnt->identificationModel());
	d_ui->identificationEditor->setup(experiment);
	connect(d_ui->tableView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        this,
	        &AntIdentificationWidget::onIdentificationSelectionChanged);

}

void  AntIdentificationWidget::onSelection() {
	if ( d_selectedAnt->isActive() == false ) {
		d_ui->groupBox->setTitle(tr("Selected Ant: None"));
		return;
	}

	d_ui->groupBox->setTitle(tr("Selected Ant: %1").arg(ToQString(fmp::Ant::FormatID(d_selectedAnt->selectedID()))));

}

void AntIdentificationWidget::on_tableView_doubleClicked(const QModelIndex & index) {
	d_selectedAnt->selectIdentification(index);
}


void AntIdentificationWidget::onIdentificationSelectionChanged() {
	if ( d_ui->tableView->selectionModel()->hasSelection() == true ) {
		d_ui->removeButton->setEnabled(true);
	} else {
		d_ui->removeButton->setEnabled(false);
	}
}

void AntIdentificationWidget::on_removeButton_clicked() {
	qWarning() << "Clicked";
	auto rows = d_ui->tableView->selectionModel()->selectedRows();
	if ( rows.isEmpty() == true ) {
		return;
	}
	d_selectedAnt->removeIdentification(rows[0]);
}