#pragma once

#include <QAbstractItemModel>
#include <QDateTime>
#include <QWidget>

#include <memory>
#include <variant>

#include <slog++/Attribute.hpp>
#include <slog++/Sink.hpp>

class QLabel;
class LoggerWidget;

Q_DECLARE_METATYPE(std::shared_ptr<const slog::Record>);

class Logger : public QAbstractItemModel, public slog::Sink {
	Q_OBJECT
	Q_PROPERTY(int warningCount READ warningCount NOTIFY warningCountChanged)
	Q_PROPERTY(int errorCount READ errorCount NOTIFY errorCountChanged)

public:
	explicit Logger(QObject *parent = nullptr);
	virtual ~Logger() = default;

	bool AllocateOnStack() const noexcept override;
	bool Enabled(slog::Level level) const noexcept override;
	void From(slog::Level level) noexcept override;
	void Set(slog::Level level, bool enabled) noexcept override;
	void Log(RecordVariant &&record) override;

	int warningCount() const;
	int errorCount() const;

	QModelIndex
	index(int row, int column, const QModelIndex &parent) const override;
	QModelIndex parent(const QModelIndex &child) const override;
	int         rowCount(const QModelIndex &parent) const override;
	int         columnCount(const QModelIndex &parent) const override;
	QVariant    data(const QModelIndex &index, int role) const override;

	QVariant headerData(
	    int section, Qt::Orientation orientation, int role = Qt::DisplayRole
	) const override;

signals:
	void warningCountChanged(int);
	void errorCountChanged(int);

	void newRecord(const std::shared_ptr<const slog::Record> &record);

	void newMessage(int type, const QString &message);
public slots:
	void appendRecord(const std::shared_ptr<const slog::Record> &record);

private:
	using DataPtr = std::variant<const slog::Record *, const slog::Attribute *>;

	struct NodeRef {
		DataPtr              self;
		int                  selfIndex;
		NodeRef             *parent = nullptr;
		std::vector<NodeRef> children;

		inline void addChildren(DataPtr child) {
			auto selfIndex = int(children.size());
			children.emplace_back(
			    NodeRef{.self = child, .selfIndex = selfIndex, .parent = this}
			);
		}

		template <typename Iter> inline void visitAttribute(Iter b, Iter e) {
			for (Iter it = b; it != e; ++it) {
				if (std::holds_alternative<std::monostate>(it->value)) {
					continue;
				}
				addChildren(it);
				if (std::holds_alternative<slog::GroupPtr>(it->value)) {
					auto g = std::get<slog::GroupPtr>(it->value);
					children.back().visitAttribute(
					    g->attributes.begin(),
					    g->attributes.end()
					);
				}
			}
		}
	};

	QVariant recordDisplayRole(const NodeRef *n, int column) const;
	QVariant attributeDisplayRole(const NodeRef *n, int column) const;
	QVariant foreground(const NodeRef *n) const;

	std::vector<std::shared_ptr<const slog::Record>> d_records;
	std::vector<NodeRef>                             d_tree;

	int d_warningCounts{0}, d_errorCounts{0};
};

namespace Ui {
class LoggerWidget;
}

class LoggerWidget : public QWidget {
	Q_OBJECT
public:
	explicit LoggerWidget(Logger *logger, QWidget *parent = 0);
	virtual ~LoggerWidget();

private:
	Ui::LoggerWidget *d_ui;
	Logger           *d_logger;
};

class LogStatusWidget : public QWidget {
	Q_OBJECT
public:
	explicit LogStatusWidget(Logger *logger, QWidget *parent = nullptr);
	virtual ~LogStatusWidget();

protected:
	void mouseDoubleClickEvent(QMouseEvent *event) override;

protected slots:
	void onNewMessage(int type, const QString &message);

signals:
	void showLog();

private:
	QLabel *d_warningLabel, *d_warningIcon, *d_errorLabel, *d_errorIcon,
	    *d_message;
};
