#include "Logger.hpp"
#include "ui_LoggerWidget.h"

#include <QDateTime>
#include <QDebug>
#include <QFontDatabase>

#include <memory>
#include <qheaderview.h>
#include <qnamespace.h>
#include <type_traits>
#include <variant>

#include <cpptrace/exceptions.hpp>

#include <slog++/FormattersImpl.hpp>
#include <slog++/Level.hpp>
#include <slog++/Record.hpp>
#include <slog++/Types.hpp>

Logger::Logger(QObject *parent)
    : QAbstractItemModel(parent) {
	qRegisterMetaType<std::shared_ptr<const slog::Record>>();
	connect(
	    this,
	    &Logger::newRecord,
	    this,
	    &Logger::appendRecord,
	    Qt::QueuedConnection
	);
}

bool Logger::AllocateOnStack() const noexcept {
	return false;
}

bool Logger::Enabled(slog::Level level) const noexcept {
#ifndef NDEBUG
	return level >= slog::Level::Info;
#else
	return true;
#endif
}

void Logger::From(slog::Level level) noexcept {}

void Logger::Set(slog::Level level, bool enabled) noexcept {}

void Logger::Log(RecordVariant &&record) {
	std::shared_ptr<const slog::Record> copied = std::visit(
	    [](auto &&r) -> std::shared_ptr<const slog::Record> {
		    using T         = std::decay_t<decltype(r)>;
		    using SharedPtr = std::shared_ptr<const slog::Record>;
		    using UniquePtr = std::unique_ptr<const slog::Record>;
		    if constexpr (std::is_same_v<T, SharedPtr>) {
			    return r;
		    } else if constexpr (std::is_same_v<T, UniquePtr>) {
			    return SharedPtr{r.release()};
		    }
		    return nullptr;
	    },
	    std::move(record)
	);

	emit newRecord(copied);
}

int Logger::warningCount() const {
	return d_warningCounts;
}

int Logger::errorCount() const {
	return d_errorCounts;
}

void Logger::appendRecord(const std::shared_ptr<const slog::Record> &record) {
	if (record == nullptr) {
		return;
	}
	beginInsertRows(QModelIndex{}, d_records.size(), d_records.size());
	auto selfIndex = int(d_records.size());
	d_records.push_back(record);
	d_tree.emplace_back(
	    new NodeRef{.self = record.get(), .selfIndex = selfIndex}
	);
	d_tree.back()->visitAttribute(
	    record->attributes.begin(),
	    record->attributes.end()
	);
	endInsertRows();
	emit newMessage(int(record->level), record->message.c_str());
}

QModelIndex
Logger::index(int row, int column, const QModelIndex &parent) const {
	if (row < 0 || column < 0 || column >= 3) {
		return {};
	}
	if (parent.isValid() == false) {
		if (row >= int(d_tree.size())) {
			return {};
		}

		return createIndex(row, column, d_tree[row].get());
	}

	auto parentData = reinterpret_cast<NodeRef *>(parent.internalPointer());
	if (parentData == nullptr) {
		return {};
	}

	if (row >= int(parentData->children.size())) {
		return {};
	}
	return createIndex(row, column, &parentData->children[row]);
}

QModelIndex Logger::parent(const QModelIndex &child) const {
	if (child.isValid() == false) {
		return {};
	}
	auto childData = reinterpret_cast<NodeRef *>(child.internalPointer());
	if (childData == nullptr || childData->parent == nullptr) {
		return {};
	}
	return createIndex(childData->parent->selfIndex, 0, childData->parent);
}

int Logger::rowCount(const QModelIndex &parent) const {
	if (parent.isValid() == false) {
		return int(d_tree.size());
	}
	auto parentData = reinterpret_cast<NodeRef *>(parent.internalPointer());
	if (parentData == nullptr) {
		return 0;
	}
	return int(parentData->children.size());
}

int Logger::columnCount(const QModelIndex &parent) const {
	return 3;
}

QVariant Logger::data(const QModelIndex &index, int role) const {
	if (index.isValid() == false) {
		return {};
	}

	auto data = reinterpret_cast<NodeRef *>(index.internalPointer());
	if (data == nullptr) {
		return {};
	}
	if (role == Qt::ForegroundRole) {
		return foreground(data);
	}

	if (data->parent == nullptr) {
		switch (role) {
		case Qt::DisplayRole:
			return recordDisplayRole(data, index.column());
		default:
			return {};
		}
	}
	switch (role) {
	case Qt::DisplayRole:
		return attributeDisplayRole(data, index.column());
	default:
		return {};
	}
}

QVariant Logger::recordDisplayRole(const NodeRef *n, int column) const {
	const auto rec = std::get<const slog::Record *>(n->self);
	switch (column) {
	case 0: {
		std::string res;
		slog::details::FormatTo(rec->timestamp, res);
		return QString(res.c_str());
	}
	case 1:
		return QString{slog::details::levelName(rec->level).c_str()};
	case 2:
		return QString{rec->message.c_str()};
	default:
		return {};
	}
}

QVariant Logger::attributeDisplayRole(const NodeRef *n, int column) const {
	const auto attribute = std::get<const slog::Attribute *>(n->self);
	switch (column) {
	case 0:
		return {};
	case 1:
		return QString{attribute->key.c_str()};
	case 2:
		return std::visit(
		    [](const auto &arg) -> QString {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, std::monostate> ||
			                  std::is_same_v<T, slog::GroupPtr>) {
				    return {};
			    } else if constexpr (std::is_same_v<T, bool>) {
				    return arg ? tr("true") : tr("false");
			    } else if constexpr (std::is_same_v<T, slog::StringType>) {
				    return arg.c_str();
			    } else {
				    std::string res;
				    slog::details::FormatTo(arg, res);
				    return res.c_str();
			    }
		    },
		    attribute->value
		);
	default:
		return {};
	}
}

QVariant Logger::foreground(const NodeRef *n) const {
	while (n->parent != nullptr) {
		return foreground(n->parent);
	}
	auto            rec = std::get<const slog::Record *>(n->self);
	static QVariant colors[slog::NumLevels - 1] = {
	    {},
	    {},
	    {},
	    {}, // TRACE
	    QBrush{Qt::darkBlue},
	    QBrush{Qt::darkBlue},
	    QBrush{Qt::darkBlue},
	    QBrush{Qt::darkBlue}, // DEBUG
	    QBrush{Qt::darkCyan},
	    QBrush{Qt::darkCyan},
	    QBrush{Qt::darkCyan},
	    QBrush{Qt::darkCyan}, // INFO
	    QBrush{Qt::darkYellow},
	    QBrush{Qt::darkYellow},
	    QBrush{Qt::darkYellow},
	    QBrush{Qt::darkYellow}, // WARNING
	    QBrush{Qt::darkRed},
	    QBrush{Qt::darkRed},
	    QBrush{Qt::darkRed},
	    QBrush{Qt::darkRed}, // ERROR
	    QBrush{Qt::darkRed}, // FATAL
	};
	auto idx = size_t(rec->level);
	if (idx >= slog::NumLevels - 1) {
		return {};
	}
	return colors[idx];
}

QString formatTimeT(const slog::TimeT &time) {
	std::string buffer;
	slog::details::FormatTo(time, buffer);
	return buffer.c_str();
}

QVariant
Logger::headerData(int section, Qt::Orientation orientation, int role) const {

	if (orientation != Qt::Horizontal) {
		return {};
	}

	if (role != Qt::DisplayRole) {
		return {};
	}
	switch (section) {
	case 0:
		return tr("Timestamp");
	case 1:
		return tr("Level/Name");
	case 2:
		return tr("Message/Value");
	default:
		return {};
	}
}

LoggerWidget::LoggerWidget(Logger *logger, QWidget *parent)
    : QWidget(parent)
    , d_ui(new Ui::LoggerWidget)
    , d_logger(logger) {

	d_ui->setupUi(this);

	d_ui->treeView->setModel(d_logger);
	auto model = d_ui->treeView->model();
	for (int r = 0; r < model->rowCount({}); ++r) {
		expandAllFiltered(model->index(r, 0, {}));
	}

	connect(
	    d_ui->treeView->model(),
	    &QAbstractItemModel::rowsInserted,
	    this,
	    &LoggerWidget::onRowInserted
	);

	d_ui->treeView->setHeaderHidden(false);
	auto *h = d_ui->treeView->header();
	h->setVisible(true);
	h->setStretchLastSection(true);
	h->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	h->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	h->setSectionResizeMode(2, QHeaderView::Stretch);

	d_ui->comboBox->insertItem(0, "Trace");
	d_ui->comboBox->insertItem(1, "Debug");
	d_ui->comboBox->insertItem(2, "Info");
	d_ui->comboBox->insertItem(3, "Warning");
	d_ui->comboBox->insertItem(4, "Error");

	d_ui->comboBox->setCurrentIndex(2);

	auto logDir = std::string{"tmp"};
	setWindowTitle(
	    tr("FORT Studio Log(%1/fort-studio.INFO)").arg(logDir.c_str())
	);

	const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	d_ui->treeView->setFont(fixedFont);
}

LoggerWidget::~LoggerWidget() {
	delete d_ui;
}

void LoggerWidget::expandAllFiltered(const QModelIndex &index) {
	if (index.isValid() == false) {
		return;
	}
	auto view  = d_ui->treeView;
	auto model = view->model();
	if (index.parent().isValid() == false ||
	    index.parent().parent().isValid() == true ||
	    index.siblingAtColumn(0).data(Qt::DisplayRole).toString() !=
	        "stacktrace") {
		view->setExpanded(index, true);
	}

	d_ui->treeView->setExpanded(index, true);
	for (int r = 0; r < model->rowCount(index); ++r) {
		auto child = model->index(r, 0, index);
		expandAllFiltered(child);
	}
}

void LoggerWidget::onRowInserted(
    const QModelIndex &parent, int first, int last
) {
	for (int r = first; r <= last; ++r) {
		QModelIndex idx = d_ui->treeView->model()->index(r, 0, parent);
		expandAllFiltered(idx);
	}
}

LogStatusWidget::LogStatusWidget(Logger *logger, QWidget *parent)
    : QWidget(parent) {
	d_errorIcon    = new QLabel(this);
	d_errorLabel   = new QLabel(this);
	d_warningIcon  = new QLabel(this);
	d_warningLabel = new QLabel(this);
	d_message      = new QLabel(this);

	auto layout = new QHBoxLayout(this);
	layout->addWidget(d_errorIcon);
	layout->addWidget(d_errorLabel);
	layout->addWidget(d_warningIcon);
	layout->addWidget(d_warningLabel);
	layout->addWidget(d_message);
	setLayout(layout);

	setToolTip(tr("Double-click to show log"));
	setStatusTip(tr("Double-click to show log"));
	connect(
	    logger,
	    &Logger::warningCountChanged,
	    this,
	    [this](int warnings) {
		    d_warningLabel->setText(tr(": %1").arg(warnings));
		    d_warningIcon->setEnabled(warnings > 0);
	    },
	    Qt::QueuedConnection
	);

	connect(
	    logger,
	    &Logger::errorCountChanged,
	    this,
	    [this](int errors) {
		    d_errorLabel->setText(tr(": %1").arg(errors));
		    d_errorIcon->setEnabled(errors > 0);
	    },
	    Qt::QueuedConnection
	);

	connect(
	    logger,
	    &Logger::newMessage,
	    this,
	    &LogStatusWidget::onNewMessage,
	    Qt::QueuedConnection
	);

	d_errorIcon->setPixmap(
	    QIcon::fromTheme("dialog-error-symbolic").pixmap(15, 15)
	);
	d_warningIcon->setPixmap(
	    QIcon::fromTheme("dialog-warning-symbolic").pixmap(15, 15)
	);

	const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

	setFont(fixedFont);
	d_message->setFont(fixedFont);

	d_warningLabel->setText(": 0");
	d_errorLabel->setText(": 0");
	d_warningIcon->setEnabled(false);
	d_errorIcon->setEnabled(false);
	onNewMessage(QtWarningMsg, "");
}

LogStatusWidget::~LogStatusWidget() {}

void LogStatusWidget::onNewMessage(int type, const QString &message) {
	const static int SIZE          = 60;
	auto             actualMessage = message;
	if (actualMessage.size() < SIZE) {
		actualMessage.resize(SIZE, ' ');
	} else if (actualMessage.size() > SIZE) {
		actualMessage.resize(SIZE - 3);
		actualMessage += "...";
	}

	switch (slog::Level(type)) {
	case slog::Level::Info:
	case slog::Level::Warn:
		d_message->setText(actualMessage);
		d_message->setStyleSheet("QLabel {}");
		break;
	case slog::Level::Error:
		d_message->setText(actualMessage);
		d_message->setStyleSheet("QLabel {color : red;}");
		QApplication::beep();
		break;
	default:
		break;
	}
}

void LogStatusWidget::mouseDoubleClickEvent(QMouseEvent * event) {
	emit showLog();
}
