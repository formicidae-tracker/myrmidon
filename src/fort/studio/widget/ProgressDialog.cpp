#include "ProgressDialog.hpp"

#include "fort/myrmidon/types/Reporter.hpp"

#include <QDebug>

ItemProgressDialog::ItemProgressDialog(const QString &title, QWidget *parent)
    : QProgressDialog(
          title, QString(), 0, -1, this, Qt::Dialog | Qt::WindowTitleHint
      ) {
	setAutoReset(false);
	setMinimumDuration(250);
	setValue(0);
	setWindowModality(Qt::ApplicationModal);
}

class ItemProgress : public fort::myrmidon::ProgressReporter {
public:
	ItemProgress(ItemProgressDialog *self)
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
	ItemProgressDialog *d_self;
	size_t              d_total = 0;
	size_t              d_value = 0;
};

std::unique_ptr<fort::myrmidon::ProgressReporter>
ItemProgressDialog::GetProgressReporter() {
	return std::make_unique<ItemProgress>(this);
}
