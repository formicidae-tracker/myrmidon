#pragma once

#include <QWidget>

#include "TrackingVideoFrame.hpp"

class AntDisplayBridge;

class TrackingVideoWidget : public QWidget {
	Q_OBJECT
	Q_PROPERTY(bool showID
	           READ showID
	           WRITE setShowID
	           NOTIFY showIDChanged);
	Q_PROPERTY(bool showCollisions
	           READ showCollisions
	           WRITE setShowCollisions
	           NOTIFY showCollisionsChanged);
	Q_PROPERTY(int opacity
	           READ opacity
	           WRITE setOpacity);

public:
	explicit TrackingVideoWidget(QWidget * parent = nullptr);
	virtual ~TrackingVideoWidget();


	void setup(AntDisplayBridge *antDisplay);

	bool showID() const;

	bool showCollisions() const;

	bool showLoadingBanner() const;

	fort::Time trackingTime() const;

	bool hasTrackingTime() const;

	int opacity() const;
signals:
	void showIDChanged(bool value);
	void showCollisionsChanged(bool value);

	void hasTrackingTimeChanged(bool value);

	void togglePlayPause();

	void trackingTimeChanged(fort::Time time);
public slots:
	void display(TrackingVideoFrame image);

	void hideLoadingBanner(bool hide);

	void setZoom(qreal value);

	void setROI(quint32 ROI);

	void setFocus(quint32 antID);


	void setShowID(bool show);
	void setShowCollisions(bool show);

	void setOpacity(int opacity);

protected:
	void paintEvent(QPaintEvent * event) override;

	void paint(QPainter * painter);

	void paintAntsAndCollisions(QPainter * painter, const QRectF & focusRectangle);

	void paintROI(QPainter * painter);

	void paintAnts(QPainter * painter, double ratio, const QRectF & focusRectangle, bool hasSolo);
	void paintCollisions(QPainter * painter, double ratio, const QRectF & focusRectangle, bool hasSolo);

	void mousePressEvent(QMouseEvent * event) override;
	void mouseDoubleClickEvent(QMouseEvent * event) override;
private:
	friend class TrackingVideoWidgetUTest;
	void focusAnt(quint32 antID, bool reset = false);

	void setHasTrackingTime(bool value);

	TrackingVideoFrame d_frame;
	AntDisplayBridge * d_antDisplay;
	bool               d_hideLoadingBanner;
	bool               d_showID;
	bool               d_showCollisions;
	bool               d_hasTrackingTime;
	quint32            d_focusedAntID;
	qreal              d_zoom;
	quint32            d_roi;
	QPointF            d_lastFocus;
	int                d_opacity;
};
