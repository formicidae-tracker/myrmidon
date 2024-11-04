#pragma once

#include <QWidget>
#include <QChart>
#include <QBarCategoryAxis>
#include <QValueAxis>

#include <fort/myrmidon/types/TagStatistics.hpp>

namespace fm = fort::myrmidon;


namespace Ui {
class TagStatisticsWidget;
}

class TagStatisticsWidget : public QWidget {
	Q_OBJECT
public:
	explicit TagStatisticsWidget(QWidget *parent = 0);
	virtual ~TagStatisticsWidget();

	void clear();
	void display(fm::TagID tagID, fm::TagStatistics stats, size_t frameCount);

private:
	Ui::TagStatisticsWidget *d_ui;

	QChart	       *d_chart;
	QBarCategoryAxis *d_xAxis;
	QValueAxis       *d_yAxis;
};
