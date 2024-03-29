#include "TimeEditorWidget.hpp"
#include "ui_TimeEditorWidget.h"

#include <fort/studio/Utils.hpp>
#include <fort/studio/Format.hpp>

#include <QIcon>
#include <QCalendarWidget>
#include <QTimeEdit>
#include <QAction>
#include <fort/studio/widget/FrameFinderDialog.hpp>

#include <fort/studio/bridge/UniverseBridge.hpp>


TimeEditorWidget::TimeEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TimeEditorWidget)
	, d_universe(nullptr) {
	d_ui->setupUi(this);


	d_warning = d_ui->lineEdit->addAction(QIcon::fromTheme("dialog-warning-symbolic"),QLineEdit::TrailingPosition);
	d_warning->setVisible(false);
	d_popup = d_ui->lineEdit->addAction(QIcon::fromTheme("appointment-new-symbolic"),QLineEdit::TrailingPosition);
	d_decrement = d_ui->lineEdit->addAction(QIcon::fromTheme("list-remove-symbolic"),QLineEdit::TrailingPosition);
	d_increment = d_ui->lineEdit->addAction(QIcon::fromTheme("list-add-symbolic"),QLineEdit::TrailingPosition);

	d_popup->setEnabled(false);
	d_decrement->setEnabled(false);
	d_increment->setEnabled(false);

	connect(d_increment,
	        &QAction::triggered,
	        this,
	        &TimeEditorWidget::increment);

	connect(d_decrement,
	        &QAction::triggered,
	        this,
	        &TimeEditorWidget::decrement);

	connect(d_popup,
	        &QAction::triggered,
	        this,
	        &TimeEditorWidget::onPopup);

	d_ui->frameButton->setEnabled(false);

	d_popupWidget = new QWidget();
	auto layout = new QVBoxLayout();
	d_timeEdit = new QTimeEdit();
	d_timeEdit->setDisplayFormat("hh:mm:ss");
	d_calendar = new QCalendarWidget();
	layout->addWidget(d_timeEdit);
	layout->addWidget(d_calendar);
	d_popupWidget->setLayout(layout);

	auto timeFromValue = [this](fort::Time time) {
		                     if (time.IsInfinite()) {
			                     if ( !d_universe || d_universe->trackingDataDirectories().empty() == true) {
				                     return;
			                     }
			                     time = d_universe->trackingDataDirectories().begin()->second->Start();
		                     }
		                     QDateTime date = QDateTime::fromSecsSinceEpoch(time.ToTimeT());
		                     d_timeEdit->setTime(date.time());
		                     d_calendar->setSelectedDate(date.date());
	                     };

	auto updateTimeFromPopup = [this]() {
		                           QDateTime dateTime(d_calendar->selectedDate(),d_timeEdit->time());
		                           try {
			                           auto time = fort::Time::Parse(ToStdString(dateTime.toString("yyyy-MM-ddThh:mm:ss.zzzZ")));
			                           setTime(time);
			                           emit timeChanged(time);
		                           } catch (const std::exception & e) {
			                           return;
		                           }
	                           };
	connect(this,
	        &TimeEditorWidget::timeChanged,
	        timeFromValue);
	connect(d_calendar,
	        &QCalendarWidget::clicked,
	        updateTimeFromPopup);
	connect(d_timeEdit,
	        &QTimeEdit::editingFinished,
	        updateTimeFromPopup);



}

TimeEditorWidget::~TimeEditorWidget() {
	delete d_popupWidget;
	delete d_ui;

}

fort::Time TimeEditorWidget::time() const {
	if ( d_ui->lineEdit->text().isEmpty() ) {
		return fort::Time::Forever();
	}

	try {
		return fort::Time::Parse(ToStdString(d_ui->lineEdit->text()));
	} catch ( const std::exception & e) {
		return fort::Time::Forever();
	}
}

void TimeEditorWidget::setTime(const fort::Time & time) {
	d_warning->setVisible(false);
	if ( time.IsInfinite() ) {
		d_ui->lineEdit->setText("");
		d_decrement->setEnabled(false);
		d_increment->setEnabled(false);
		return;
	}
	d_decrement->setEnabled(true);
	d_increment->setEnabled(true);

	d_ui->lineEdit->setText(ToQString(time));
}


void TimeEditorWidget::on_lineEdit_editingFinished() {
	if ( d_ui->lineEdit->text().isEmpty() == true ) {
		setTime(fort::Time::Forever());
		emit timeChanged(fort::Time::Forever());
		return;
	}

	fort::Time res;
	try {
		res = fort::Time::Parse(ToStdString(d_ui->lineEdit->text()));
	} catch ( const std::exception & e) {
		d_warning->setVisible(true);
		return;
	}
	d_warning->setVisible(false);

	emit timeChanged(res);
}

void TimeEditorWidget::increment() {
	incrementBy(1 * fort::Duration::Second);
}

void TimeEditorWidget::decrement() {
	incrementBy(-1 * fort::Duration::Second);
}

void TimeEditorWidget::incrementBy(fort::Duration duration) {
	if ( d_ui->lineEdit->text().isEmpty() == true ) {
		return;
	}
	fort::Time res;
	try {
		res = fort::Time::Parse(ToStdString(d_ui->lineEdit->text()));
	} catch ( const std::exception & e ) {
		return;
	}
	res = res.Add(duration);
	setTime(res);
	emit timeChanged(res);
}


void TimeEditorWidget::setup(UniverseBridge * universe) {
	d_universe = universe;
	d_popup->setEnabled(true);
	d_ui->frameButton->setEnabled(true);
}

void TimeEditorWidget::onPopup() {
	d_popupWidget->setGeometry(this->x(),this->y() + this->height(), 400,400);
	d_popupWidget->setWindowFlag(Qt::Popup);
	d_popupWidget->show();
}


void TimeEditorWidget::on_frameButton_clicked() {
	if ( d_universe == nullptr ) {
		return;
	}
	auto ref = FrameFinderDialog::Get(d_universe,this);
	if ( !ref ) {
		return;
	}
	auto time = ref->Time();
	setTime(time);
	emit timeChanged(time);
}
