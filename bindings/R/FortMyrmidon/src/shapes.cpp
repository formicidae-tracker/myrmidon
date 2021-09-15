#include "shapes.h"

#include "Rcpp.h"

void fmShape_show(const fort::myrmidon::Shape * s) {
	switch(s->ShapeType()) {
	case fort::myrmidon::Shape::Type::CIRCLE:
		fmCircle_show(dynamic_cast<const fort::myrmidon::Circle*>(s));
		return;
	case fort::myrmidon::Shape::Type::CAPSULE:
		fmCapsule_show(dynamic_cast<const fort::myrmidon::Capsule*>(s));
		return;
	case fort::myrmidon::Shape::Type::POLYGON:
		fmPolygon_show(dynamic_cast<const fort::myrmidon::Polygon*>(s));
		return;
	}
}

void fmCircle_show(const fort::myrmidon::Circle * c) {
	Rcpp::Rcout << "fmCircle($center = "
	            << c->Center().transpose()
	            << ", $radius = "
	            << c->Radius()
	            << ")\n";
}

void fmCapsule_show(const fort::myrmidon::Capsule * c) {
	Rcpp::Rcout << "fmCapsule($c1 = "
	            << c->C1().transpose()
	            << ", $c2 = "
	            << c->C2().transpose()
	            << ", $r1 = "
	            << c->R1()
	            << ", $r2 = "
	            << c->R2()
	            << ")\n";
}

void fmPolygon_show(const fort::myrmidon::Polygon * p) {
	Rcpp::Rcout << "fmPolygon(";
	for ( size_t i = 0 ; i < p->Size(); ++i ) {
		Rcpp::Rcout <<" (" << p->Vertex(i).transpose() << ")";
	}
	Rcpp::Rcout << ")\n";
}


fort::myrmidon::Circle fmpCircleCreate(const Eigen::Vector2d & center = {0,0},
                                      double radius = 1.0) {
	return fort::myrmidon::Circle(center,radius);
}


RCPP_MODULE(shapes) {
	using namespace Rcpp;
	class_<fort::myrmidon::Shape>("fmShape")
		.property("shapeType",&fort::myrmidon::Shape::ShapeType)
		.const_method("show",&fmShape_show)
		;

	class_<fort::myrmidon::Circle>("fmCircle")
		.derives<fort::myrmidon::Shape>("fmShape")
		.const_method("show",&fmCircle_show)
		.property("center",
		          &fort::myrmidon::Circle::Center,
		          &fort::myrmidon::Circle::SetCenter,
		          "the circle center")
		.property("radius",
		          &fort::myrmidon::Circle::Radius,
		          &fort::myrmidon::Circle::SetRadius,
		          "the circle radius")
		;

	function("fmpCircleCreate",&fmpCircleCreate);
}
