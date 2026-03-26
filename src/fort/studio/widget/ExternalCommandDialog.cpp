#include "ExternalCommandDialog.hpp"
#include "ui_ExternalCommandDialog.h"

#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QEventLoop>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QStyle>

#include <qdialogbuttonbox.h>
#include <slog++/slog++.hpp>

#include <fort/studio/Slogpp.hpp>

ExternalCommandDialog::ExternalCommandDialog(
    const QString &title,
    const QString &body,
    const QString &commands,
    const QString &informativeText,
    QWidget       *parent
)
    : QDialog(parent)
    , d_ui{new Ui::ExternalCommandDialog{}}
    , d_commands{commands} {
	d_ui->setupUi(this);

	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint);
	setWindowModality(Qt::ApplicationModal);
	setFixedWidth(600);

	auto style     = QApplication::style();
	auto iconWidth = style->pixelMetric(QStyle::PM_MessageBoxIconSize);
	auto iconSize  = QSize{iconWidth, iconWidth};
	auto icon      = style->standardIcon(QStyle::SP_MessageBoxCritical);
	d_ui->icon->setFixedSize(iconSize);
	d_ui->icon->setPixmap(icon.pixmap(iconSize));

	d_ui->text->setText(body);
	d_ui->informativeText->setText(informativeText);
	d_ui->command->setPlainText(commands);
	auto monospaceFont = d_ui->command->font();
	monospaceFont.setFamily("monospace");
	d_ui->command->setFont(monospaceFont);

	auto *quitButton = new QPushButton{
	    QIcon::fromTheme("application-exit-symbolic"),
	    tr("Quit"),
	    this
	};

	auto *acceptButton = new QPushButton{
	    style->standardIcon(QStyle::SP_DialogDiscardButton),
	    tr("Ignore"),
	    this
	};

	auto *copyButton = new QPushButton{
	    QIcon::fromTheme("edit-copy-symbolic"),
	    tr("Copy to clipboard"),
	    this,
	};
	connect(quitButton, &QAbstractButton::clicked, this, &QDialog::reject);
	connect(acceptButton, &QAbstractButton::clicked, this, &QDialog::accept);
	connect(copyButton, &QAbstractButton::clicked, this, [commands]() {
		QApplication::clipboard()->setText(commands);
	});
	d_ui->buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);
	d_ui->buttonBox->addButton(acceptButton, QDialogButtonBox::AcceptRole);
	d_ui->buttonBox->addButton(copyButton, QDialogButtonBox::ResetRole);

	adjustSize();
	slog::Info(
	    "computed size",
	    slog::QSize("hint", sizeHint()),
	    slog::QSize("size", size())
	);
	setFixedSize(size());
}

bool ExternalCommandDialog::Prompt(
    const QString &title,
    const QString &body,
    const QString &commands,
    const QString &informativeText
) {

	ExternalCommandDialog dialog{title, body, commands, informativeText};
	QEventLoop            loop;
	connect(&dialog, &QDialog::finished, &loop, &QEventLoop::quit);
	dialog.open();

	loop.exec();
	return dialog.result() == QDialog::Accepted;
}
