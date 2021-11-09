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
public slots:
	void display(TrackingVideoFrame image);

	void hideLoadingBanner(bool hide);

	void setZoomFocus(quint32 antID,qreal value);

	void setShowID(bool show);
	void setShowCollisions(bool show);

	void setOpacity(int opacity);

protected:
	void paintEvent(QPaintEvent * event) override;

	void paint(QPainter * painter);

	void paintAntsAndCollisions(QPainter * painter, const QRectF & focusRectangle);

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
	QPointF            d_lastFocus;
	int                d_opacity;
};
