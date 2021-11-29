#pragma once

#include "Workspace.hpp"

namespace Ui {
class AntMetadataWorkspace;
}

class AntKeyValueBridge;

class AntMetadataWorkspace : public Workspace {
	Q_OBJECT
public:
	explicit AntMetadataWorkspace(QWidget *parent = 0);
	virtual ~AntMetadataWorkspace();


protected:
	void initialize(QMainWindow * main,ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions) override;
	void tearDown(const NavigationAction & actions) override;


private slots:
	void onAddButtonClicked();
	void onRemoveButtonClicked();


private:
	void initialize(AntKeyValueBridge * metadata);


	Ui::AntMetadataWorkspace * d_ui;
	AntKeyValueBridge        * d_keyValues;
};
