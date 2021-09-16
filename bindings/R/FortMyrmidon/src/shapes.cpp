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

fort::myrmidon::Circle fmpCircleCreate(const Eigen::Vector2d & center,
                                       double radius) {
	return fort::myrmidon::Circle(center,radius);
}

fort::myrmidon::Capsule fmpCapsuleCreate(const Eigen::Vector2d & c1,
                                         const Eigen::Vector2d & c2,
                                         double r1,
                                         double r2) {
	return fort::myrmidon::Capsule(c1,c2,r1,r2);
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

	class_<fort::myrmidon::Capsule>("fmCapsule")
		.derives<fort::myrmidon::Shape>("fmShape")
		.const_method("show",&fmCapsule_show)
		.property("c1",
		          &fort::myrmidon::Capsule::C1,
		          &fort::myrmidon::Capsule::SetC1,
		          "the first circle center")
		.property("c2",
		          &fort::myrmidon::Capsule::C2,
		          &fort::myrmidon::Capsule::SetC2,
		          "the second circle center")
		.property("r1",
		          &fort::myrmidon::Capsule::R1,
		          &fort::myrmidon::Capsule::SetR1,
		          "the first circle radius")
		.property("r2",
		          &fort::myrmidon::Capsule::R2,
		          &fort::myrmidon::Capsule::SetR2,
		          "the second circle radius")
		;

	class_<fort::myrmidon::Polygon>("fmPolygon")
		.derives<fort::myrmidon::Shape>("fmShape")
		.const_method("show",&fmPolygon_show)
		.property("vertices",
		          &fort::myrmidon::Polygon::Vertices,
		          &fort::myrmidon::Polygon::SetVertices,
		          "the polygon vertices")
		;

	function("pfmCircleCreate",&fmpCircleCreate);
	function("pfmCapsuleCreate",&fmpCapsuleCreate);
}

//' Creates a fmPolygon
//' @param vertices the polygon vertices
//' @return a closed \code{\link{fmPolygon}} with the given vertices
//' @family fmShape methods and classes
//[[Rcpp::export]]
fort::myrmidon::Polygon fmPolygonCreate(const fort::myrmidon::Vector2dList & vertices) {
	return fort::myrmidon::Polygon(vertices);
}
