#include "KeyTypeListWidget.hpp"
#include "ui_KeyTypeListWidget.h"

#include <fort/studio/bridge/AntKeyValueBridge.hpp>
#include <fort/studio/MyrmidonTypes/Value.hpp>

#include <QStyledItemDelegate>
#include <QDebug>


class KeyTypeDelegate : public QStyledItemDelegate {
public:
	KeyTypeDelegate(QWidget * parent, AntKeyValueBridge * bridge)
		: QStyledItemDelegate(parent)
		, d_bridge(bridge) {
	};

protected:

	QWidget * createEditor( QWidget *parent,
	                        const QStyleOptionViewItem &option,
	                        const QModelIndex &index ) const override {
		auto res = new QComboBox(parent);
		res->setModel(d_bridge->typeModel());
		return res;
	}

	void setEditorData ( QWidget *editor, const QModelIndex &index ) const override {
		auto text = index.data(Qt::DisplayRole).toString();
		auto combo = qobject_cast<QComboBox*>(editor);
		if ( combo == nullptr || text.isEmpty() ) {
			return;
		}

		combo->setCurrentIndex(combo->findText(text));
	}

	void setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const override {
		auto combo = qobject_cast<QComboBox*>(editor);
		if ( combo == nullptr ) {
			return;
		}
		auto type = fm::ValueType(combo->currentData(AntKeyValueBridge::KeyTypeRole).toInt());
		d_bridge->setKey(index.siblingAtColumn(0).data(Qt::DisplayRole).toString(),
		                 fm::ValueUtils::Default(type));
	}

private :
	AntKeyValueBridge * d_bridge;
};

KeyTypeListWidget::KeyTypeListWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::KeyTypeListWidget)
	, d_bridge(nullptr) {
	d_ui->setupUi(this);
	d_ui->addButton->setEnabled(false);
	d_ui->removeButton->setEnabled(false);
	d_ui->comboBox->setEnabled(false);
}

void KeyTypeListWidget::setup(AntKeyValueBridge * metadata) {
	d_bridge = metadata;
	d_ui->tableView->setModel(d_bridge->keyModel());
	connect(d_bridge,
	        &AntKeyValueBridge::activated,
	        d_ui->addButton,
	        &QToolButton::setEnabled);

	connect(d_bridge,
	        &AntKeyValueBridge::activated,
	        d_ui->comboBox,
	        &QComboBox::setEnabled);
	auto sModel = d_ui->tableView->selectionModel();

	connect(sModel,
	        &QItemSelectionModel::selectionChanged,
	        [this]() {
		        d_ui->removeButton->setEnabled(d_ui->tableView->selectionModel()->selectedRows().size() == 1);
	        });

	d_ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	d_ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

	auto hHeader = d_ui->tableView->horizontalHeader();
	hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
	d_ui->tableView->setItemDelegateForColumn(1,new KeyTypeDelegate(this,d_bridge));
	buildTypeCombo(d_ui->comboBox);
}

KeyTypeListWidget::~KeyTypeListWidget() {
	delete d_ui;
}

void KeyTypeListWidget::buildTypeCombo(QComboBox *  combo) {
	combo->setModel(d_bridge->typeModel());
}


void KeyTypeListWidget::on_addButton_clicked() {
	auto newName = tr("key-%1").arg(d_ui->tableView->model()->rowCount()+1);
	auto type = fm::ValueType(d_ui->comboBox->currentData(AntKeyValueBridge::KeyTypeRole).toInt());
	d_bridge->setKey(newName,fm::ValueUtils::Default(type));
	d_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


void KeyTypeListWidget::on_removeButton_clicked() {
	auto rows = d_ui->tableView->selectionModel()->selectedRows();
	if ( rows.size() != 1 ) {
		return;
	}

	d_bridge->removeKey(rows[0].data(Qt::DisplayRole).toString());
}
