#pragma once

#include <QDialog>

#include <fort/studio/MyrmidonTypes/FixableError.hpp>

namespace Ui {
class FixableErrorDialog;
}

class FixableErrorDialog : public QDialog {
	Q_OBJECT

public:
	explicit FixableErrorDialog(fm::FixableErrorList errors,
	                            const QString & context,
	                            QWidget *parent = nullptr);
	~FixableErrorDialog();

public slots:
	void fixSelected();

private slots:
	void on_tableWidget_cellClicked(int row,int column);
	void on_checkBox_stateChanged();

private:
	void buildTable();



	Ui::FixableErrorDialog * d_ui;
	fm::FixableErrorList d_errors;
};
