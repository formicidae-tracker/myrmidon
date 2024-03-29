#include "VectorialScene.hpp"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include <QDebug>


#include "Handle.hpp"
#include "Vector.hpp"
#include "Capsule.hpp"
#include "Polygon.hpp"
#include "Circle.hpp"

#include <fort/myrmidon/Shapes.hpp>
#include <fort/studio/Utils.hpp>

#include <fort/studio/MyrmidonTypes/Conversion.hpp>

VectorialScene::VectorialScene(QObject * parent)
	: QGraphicsScene(parent)
	, d_once(true)
	, d_handleScaleFactor(1.0)
	, d_poseIndicator(nullptr)
	, d_background(nullptr)
	, d_staticPolygon(nullptr) {
	setBackgroundPicture("");
	d_color = Conversion::colorFromFM(fm::DefaultPaletteColor(0));

	d_editPressEH = [this](QGraphicsSceneMouseEvent *e) {
		                QGraphicsScene::mousePressEvent(e);
	                };

	d_editMoveEH = [this](QGraphicsSceneMouseEvent *e) {
		               QGraphicsScene::mouseMoveEvent(e);
	               };

	d_editReleaseEH = [this](QGraphicsSceneMouseEvent *e) {
		                  QGraphicsScene::mouseReleaseEvent(e);
	                  };

	d_insertVectorPressEH =
		[this](QGraphicsSceneMouseEvent *e) {
			if ( e->button() != Qt::LeftButton) {
				return;
			}
			auto pos = e->scenePos();
			auto vector = QSharedPointer<Vector>(new Vector(pos.x(),pos.y(),pos.x(),pos.y(),d_color));
			vector->addToScene(this);
			d_mouseMove =
				[vector] (QGraphicsSceneMouseEvent *e) {
					vector->setEndPos(e->scenePos());
				};
			d_mouseRelease =
				[vector,this](QGraphicsSceneMouseEvent * e) {
					if ( e->button() != Qt::LeftButton ) {
						return;
					}
					auto pos = e->scenePos();
					vector->setEndPos(pos);
					d_vectors.push_back(vector);
					if ( d_once == true ) {
						setMode(Mode::Edit);
					} else {
						this->d_mouseMove = this->d_editMoveEH;
						this->d_mouseRelease = this->d_editReleaseEH;
					}
					emit vectorCreated(vector);
				};
		};

	d_insertCapsulePressEH =
		[this] (QGraphicsSceneMouseEvent *e) {
			if ( e->button() != Qt::LeftButton) {
				return;
			}
			auto pos = e->scenePos();
			auto capsule = QSharedPointer<Capsule>(new Capsule(pos,pos,0,0,d_color));
			capsule->setC2AndRadiusFromPos(pos);
			capsule->addToScene(this);
			d_mouseMove =
				[capsule] (QGraphicsSceneMouseEvent *e) {
					capsule->setC2AndRadiusFromPos(e->scenePos());
				};
			d_mouseRelease =
				[capsule,this](QGraphicsSceneMouseEvent * e) {
					if ( e->button() != Qt::LeftButton ) {
						return;
					}
					auto pos = e->scenePos();
					capsule->setC2AndRadiusFromPos(pos);
					d_capsules.push_back(capsule);
					if ( d_once == true ) {
						setMode(Mode::Edit);
					} else {
						d_mouseMove = d_editMoveEH;
						d_mouseRelease = d_editReleaseEH;
					}
					emit capsuleCreated(capsule);
				};
		};

	d_insertCirclePressEH =
		[this] (QGraphicsSceneMouseEvent *e) {
			if ( e->button() != Qt::LeftButton) {
				return;
			}
			auto pos = e->scenePos();
			auto circle = QSharedPointer<Circle>(new Circle(pos,0,d_color));
			circle->setRadiusFromPos(pos);
			circle->addToScene(this);
			d_mouseMove =
				[circle] (QGraphicsSceneMouseEvent *e) {
					circle->setRadiusFromPos(e->scenePos());
				};
			d_mouseRelease =
				[circle,this](QGraphicsSceneMouseEvent * e) {
					if ( e->button() != Qt::LeftButton ) {
						return;
					}
					auto pos = e->scenePos();
					circle->setRadiusFromPos(pos);
					d_circles.push_back(circle);
					if ( d_once == true ) {
						setMode(Mode::Edit);
					} else {
						d_mouseMove = d_editMoveEH;
						d_mouseRelease = d_editReleaseEH;
					}
					emit circleCreated(circle);
				};
		};

	d_insertPolygonPressEH =
		[this] (QGraphicsSceneMouseEvent *e) {
			if ( e->button() != Qt::LeftButton ) {
				return;
			}
			auto start = e->scenePos();
			auto polygon = QSharedPointer<Polygon>(new Polygon({start},d_color));
			polygon->addToScene(this);
			d_mouseMove = [polygon](QGraphicsSceneMouseEvent * e){
				              e->ignore();
			              };
			d_mouseRelease = [](QGraphicsSceneMouseEvent * e){ e->ignore();};
			d_mousePress =
				[start,polygon,this](QGraphicsSceneMouseEvent *e) {
					auto newPos = e->scenePos();
					auto dist = (ToEigen(newPos) - ToEigen(start)).norm();

					if ( polygon->vertices().size() > 2
					     && (e->button() == Qt::RightButton
					         || ( e->button() == Qt::LeftButton && dist < Handle::SIZE) ) ) {
						polygon->close();
						d_polygons.push_back(polygon);
						if ( d_once == true ) {
							setMode(Mode::Edit);
						} else {
							d_mousePress = d_insertPolygonPressEH;
						}
						emit polygonCreated(polygon);
						return;
					}
					if ( e->button() != Qt::LeftButton ) {
						return;
					}

					auto p = polygon->appendPoint(e->scenePos());
					if ( p != nullptr ) {
						p->addToScene(this);
					}
				};
		};

	d_mode = Mode::Edit;
	d_mousePress = d_editPressEH;
	d_mouseMove = d_editMoveEH;
	d_mouseRelease = d_editReleaseEH;

}

VectorialScene::~VectorialScene() {
	for ( const auto & v : d_vectors ) {
		v->removeFromScene(this);
	}

	for ( const auto & c : d_capsules ) {
		c->removeFromScene(this);
	}

	for ( const auto & c : d_circles ) {
		c->removeFromScene(this);
	}

	for ( const auto & p : d_polygons ) {
		p->removeFromScene(this);
	}
}


const QColor VectorialScene::color() const {
	return d_color;
}

VectorialScene::Mode VectorialScene::mode() const {
	return d_mode;
}

bool VectorialScene::once() const {
	return d_once;
}

double VectorialScene::handleScaleFactor() const {
	return d_handleScaleFactor;
}

const QVector<QSharedPointer<Vector>> & VectorialScene::vectors() const {
	return d_vectors;
}

const QVector<QSharedPointer<Capsule>> & VectorialScene::capsules() const {
	return d_capsules;
}

const QVector<QSharedPointer<Polygon>> & VectorialScene::polygons() const {
	return d_polygons;
}

const QVector<QSharedPointer<Circle>> & VectorialScene::circles() const {
	return d_circles;
}



QSharedPointer<Circle> VectorialScene::appendCircle(const QPointF & center, qreal radius) {
	auto circle = QSharedPointer<Circle>(new Circle(center,radius,d_color));
	circle->addToScene(this);
	d_circles.push_back(circle);
	return circle;
}

QSharedPointer<Capsule> VectorialScene::appendCapsule(const QPointF & c1, const QPointF & c2,
                                                      qreal r1, qreal r2) {
	auto capsule = QSharedPointer<Capsule>(new Capsule(c1,c2,r1,r2,d_color));
	capsule->addToScene(this);
	d_capsules.push_back(capsule);
	return capsule;
}

QSharedPointer<Polygon> VectorialScene::appendPolygon(const QVector<QPointF> & vertices) {
	auto polygon = QSharedPointer<Polygon>(new Polygon(vertices,d_color));
	polygon->addToScene(this);
	d_polygons.push_back(polygon);
	return polygon;
}

QSharedPointer<Vector> VectorialScene::appendVector(const QPointF & start, const QPointF & end) {
	auto vector = QSharedPointer<Vector>(new Vector(start.x(),start.y(),end.x(),end.y(),d_color));
	vector->addToScene(this);
	d_vectors.push_back(vector);
	return vector;
}

void VectorialScene::clearCircles() {
	for ( const auto & c : d_circles ) {
		c->removeFromScene(this);
	}
	d_circles.clear();
}

void VectorialScene::clearCapsules() {
	for ( const auto & c : d_capsules ) {
		c->removeFromScene(this);
	}
	d_capsules.clear();

}

void VectorialScene::clearVectors() {
	for ( const auto & v : d_vectors ) {
		v->removeFromScene(this);
	}
	d_vectors.clear();
}

void VectorialScene::clearPolygons() {
	for ( const auto & p : d_polygons ) {
		p->removeFromScene(this);
	}
	d_polygons.clear();
}



void VectorialScene::setPoseIndicator(const QPointF & position, double angle) {
	if ( d_poseIndicator == nullptr ) {
		d_poseIndicator = new PoseIndicator;
		addItem(d_poseIndicator);
		d_poseIndicator->setZValue(-98);
		d_poseIndicator->setEnabled(false);
	}

	d_poseIndicator->setPos(position);
	d_poseIndicator->setRotation(angle*180.0/M_PI);
}

void VectorialScene::clearPoseIndicator() {
	if ( d_poseIndicator == nullptr ) {
		return;
	}
	removeItem(d_poseIndicator);
	delete d_poseIndicator;
	d_poseIndicator = nullptr;
}

class PictureCache {
public:

	QPixmap load(const QString & filepath) {
		auto fi = d_pixmap.find(filepath);
		if ( fi != d_pixmap.cend() ) {
			return fi->second;
		}

		auto pixmap = QPixmap(filepath);

		if ( d_pixmap.size() > 10 ) {
			d_pixmap.erase(d_fifo.front());
			d_fifo.pop_front();
		}

		d_pixmap[filepath] = pixmap;
		d_fifo.push_back(filepath);

		return pixmap;
	}
private:

	std::map<QString,QPixmap> d_pixmap;
	std::list<QString>        d_fifo;
};


void VectorialScene::setBackgroundPicture(const QString & filepath) {
	if ( d_background != nullptr) {
		removeItem(d_background);
		delete d_background;
		d_background = nullptr;
	}
	setBackgroundBrush(QColor(127,127,127));
	if ( filepath.isEmpty() ) {
		setSceneRect(QRectF(0,0,500,500));
		return;
	}
	static PictureCache cache;

	qInfo() << "setting " << filepath;
	d_background = new QGraphicsPixmapItem(cache.load(filepath));
	addItem(d_background);
	setSceneRect(d_background->boundingRect());
	d_background->setZValue(-100);
}


void VectorialScene::onZoomed(double factor) {
	setHandleScaleFactor(std::max(1.0/factor,1.0));
}

void VectorialScene::setOnce(bool once) {
	if ( once == d_once ) {
		return;
	}
	d_once = once;
	emit onceChanged(d_once);
}

void VectorialScene::setMode(Mode mode) {
	if ( mode == d_mode) {
		return;
	}
	d_mode = mode;
	switch(d_mode) {
	case Mode::Edit:
		d_mousePress = d_editPressEH;
		d_mouseMove = d_editMoveEH;
		d_mouseRelease = d_editReleaseEH;
		break;
	case Mode::InsertVector: {
		d_mousePress = d_insertVectorPressEH;
		d_mouseMove = d_editMoveEH;
		d_mouseRelease = d_editReleaseEH;
		break;
	}
	case Mode::InsertCapsule: {
		d_mousePress = d_insertCapsulePressEH;
		d_mouseMove = d_editMoveEH;
		d_mouseRelease = d_editReleaseEH;
		break;
	}
	case Mode::InsertCircle: {
		d_mousePress = d_insertCirclePressEH;
		d_mouseMove = d_editMoveEH;
		d_mouseRelease = d_editReleaseEH;
		break;
	}
	case Mode::InsertPolygon: {
		d_mousePress = d_insertPolygonPressEH;
		d_mouseMove = d_editMoveEH;
		d_mouseRelease = d_editReleaseEH;
		break;
	}
	default:
		qWarning() << "Unknown mode: " << mode;
	}
	emit modeChanged(d_mode);
}


void VectorialScene::setColor(const QColor & color) {
	if ( color == d_color || color.isValid() == false ){
		return;
	}
	d_color = color;
	emit colorChanged(d_color);
}


void VectorialScene::setHandleScaleFactor(double factor) {
	if ( factor == d_handleScaleFactor ) {
		return;
	}
	d_handleScaleFactor = factor;
	emit handleScaleFactorChanged(d_handleScaleFactor);
}

void VectorialScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	auto saved = d_mousePress;
	saved(mouseEvent);
}


void VectorialScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	auto saved = d_mouseMove;
	saved(mouseEvent);
}

void VectorialScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	auto saved = d_mouseRelease;
	saved(mouseEvent);
}

QDebug operator<<(QDebug debug, VectorialScene::Mode mode) {
	switch (mode) {
	case VectorialScene::Mode::Edit:
		debug << "VectorialScene::Mode::Edit";
		break;
	case VectorialScene::Mode::InsertVector:
		debug << "VectorialScene::Mode::InsertVector";
		break;
	case VectorialScene::Mode::InsertCircle:
		debug << "VectorialScene::Mode::InsertCircle";
		break;
	case VectorialScene::Mode::InsertPolygon:
		debug << "VectorialSxcene::Mode::InsertPolygon";
		break;
	case VectorialScene::Mode::InsertCapsule:
		debug << "VectorialScene::Mode::InsertCapsule";
		break;
	default:
		debug << "VectorialScene::Mode::" << int(mode);
	}
	return debug;
}


void VectorialScene::keyPressEvent(QKeyEvent * e) {
	if ( mode() != Mode::Edit ) {
		return;
	}
	switch ( e->key() ) {
	case Qt::Key_Backspace:
	case Qt::Key_Delete:
		e->accept();
		for ( const auto & i : selectedItems() ) {
			if ( auto s = dynamic_cast<Shape*>(i) ) {
				deleteShapePtr(s);
			}
		}
	default:
		e->ignore();
	}
}

void VectorialScene::deleteShapePtr(Shape * shape) {
	auto className = QString(shape->metaObject()->className());
#define implements_lookup_type(Name,name) do {	  \
		if ( className == #Name ) { \
			auto fi = std::find_if(d_ ## name ## s.begin(), \
			                       d_ ## name ## s.end(), \
			                       [shape](const QSharedPointer<Name> & o ) -> bool { \
				                       return o.data() == shape; \
			                       }); \
			if ( fi != d_ ## name ## s.end() ) { \
				auto o = *fi; \
				deleteShape(o); \
				emit name ## Removed(o); \
				return; \
			} \
		} \
	} while(0)

	implements_lookup_type(Vector,vector);
	implements_lookup_type(Capsule,capsule);
	implements_lookup_type(Circle,circle);
	implements_lookup_type(Polygon,polygon);

}

void VectorialScene::deleteShape(QSharedPointer<Shape> shape) {
	shape->removeFromScene(this);
	if ( auto v = shape.dynamicCast<Vector>() ) {
		d_vectors.removeOne(v);
	}
	if ( auto c = shape.dynamicCast<Circle>() ) {
		d_circles.removeOne(c);
	}
	if ( auto c = shape.dynamicCast<Capsule>() ) {
		d_capsules.removeOne(c);
	}
	if ( auto p = shape.dynamicCast<Polygon>() ) {
		d_polygons.removeOne(p);
	}

}


void VectorialScene::setStaticPolygon(const fm::Vector2dList & corners,
                                      const QColor & color) {
	if ( d_staticPolygon == nullptr ) {
		d_staticPolygon = new QGraphicsPolygonItem;
		addItem(d_staticPolygon);
		d_staticPolygon->setEnabled(false);
		d_staticPolygon->setZValue(-99);
	}
	QVector<QPointF> vertices;
	for ( const auto & c : corners ) {
		vertices.push_back(QPointF(c.x(),c.y()));
	}
	if ( corners.empty() == false ) {
		vertices.push_back(QPointF(corners[0].x(),corners[0].y()));
	}
	QColor actual(color);
	actual.setAlpha(Shape::BORDER_OPACITY);
	d_staticPolygon->setPen(QPen(actual,Shape::LINE_WIDTH));
	actual.setAlpha(Shape::FILL_OPACITY);
	d_staticPolygon->setBrush(actual);
	d_staticPolygon->setPolygon(vertices);
}

void VectorialScene::clearStaticPolygon() {
	if ( d_staticPolygon == nullptr ) {
		return;
	}
	removeItem(d_staticPolygon);
	delete d_staticPolygon;
	d_staticPolygon = nullptr;
}


VectorialScene::PoseIndicator::PoseIndicator(QGraphicsItem * parent )
	: QGraphicsItemGroup(parent) {

	const static qreal SIZE = 20.0;
	const static qreal LINE_WIDTH = 6.0;

	auto outerPathItem = new QGraphicsPathItem(this);
	auto innerPathItem = new QGraphicsPathItem(this);
	QPainterPath path;
	path.moveTo(0,0);
	path.lineTo(2*SIZE,0);
	path.lineTo(1.5*SIZE,SIZE/2.0);
	path.moveTo(1.5*SIZE,-SIZE/2.0);
	path.lineTo(2*SIZE,0);
	outerPathItem->setPen(QPen(QColor(0,0,0),LINE_WIDTH,Qt::SolidLine,Qt::RoundCap));
	outerPathItem->setPath(path);

	innerPathItem->setPen(QPen(QColor(255,255,255),LINE_WIDTH/2.0,Qt::SolidLine,Qt::RoundCap));
	innerPathItem->setPath(path);

	auto center = new QGraphicsEllipseItem(-LINE_WIDTH,-LINE_WIDTH,
	                                       2*LINE_WIDTH,2*LINE_WIDTH,
	                                       this);
	center->setPen(QPen(QColor(0,0,0),LINE_WIDTH/4.0));
	center->setBrush(QColor(255,255,255));

}

VectorialScene::PoseIndicator::~PoseIndicator() {
}
