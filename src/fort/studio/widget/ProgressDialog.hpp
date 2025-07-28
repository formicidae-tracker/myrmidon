#pragma once

#include <QProgressDialog>

namespace fort {
namespace myrmidon {
class ProgressReporter;
}
} // namespace fort

class ItemProgressDialog : public QProgressDialog {
	Q_OBJECT
public:
	explicit ItemProgressDialog(const QString &title, QWidget *parent);

	virtual ~ItemProgressDialog() = default;

	friend class ItemProgress;
	std::unique_ptr<fort::myrmidon::ProgressReporter> GetProgressReporter();
};
