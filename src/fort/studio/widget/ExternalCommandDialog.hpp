#pragma once

#include <QDialog>

namespace Ui {
class ExternalCommandDialog;
}

class ExternalCommandDialog : public QDialog {
	Q_OBJECT
public:
	explicit ExternalCommandDialog(
	    const QString &title,
	    const QString &body,
	    const QString &commands,
	    const QString &informativeText,
	    QWidget       *parent = nullptr
	);
	virtual ~ExternalCommandDialog() = default;

	static bool Prompt(
	    const QString &title,
	    const QString &body,
	    const QString &commands,
	    const QString &informativeText = ""
	);

private:
	Ui::ExternalCommandDialog *d_ui;
	QString                    d_commands;
};
