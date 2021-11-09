#include "VisualizationWorkspace.hpp"
#include "ui_VisualizationWorkspace.h"

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/MovieBridge.hpp>
#include <fort/studio/bridge/UniverseBridge.hpp>

#include <fort/studio/widget/TrackingVideoPlayer.hpp>
#include <fort/studio/widget/AntListWidget.hpp>

#include <QAction>
#include <QShortcut>
#include <QClipboard>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLineEdit>
#include <QAbstractButton>
#include <QDockWidget>
#include <QMainWindow>
#include <QTreeView>
#include <QDebug>
#include <QHeaderView>

#include <fort/studio/Format.hpp>

void VisualizationWorkspace::setUpUI() {
	d_antDisplay = new AntDisplayListWidget(this);
	d_antDisplayDock = new QDockWidget(tr("Ants Display States"));
	d_antDisplayDock->setWidget(d_antDisplay);
	d_antDisplay->setMaximumSize(QSize(400,65535));

	auto widget = new QWidget(this);
	auto layout = new QVBoxLayout();

	d_treeView = new QTreeView(widget);
	d_treeView->header()->setVisible(true);

	layout->addWidget(d_treeView);

	widget->setLayout(layout);
	widget->setMaximumSize(QSize(400,65535));

	d_segmentListDock = new QDockWidget(tr("Movie Segments"),this);
	d_segmentListDock->setWidget(widget);
}

VisualizationWorkspace::VisualizationWorkspace(QWidget *parent)
	: Workspace(true,parent)
	, d_experiment(nullptr)
	, d_ui(new Ui::VisualizationWorkspace)
	, d_videoPlayer(new TrackingVideoPlayer(this)) {
	setUpUI();

	d_ui->setupUi(this);


	auto togglePlayPauseShortcut = new QShortcut(tr("Space"),this);
	auto nextFrameShortcut = new QShortcut(tr("."),this);
	auto prevFrameShortcut = new QShortcut(tr(","),this);

	auto skipForwardSmallShortcut = new QShortcut(tr("L"),this);
	auto skipBackwardSmallShortcut = new QShortcut(tr("J"),this);

	auto skipForwardMediumShortcut = new QShortcut(tr("Shift+L"),this);
	auto skipBackwardMediumShortcut = new QShortcut(tr("Shift+J"),this);

	auto skipForwardLargeShortcut = new QShortcut(tr("Ctrl+Shift+L"),this);
	auto skipBackwardLargeShortcut = new QShortcut(tr("Ctrl+Shift+J"),this);

	static fort::Duration small = 10 * fort::Duration::Second;
	static fort::Duration medium = 1 * fort::Duration::Minute;
	static fort::Duration large = 10 * fort::Duration::Minute;

	connect(d_videoPlayer,
	        &TrackingVideoPlayer::displayVideoFrame,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::display);

	connect(d_videoPlayer,
	        &TrackingVideoPlayer::seekReady,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::hideLoadingBanner);

	connect(d_ui->trackingVideoWidget,&TrackingVideoWidget::togglePlayPause,
	        d_videoPlayer,&TrackingVideoPlayer::togglePlayPause);

	connect(togglePlayPauseShortcut,&QShortcut::activated,
	        d_videoPlayer,&TrackingVideoPlayer::togglePlayPause);

	connect(nextFrameShortcut,&QShortcut::activated,
	        d_videoPlayer,&TrackingVideoPlayer::jumpNextFrame);

	connect(prevFrameShortcut,&QShortcut::activated,
	        d_videoPlayer,&TrackingVideoPlayer::jumpPrevFrame,
	        Qt::QueuedConnection);

	connect(skipForwardSmallShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(small);
	             });

	connect(skipBackwardSmallShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(-small);
	             });

	connect(skipForwardMediumShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(medium);
	             });

	connect(skipBackwardMediumShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(-medium);
	             });

	connect(skipForwardLargeShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(large);
	             });

	connect(skipBackwardLargeShortcut,&QShortcut::activated,
	        this,[this]() {
		             d_videoPlayer->skipDuration(-large);
	             });

}

VisualizationWorkspace::~VisualizationWorkspace() {
	delete d_ui;
}

void VisualizationWorkspace::onMovieSegmentActivated ( const QModelIndex & index ) {
	auto movieBridge = d_experiment->movies();
	const auto & [spaceID,tdd,segment,start]  = movieBridge->tddAndMovieSegment(index);
	if ( !segment == true || !tdd == true || spaceID == 0) {
		return;
	}
	d_videoPlayer->setMovieSegment(spaceID,tdd,segment,start);
	d_videoPlayer->play();
}

void VisualizationWorkspace::initialize(QMainWindow * main,ExperimentBridge * experiment) {
	d_experiment = experiment;
	auto movieBridge = experiment->movies();

	d_treeView->setModel(movieBridge->movieModel());
	d_treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	connect(movieBridge->movieModel(),
	        &QAbstractItemModel::rowsInserted,
	        d_treeView,
	        &QTreeView::expandAll);
	d_treeView->expandAll();

	connect(movieBridge,&Bridge::activated,
	        d_videoPlayer,&TrackingVideoPlayer::clearMovieSegment);

	connect(d_treeView,
	        &QAbstractItemView::activated,
	        this,
	        &VisualizationWorkspace::onMovieSegmentActivated);

	d_videoPlayer->setup(experiment->identifiedFrameLoader());
	d_ui->trackingVideoWidget->setup(experiment->antDisplay());
	d_ui->videoControl->setup(d_videoPlayer,experiment);

	connect(d_ui->videoControl,
	        &TrackingVideoControl::zoomFocusChanged,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::setZoomFocus);

	connect(d_ui->videoControl,
	        &TrackingVideoControl::showID,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::setShowID);

	connect(d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::showIDChanged,
	        d_ui->videoControl,
	        &TrackingVideoControl::setShowID);

	connect(d_ui->videoControl,
	        &TrackingVideoControl::showCollisions,
	        d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::setShowCollisions);

	connect(d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::showCollisionsChanged,
	        d_ui->videoControl,
	        &TrackingVideoControl::setShowCollisions);

	d_ui->videoControl->setShowID(d_ui->trackingVideoWidget->showID());


	d_videoPlayer->clearMovieSegment();

	d_antDisplay->initialize(experiment);

	main->addDockWidget(Qt::LeftDockWidgetArea,d_antDisplayDock);
	d_antDisplayDock->hide();

	main->addDockWidget(Qt::LeftDockWidgetArea,d_segmentListDock);
	d_segmentListDock->hide();

	float height = main->size().height();
	int antDisplayHeight = 0.5 * height;
	int segmentListHeight = 0.5 * height;
	main->resizeDocks({d_antDisplayDock,d_segmentListDock},{antDisplayHeight,segmentListHeight},Qt::Vertical);
}

void VisualizationWorkspace::onCopyTimeActionTriggered() {
	if ( d_ui->trackingVideoWidget->hasTrackingTime() == false ) {
		return;
	}
	auto time = d_ui->trackingVideoWidget->trackingTime();
	QApplication::clipboard()->setText(ToQString(time));
}

void VisualizationWorkspace::setUp(const NavigationAction & actions) {
	connect(d_ui->trackingVideoWidget,
	        &TrackingVideoWidget::hasTrackingTimeChanged,
	        actions.CopyCurrentTime,
	        &QAction::setEnabled);

	connect(actions.CopyCurrentTime,
	        &QAction::triggered,
	        this,
	        &VisualizationWorkspace::onCopyTimeActionTriggered);

	connect(actions.JumpToTime,&QAction::triggered,
	        this,&VisualizationWorkspace::jumpToTimeAction);

	actions.CopyCurrentTime->setEnabled(d_ui->trackingVideoWidget->hasTrackingTime());

	actions.JumpToTime->setEnabled(true);
	d_antDisplayDock->show();
	d_segmentListDock->show();
}

void VisualizationWorkspace::tearDown(const NavigationAction & actions) {
	disconnect(d_ui->trackingVideoWidget,
	           &TrackingVideoWidget::hasTrackingTimeChanged,
	           actions.CopyCurrentTime,
	           &QAction::setEnabled);

	disconnect(actions.CopyCurrentTime,
	           &QAction::triggered,
	           this,
	           &VisualizationWorkspace::onCopyTimeActionTriggered);

	disconnect(actions.JumpToTime,&QAction::triggered,
	        this,&VisualizationWorkspace::jumpToTimeAction);

	actions.CopyCurrentTime->setEnabled(false);
	actions.JumpToTime->setEnabled(false);
	d_antDisplayDock->hide();
	d_segmentListDock->hide();
}


void VisualizationWorkspace::jumpToTimeAction() {
	if ( d_experiment == nullptr || d_experiment->isActive() == false ) {
		return;
	}
    QDialog dialog(this);
    dialog.setMinimumWidth(400);
    dialog.setWindowModality(Qt::ApplicationModal);

    auto layout = new QFormLayout(&dialog);
    dialog.setLayout(layout);

    auto spaceCombo = new QComboBox(&dialog);
    for ( const auto & [spaceID,spaceName] : d_experiment->universe()->spaceNamesByID() ) {
	    spaceCombo->addItem(spaceName,spaceID);
    }
    layout->addRow(tr("Space:"),spaceCombo);

    auto lineEdit = new QLineEdit(&dialog);
    auto warning = lineEdit->addAction(QIcon::fromTheme("dialog-warning-symbolic"),QLineEdit::TrailingPosition);
    warning->setVisible(false);
    layout->addRow(tr("Time:"),lineEdit);


    auto buttonBox = new QDialogButtonBox(&dialog);

    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    auto okButton = buttonBox->buttons()[0];
    okButton->setEnabled(false);
    layout->addRow(buttonBox);
    connect(buttonBox,&QDialogButtonBox::accepted,
            &dialog,&QDialog::accept);
    connect(buttonBox,&QDialogButtonBox::rejected,
            &dialog,&QDialog::reject);

    connect(lineEdit,&QLineEdit::editingFinished,
            &dialog,[lineEdit,warning,okButton,&dialog](){
	                    try {
		                    fort::Time::Parse(ToStdString(lineEdit->text()));
		                    okButton->setEnabled(true);
		                    warning->setVisible(false);
	                    } catch ( const std::exception & e) {
		                    okButton->setEnabled(false);
		                    warning->setVisible(true);
	                    }
                    });

    QEventLoop loop;
    connect(&dialog,&QDialog::finished,
            &loop,&QEventLoop::quit);

    dialog.open();
    loop.exec();

    if ( dialog.result() == QDialog::Rejected
         || lineEdit->text().isEmpty() == true ) {
	    return;
    }

    fort::Time wanted;
    try {
	    wanted = fort::Time::Parse(ToStdString(lineEdit->text()));
    } catch (  const std::exception & e ) {
	    return;
    }
    auto spaceID = spaceCombo->currentData().toInt();
    jumpToTime(spaceID,wanted);

}


void VisualizationWorkspace::jumpToTime(uint32_t spaceID,
                                        const fort::Time & time) {
	auto [tdd,segment,start] = d_experiment->movies()->findTime(spaceID,
                                                                time);
    if ( !tdd || !segment ) {
	    qCritical() << "Could not find time " << ToQString(time) << " in space "
	                << spaceID;
	    return;
    }

    d_videoPlayer->pause();
    const auto & currentSegment = d_videoPlayer->currentSegment();
    if ( !currentSegment == true ||
         currentSegment->URI() != segment->URI() ) {
	    d_videoPlayer->setMovieSegment(spaceID,tdd,segment,start);
    }
    d_videoPlayer->setTime(time);
}
