#include "SetAntValueDialog.hpp"
#include "ui_SetAntValueDialog.h"

SetAntValueDialog::SetAntValueDialog(QWidget *parent)
	: QDialog(parent)
	, d_ui(new Ui::SetAntValueDialog) {
	d_ui->setupUi(this);
}

SetAntValueDialog::~SetAntValueDialog() {
    delete d_ui;
}
