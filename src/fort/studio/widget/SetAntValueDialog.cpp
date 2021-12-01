#include "SetAntValueDialog.hpp"
#include "ui_SetAntValueDialog.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/AntKeyValueBridge.hpp>

#include <QPushButton>


SetAntValueDialog::SetAntValueDialog(QWidget *parent)
	: QDialog(parent)
	, d_ui(new Ui::SetAntValueDialog)
	, d_keyValues(nullptr)
	, d_inTime(fort::Time::SinceEver())
	, d_outTime(fort::Time::Forever())
	, d_hasValue(false) {
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

	connect(this,&SetAntValueDialog::hasValueChanged,
	        this,&SetAntValueDialog::updateState);

	updateState();
}

SetAntValueDialog::~SetAntValueDialog() {
    delete d_ui;
}

void SetAntValueDialog::initialize(ExperimentBridge * experiment) {
	d_keyValues = experiment->antKeyValues();
	d_ui->keyComboBox->setModel(d_keyValues->keyModel());
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

const fm::Value & SetAntValueDialog::value() const {
	return d_value;
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
	updateValidatorAndCompleter();
}

void SetAntValueDialog::updateState() {
	d_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(hasTime() && hasKey() && hasValue());
}

void SetAntValueDialog::updateValidatorAndCompleter() {
	if ( d_ui->keyComboBox->currentIndex() >= 0 ) {
		auto type = fm::ValueType(d_ui->keyComboBox->currentData(AntKeyValueBridge::KeyTypeRole).toInt());
		d_ui->valueEdit->setValidator(d_keyValues->validatorForType(type));
		d_ui->valueEdit->setCompleter(d_keyValues->completerForType(type));
	} else {
		d_ui->valueEdit->setValidator(nullptr);
		d_ui->valueEdit->setCompleter(nullptr);
	}
}

void SetAntValueDialog::on_keyComboBox_currentIndexChanged(int) {
	d_ui->valueEdit->clear();
	updateValidatorAndCompleter();
	updateState();
}

void SetAntValueDialog::on_valueEdit_textChanged(const QString & text) {
	if ( d_ui->keyComboBox->currentIndex() < 0 ) {
		setHasValue(false);
		return;
	}
	try {
		auto type = fm::ValueType(d_ui->keyComboBox->currentData(AntKeyValueBridge::KeyTypeRole).toInt());
		d_value = fm::ValueUtils::Parse(type,text.toUtf8().constData());
		setHasValue(true);
	} catch ( const std::exception & ) {
		setHasValue(false);
	}
}

void SetAntValueDialog::setHasValue(bool valid) {
	if ( d_hasValue == valid ) {
		return;
	}
	d_hasValue = valid;
	emit hasValueChanged(valid);
}

bool SetAntValueDialog::hasValue() const {
	return d_hasValue;
}

bool SetAntValueDialog::hasTime() const {
	return d_inTime.IsInfinite() == false || d_outTime.IsInfinite() == false;
}
bool SetAntValueDialog::hasKey() const {
	return d_ui->keyComboBox->currentIndex() >= 0;
}
