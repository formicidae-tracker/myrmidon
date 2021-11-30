#include "SetAntValueDialog.hpp"
#include "ui_SetAntValueDialog.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/AntKeyValueBridge.hpp>

SetAntValueDialog::SetAntValueDialog(QWidget *parent)
	: QDialog(parent)
	, d_ui(new Ui::SetAntValueDialog)
	, d_inTime(fort::Time::SinceEver())
	, d_outTime(fort::Time::Forever()) {
	d_ui->setupUi(this);

	connect(this,&SetAntValueDialog::inTimeChanged,
	        this,[this]() {
		        d_ui->fromTimeLabel->setText(d_inTime.Format().c_str());
		        updateState();
	        });
	connect(this,&SetAntValueDialog::outTimeChanged,
	        this,[this]() {
		        d_ui->toTimeLabel->setText(d_outTime.Format().c_str());
		        updateState();
	        });

}

SetAntValueDialog::~SetAntValueDialog() {
    delete d_ui;
}

void SetAntValueDialog::initialize(ExperimentBridge * experiment) {
	d_ui->keyComboBox->setModel(experiment->antKeyValues()->keyModel());
	connect(experiment,&ExperimentBridge::antSelected,
	        this,[this](quint32 antID) {
		             setWindowTitle(tr("Set Value for Ant %1").arg(fm::FormatAntID(antID).c_str()));
		             updateState();
	             });
}


const fort::Time & SetAntValueDialog::inTime() const {
	return d_inTime;
}

const fort::Time & SetAntValueDialog::outTime() const {
	return d_outTime;
}

fm::AntStaticValue SetAntValueDialog::value() const {
	return false;
}

QString SetAntValueDialog::key() const {
	return d_ui->keyComboBox->currentText();
}


void SetAntValueDialog::setInTime(const fort::Time & v) {
	auto inTime = std::min(v,d_outTime);
	if ( inTime == d_inTime ) {
		return;
	}
	d_inTime = inTime;
	emit inTimeChanged(d_inTime);
}

void SetAntValueDialog::setOutTime(const fort::Time & v) {
	auto outTime = std::max(v,d_inTime);
	if ( outTime == d_outTime ) {
		return;
	}
	d_outTime = outTime;
	emit outTimeChanged(d_outTime);
}


void SetAntValueDialog::showEvent(QShowEvent * event) {
	QDialog::showEvent(event);
	QApplication::beep();
}

void SetAntValueDialog::updateState() {
}


void SetAntValueDialog::on_keyComboBox_currentIndexChanged(int) {
	d_ui->valueEdit->clear();


}
