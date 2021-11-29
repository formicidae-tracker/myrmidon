#pragma once

#include <QDialog>

namespace Ui {
class SetAntValueDialog;
}

class SetAntValueDialog : public QDialog {
	Q_OBJECT

public:
	explicit SetAntValueDialog(QWidget *parent = nullptr);
    virtual  ~SetAntValueDialog();

private:
    Ui::SetAntValueDialog * d_ui;
};
