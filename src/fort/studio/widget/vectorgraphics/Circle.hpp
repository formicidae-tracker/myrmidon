#pragma once


#include <QGraphicsEllipseItem>
#include "Shape.hpp"

#include <memory>

class Handle;

class Circle : public Shape, public QGraphicsEllipseItem {
	Q_OBJECT
public:
	Circle(
	    const QPointF &pos,
	    qreal          radius,
	    QColor         color,
	    QGraphicsItem *parent = nullptr
	);
	virtual ~Circle();

	QPointF pos() const;

	qreal radius() const;

	void setRadiusFromPos(const QPointF &pos);

protected:
	void addToSceneProtected(QGraphicsScene *scene) override;
	void removeFromSceneProtected(QGraphicsScene *scene) override;

	void paint(
	    QPainter	                   *painter,
	    const QStyleOptionGraphicsItem *option,
	    QWidget	                    *widget
	) override;

	void mousePressEvent(QGraphicsSceneMouseEvent *e) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *e) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *e) override;

private:
	void update(bool fixRadius);
	void moveUpdate(const QPointF &mewPos);

	Handle	              *d_center, *d_radiusHandle;
	qreal                    d_radius;
	std::shared_ptr<QPointF> d_moveEvent;
};

QDebug operator<<(QDebug, const Circle *);
