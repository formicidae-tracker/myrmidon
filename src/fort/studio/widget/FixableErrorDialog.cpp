#include "FixableErrorDialog.hpp"
#include "ui_FixableErrorDialog.h"

#include <QDebug>

FixableErrorDialog::FixableErrorDialog(fm::FixableErrorList errors,
                                       const QString & context,
                                       QWidget *parent)
	: QDialog(parent)
	, d_ui(new Ui::FixableErrorDialog)
	, d_errors(std::move(errors)) {
	d_ui->setupUi(this);

	buildTable();

	d_ui->label->setText(tr("Found %1 error(s) when %2").arg(d_errors.size()).arg(context));

}

FixableErrorDialog::~FixableErrorDialog() {
	delete d_ui;
}

void FixableErrorDialog::fixSelected() {
	for ( int i = 0; i < d_ui->tableWidget->rowCount(); ++i) {
		auto item = d_ui->tableWidget->item(i,0);
		if ( item->checkState() == Qt::Unchecked ) {
			continue;
		}
		try {
			d_errors.at(i)->Fix();
		} catch ( const std::exception & e ) {
			qCritical() << "Could not " << d_errors.at(i)->FixDescription().c_str()
			            << ": " << e.what();
		}
	}
}

void FixableErrorDialog::buildTable() {
	d_ui->tableWidget->setColumnCount(3);
	d_ui->tableWidget->setRowCount(d_errors.size());
	d_ui->tableWidget->setHorizontalHeaderLabels(
	    {tr(""), tr("Error"), tr("Fix")}
	);
	auto hHeader = d_ui->tableWidget->horizontalHeader();
	hHeader->resizeSection(0, 30);
	hHeader->resizeSection(1, 385);
	hHeader->resizeSection(2, 385);
	d_ui->tableWidget->verticalHeader()->setSectionResizeMode(
	    QHeaderView::ResizeToContents
	);
	for (size_t i = 0; i < d_errors.size(); ++i) {
		const auto &e         = *d_errors.at(i);
		auto        checkItem = new QTableWidgetItem("");
		checkItem->setFlags(
		    Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable
		);
		checkItem->setCheckState(Qt::Checked);
		auto whatItem = new QTableWidgetItem(tr(e.message()));
		whatItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		auto fixItem = new QTableWidgetItem(tr(e.FixDescription().c_str()));
		fixItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		d_ui->tableWidget->setItem(i, 0, checkItem);
		d_ui->tableWidget->setItem(i, 1, whatItem);
		d_ui->tableWidget->setItem(i, 2, fixItem);
	}
	d_ui->tableWidget->verticalHeader()->resizeSections(
	    QHeaderView::ResizeToContents
	);
}

void FixableErrorDialog::on_tableWidget_cellClicked(int row,int column) {
	if ( column != 0 ) {
		return;
	}

	int count = 0;
	for ( int i = 0; i < d_ui->tableWidget->rowCount(); ++i) {
		auto item = d_ui->tableWidget->item(i,0);
		if ( item->checkState() == Qt::Checked ) {
			++count;
		}
	}

	if ( count == d_ui->tableWidget->rowCount() ) {
		d_ui->checkBox->setCheckState(Qt::Checked);
	} else if ( count > 0 ) {
		d_ui->checkBox->setCheckState(Qt::PartiallyChecked);
	} else {
		d_ui->checkBox->setCheckState(Qt::Unchecked);
	}
}


void FixableErrorDialog::on_checkBox_stateChanged() {
	auto state = d_ui->checkBox->checkState();
	if ( state == Qt::PartiallyChecked ) {
		return;
	}

	for ( int i = 0; i < d_ui->tableWidget->rowCount(); ++i) {
		auto item = d_ui->tableWidget->item(i,0);
		item->setCheckState(state);
	}
}

bool FixableErrorDialog::promptForFix(const QString & path,
                                      fm::FixableErrorList errors,
                                      QWidget * parent) {

	if (errors.empty()) {
		return true;
	}

	auto errorDialog = new FixableErrorDialog(std::move(errors),
	                                          tr("opening Tracking Data Directory '%1'").arg(path),
	                                          parent);
	if( errorDialog->exec() == QDialog::Rejected ) {
		delete errorDialog;
		return false;
	}
	errorDialog->fixSelected();
	delete errorDialog;
	return true;
}
