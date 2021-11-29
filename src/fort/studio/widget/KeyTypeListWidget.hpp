#pragma once

#include <QWidget>

namespace Ui {
class KeyTypeListWidget;
}

class AntKeyValueBridge;
class QComboBox;


class KeyTypeListWidget : public QWidget {
	Q_OBJECT
public:
	explicit KeyTypeListWidget(QWidget *parent = nullptr);
	virtual ~KeyTypeListWidget();

	void setup(AntKeyValueBridge * bridge);

	void buildTypeCombo(QComboBox * combo);

private slots:
	void on_addButton_clicked();
	void on_removeButton_clicked();

private:
	Ui::KeyTypeListWidget * d_ui;
	AntKeyValueBridge     * d_bridge;
};
