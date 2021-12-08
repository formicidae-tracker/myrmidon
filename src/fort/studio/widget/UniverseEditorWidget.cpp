#include "UniverseEditorWidget.hpp"
#include "ui_UniverseEditorWidget.h"

#include <fort/studio/bridge/UniverseBridge.hpp>

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QEventLoop>

#include "SpaceChoiceDialog.hpp"

#include "TrackingDataDirectoryLoader.hpp"

#include "FixableErrorDialog.hpp"

UniverseEditorWidget::UniverseEditorWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::UniverseEditorWidget) {
	d_ui->setupUi(this);

	d_ui->addButton->setEnabled(false);
	d_ui->deleteButton->setEnabled(false);

	d_ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

}

UniverseEditorWidget::~UniverseEditorWidget() {
	delete d_ui;
}


void UniverseEditorWidget::setup(UniverseBridge * universe) {
	d_universe = universe;
	connect(d_universe,
	        &UniverseBridge::activated,
	        d_ui->addButton,
	        &QToolButton::setEnabled);

	d_ui->treeView->setModel(d_universe->model());

	connect(d_ui->treeView->selectionModel(),
	        &QItemSelectionModel::selectionChanged,
	        this,
	        &UniverseEditorWidget::onSelectionChanged);

	connect(d_universe->model(),
	        &QAbstractItemModel::rowsInserted,
	        [this](const QModelIndex & parent, int first, int last) {
		        if (parent.isValid() == true) {
			        return;
		        }
		        for(; first <= last; ++first) {
			        d_ui->treeView->expand(d_universe->model()->index(first,0,parent));
		        }
	        });

}



fmp::TrackingDataDirectory::Ptr UniverseEditorWidget::openTDD(const QString & path) {

	QFutureWatcher<void> watcher;
	QEventLoop loop;

	fmp::TrackingDataDirectory::Ptr res;
	fm::FixableErrorList errors;
	connect(&watcher,&QFutureWatcher<void>::finished,
	        &loop,&QEventLoop::quit);

	auto dialog  = new QProgressDialog(tr("Gathering %1 time data").arg(path),
	                                   QString(),
	                                   0,-1,
	                                   this,
	                                   Qt::Dialog | Qt::WindowTitleHint | Qt::FramelessWindowHint);
	dialog->setAutoReset(false);
	dialog->setMinimumDuration(250);
	dialog->setValue(0);
	dialog->setWindowModality(Qt::ApplicationModal);

	auto openDataDir
		= [&res,
		   &errors,
		   &path,
		   dialog,
		   this]() {
			  try {
				  std::tie(res,errors)
					  = fmp::TrackingDataDirectory::Open(path.toUtf8().constData(),
					                                     d_universe->basepath().toUtf8().constData(),
					                                     [dialog](int done, int total ) {
						                                     QMetaObject::invokeMethod(dialog, "setMaximum", Qt::QueuedConnection,
						                                                               Q_ARG( int, total ) );
						                                     QMetaObject::invokeMethod(dialog, "setValue", Qt::QueuedConnection,
						                                                               Q_ARG( int, done ) );

					                                     });
			  } catch (const std::exception & e) {
				  qCritical() << "Could not open TrackingDataDirectory"
				              << path << ": " << e.what();
			  }
			  dialog->reset();
		  };
	watcher.setFuture(QtConcurrent::run(openDataDir));

	loop.exec();
	dialog->deleteLater();

	if (errors.empty() == false ) {
		promptForFix(path,std::move(errors));
	}

	return res;
}

void UniverseEditorWidget::on_addButton_clicked() {
	if ( d_universe == NULL ) {
		return;
	}

	auto tddFilePath = QFileDialog::getExistingDirectory(this, tr("Open Tracking Data Directory"),
	                                                     d_universe->basepath(),
	                                                     QFileDialog::ShowDirsOnly);

	if ( tddFilePath.isEmpty() ) {
		return;
	}

	addTrackingDataDirectory(tddFilePath);
}


void UniverseEditorWidget::addTrackingDataDirectory(const QString & filepath) {

	fmp::TrackingDataDirectory::Ptr tdd = openTDD(filepath);
	if ( !tdd == true ) {
		return;
	}
	try {
		TrackingDataDirectoryLoader::EnsureLoaded({tdd},this);
	} catch ( const std::exception & e ) {
		return;
	}

	auto space = SpaceChoiceDialog::Get(d_universe,this);

	if ( space.isEmpty() ) {
		qDebug() << "[UniverseEditorWidget]: TDD addition aborded by user";
		return;
	}

	if ( d_universe->spaceExists(space) == false ) {
		d_universe->addSpace(space);
	}

	d_universe->addTrackingDataDirectoryToSpace(space,tdd);
}

void UniverseEditorWidget::on_deleteButton_clicked() {
	if ( !d_universe ) {
		return;
	}

	auto selected = d_ui->treeView->selectionModel()->selectedIndexes();
	if ( selected.isEmpty() ) {
		return;
	}

	d_universe->deleteSelection(selected);
}


void UniverseEditorWidget::onSelectionChanged(const QItemSelection & selection) {
	if ( selection.isEmpty() ) {
		d_ui->deleteButton->setEnabled(false);
		return;
	}
	const auto & currentSelection  =d_ui->treeView->selectionModel()->selectedIndexes();
	d_ui->deleteButton->setEnabled(d_universe->isDeletable(currentSelection));
}


void UniverseEditorWidget::promptForFix(const QString & path,fm::FixableErrorList  errors) {

	if (errors.empty()) {
		return;
	}

	auto errorDialog = new FixableErrorDialog(std::move(errors),
	                                          tr("opening Tracking Data Directory '%1'").arg(path),
	                                          this);
	if( errorDialog->exec() == QDialog::Rejected ) {
		delete errorDialog;
		return;
	}
	errorDialog->fixSelected();
	delete errorDialog;
}
