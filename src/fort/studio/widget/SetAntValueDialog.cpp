#include "SetAntValueDialog.hpp"
#include "ui_SetAntValueDialog.h"

#include <fort/myrmidon/types/ValueUtils.hpp>

#include <fort/studio/Format.hpp>
#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/AntKeyValueBridge.hpp>

#include <QPushButton>


SetAntValueDialog::SetAntValueDialog(QWidget *parent)
	: QDialog(parent)
	, d_ui(new Ui::SetAntValueDialog)
	, d_experiment(nullptr)
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
	d_ui->conflictTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	updateState();
}

SetAntValueDialog::~SetAntValueDialog() {
    delete d_ui;
}

void SetAntValueDialog::initialize(ExperimentBridge * experiment) {
	d_experiment = experiment;
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
	d_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(hasAnt() && hasTime() && hasKey() && hasValue());
	updateConflicts();
}

void SetAntValueDialog::updateConflicts() {
	fm::ValueUtils::ValuedTimeRangeList conflicts = findConflicts();

	if ( conflicts.empty() ) {
		d_ui->conflictTable->setRowCount(0);
		d_ui->conflictSection->setEnabled(false);
		d_ui->mergeButton->setText(tr("Apply *value* only on ranges where *key* is the *default value*"));
		this->adjustSize();
		return;
	}

	d_ui->conflictTable->setRowCount(conflicts.size());
	int row = -1;
	for ( const auto & vr : conflicts ) {
		++row;
		d_ui->conflictTable->setItem(row,0,new QTableWidgetItem(ToQString(vr.Start.Round(fort::Duration::Millisecond))));
		d_ui->conflictTable->setItem(row,1,new QTableWidgetItem(ToQString(vr.End.Round(fort::Duration::Millisecond))));
		d_ui->conflictTable->setItem(row,2,new QTableWidgetItem(ToQString(vr.Value)));
	}
	auto keyName = d_ui->keyComboBox->currentText();
	auto defaultValue = d_keyValues->defaultValue(keyName);
	d_ui->mergeButton->setText(tr("Apply '%1' only on ranges where '%2' is '%3'")
	                           .arg(ToQString(d_value))
	                           .arg(keyName)
	                           .arg(ToQString(defaultValue)));
	d_ui->conflictSection->setEnabled(true);
	this->adjustSize();
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
		updateState();
		return;
	}
	try {
		auto type = fm::ValueType(d_ui->keyComboBox->currentData(AntKeyValueBridge::KeyTypeRole).toInt());
		d_value = fm::ValueUtils::Parse(type,text.toUtf8().constData());
		setHasValue(true);
	} catch ( const std::exception & ) {
		setHasValue(false);
	}
	updateState();
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

bool SetAntValueDialog::hasAnt() const {
	return d_experiment != nullptr && d_experiment->selectedAntID() != 0;
}
#include <QDebug>

fm::ValueUtils::ValuedTimeRangeList SetAntValueDialog::findConflicts() {
	if ( !hasAnt() || !hasKey() || !hasValue() ) {
		return {};
	}

	auto key = d_ui->keyComboBox->currentText();


	const auto & values = d_keyValues->getValues(d_experiment->selectedAntID(),
	                                             key);
	qInfo() << "FOOOOOOOO";
	qInfo() << key << ToQString(d_keyValues->defaultValue(key)) << ToQString(d_value);
	for ( const auto & [time,value] : values ) {
		qInfo() << "++" << ToQString(time) << ToQString(value);
	}
	auto res = fm::ValueUtils::FindConflicts(values,
	                                         d_keyValues->defaultValue(key),
	                                         {.Value = d_value,
	                                          .Start = d_inTime,
	                                          .End = d_outTime});

	for ( const auto & rv : res ) {
		qInfo() << "--" << ToQString(rv.Start) << ToQString(rv.End) << ToQString(rv.Value);
	}
	return res;
}
