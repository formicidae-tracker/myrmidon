#pragma once

#include <QDialog>

#include <fort/studio/MyrmidonTypes/Time.hpp>
#include <fort/studio/MyrmidonTypes/AntMetadata.hpp>

class ExperimentBridge;

namespace Ui {
class SetAntValueDialog;
}


class SetAntValueDialog : public QDialog {
	Q_OBJECT
	Q_PROPERTY(fort::Time inTime
	           READ inTime
	           WRITE setInTime
	           NOTIFY inTimeChanged)
	Q_PROPERTY(fort::Time outTime
	           READ outTime
	           WRITE setOutTime
	           NOTIFY outTimeChanged)
public:
	explicit SetAntValueDialog(QWidget *parent = nullptr);
    virtual  ~SetAntValueDialog();


	void initialize(ExperimentBridge * experiment);

	const fort::Time & inTime() const;
	const fort::Time & outTime() const;

	const fm::AntStaticValue & value() const;
	bool hasValue() const;

	QString key() const;

public slots:
	void setInTime(const fort::Time & v);
	void setOutTime(const fort::Time & v);

private slots:
	void updateState();
	void on_keyComboBox_currentIndexChanged(int);
	void on_valueEdit_textChanged(const QString & text);


signals:
	void inTimeChanged(const fort::Time &);
	void outTimeChanged(const fort::Time &);
	void hasValueChanged(bool valid);
protected:
	void setHasValue(bool valid);

	void showEvent(QShowEvent * event) override;

    Ui::SetAntValueDialog * d_ui;
	fort::Time              d_inTime,d_outTime;
	fm::AntStaticValue      d_value;
	bool                    d_hasValue;
};
