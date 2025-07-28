#include "ProgressDialog.hpp"

#include "fort/myrmidon/types/Reporter.hpp"

#include <QDebug>
#include <QProgressDialog>

class ItemProgress : public fort::myrmidon::ProgressReporter {
public:
	ItemProgress(QProgressDialog *self)
	    : d_self(self) {}

	void AddTotal(size_t delta) override {
		d_total += delta;
		QMetaObject::invokeMethod(
		    d_self,
		    "setMaximum",
		    Qt::QueuedConnection,
		    Q_ARG(int, d_total)
		);
	}

	void Add(size_t value) override {
		d_value += value;
		QMetaObject::invokeMethod(
		    d_self,
		    "setValue",
		    Qt::QueuedConnection,
		    Q_ARG(int, d_value)
		);
	}

	void ReportError(const std::string &error) override {
		qWarning() << QString(error.c_str());
	}

private:
	QProgressDialog *d_self;
	size_t           d_total = 0;
	size_t           d_value = 0;
};

std::tuple<QProgressDialog *, std::unique_ptr<fort::myrmidon::ProgressReporter>>
OpenItemProgressDialog(const QString &title, QWidget *parent) {
	auto dialog = new QProgressDialog(
	    title,
	    QString(),
	    0,
	    -1,
	    parent,
	    Qt::Dialog | Qt::WindowTitleHint | Qt::FramelessWindowHint
									  );
	dialog->setAutoReset(false);
	dialog->setMinimumDuration(250);
	dialog->setValue(0);
	dialog->setWindowModality(Qt::ApplicationModal);

	return {dialog,std::make_unique<ItemProgress>(dialog)};
}
