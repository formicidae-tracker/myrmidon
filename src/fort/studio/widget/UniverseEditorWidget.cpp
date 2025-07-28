#include "UniverseEditorWidget.hpp"
#include "fort/myrmidon/types/Reporter.hpp"
#include "ui_UniverseEditorWidget.h"

#include <fort/studio/bridge/UniverseBridge.hpp>
#include <fort/studio/widget/ProgressDialog.hpp>

#include <QDebug>
#include <QEventLoop>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QProgressDialog>
#include <QtConcurrent>
#include <qprogressdialog.h>

#include "SpaceChoiceDialog.hpp"

#include "TrackingDataDirectoryLoader.hpp"

#include "FixableErrorDialog.hpp"

UniverseEditorWidget::UniverseEditorWidget(QWidget *parent)
    : QWidget(parent)
    , d_ui(new Ui::UniverseEditorWidget) {
	d_ui->setupUi(this);

	d_ui->addButton->setEnabled(false);
	d_ui->deleteButton->setEnabled(false);

	d_ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents
	);
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

fmp::TrackingDataDirectory::Ptr
UniverseEditorWidget::openTDD(const QString &path) {

	QFutureWatcher<void> watcher;
	QEventLoop           loop;

	fmp::TrackingDataDirectory::Ptr res;
	fm::FixableErrorList            errors;
	connect(
	    &watcher,
	    &QFutureWatcher<void>::finished,
	    &loop,
	    &QEventLoop::quit
	);

	QProgressDialog	                  *dialog;
	fort::myrmidon::ProgressReporter::Ptr progress;

	std::tie(dialog, progress) = OpenItemProgressDialog(
	    tr("Loading %1 frame references").arg(path),
	    this
	);

	auto openDataDir = [&res, &errors, &path, &progress, dialog, this]() {
		try {
			std::tie(res, errors) = fmp::TrackingDataDirectory::Open(
			    path.toUtf8().constData(),
			    d_universe->basepath().toUtf8().constData(),
			    {.Progress = std::move(progress)}
			);

		} catch (const std::exception &e) {
			qCritical() << "Could not open TrackingDataDirectory" << path
			            << ": " << e.what();
		}
		dialog->reset();
	};
	watcher.setFuture(QtConcurrent::run(openDataDir));

	loop.exec();
	dialog->deleteLater();
	qWarning() << "done";
	if (errors.empty() == false) {
		if (FixableErrorDialog::promptForFix(path, std::move(errors), this) ==
		    false) {
			qWarning(
			) << "[UniverseEditorWidget]: Fixing errors discarded by user";
			return fmp::TrackingDataDirectory::Ptr();
		}
		res->SaveToCache();
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

void UniverseEditorWidget::addTrackingDataDirectory(const QString &filepath) {

	fmp::TrackingDataDirectory::Ptr tdd = openTDD(filepath);
	if (!tdd == true) {
		return;
	}

	try {
		TrackingDataDirectoryLoader::EnsureLoaded({tdd}, this);
	} catch (const std::exception &e) {
		return;
	}

	auto space = SpaceChoiceDialog::Get(d_universe, this);

	if (space.isEmpty()) {
		qDebug() << "[UniverseEditorWidget]: TDD addition aborded by user";
		return;
	}

	if (d_universe->spaceExists(space) == false) {
		d_universe->addSpace(space);
	}

	d_universe->addTrackingDataDirectoryToSpace(space, tdd);
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
