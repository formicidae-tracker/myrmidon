#include "shapes.h"

#include "Rcpp.h"

Eigen::IOFormat fmFormat(Eigen::StreamPrecision,0,", ", "\n","","","(",")");

void fmShape_show(const fort::myrmidon::Shape * s) {
	switch(s->ShapeType()) {
	case fort::myrmidon::Shape::Type::CIRCLE:
		fmCircle_show((fort::myrmidon::Circle*)(s));
		return;
	case fort::myrmidon::Shape::Type::CAPSULE:
		fmCapsule_show((const fort::myrmidon::Capsule*)(s));
		return;
	case fort::myrmidon::Shape::Type::POLYGON:
		fmPolygon_show((const fort::myrmidon::Polygon*)(s));
		return;
	}
}

fort::myrmidon::Shape::Ptr fmShape_as(SEXP s) {
	using namespace fort::myrmidon;
	try {
		return std::make_shared<Capsule>(Rcpp::as<Capsule>(s));
	} catch (...) {}
	try {
		return std::make_shared<Polygon>(Rcpp::as<Polygon>(s));
	} catch (...) {}
	try {
		return std::make_shared<Circle>(Rcpp::as<Circle>(s));
	} catch (...) {}
	throw std::runtime_error("Could not convert R object to fmCircle, fmCapsule or fmPolygon");
}


void fmCircle_show(const fort::myrmidon::Circle * c) {
	Rcpp::Rcout << "fmCircle($center = "
	            << c->Center().transpose().format(fmFormat)
	            << ", $radius = "
	            << c->Radius()
	            << ")\n";
}

void fmCapsule_show(const fort::myrmidon::Capsule * c) {
	Rcpp::Rcout << "fmCapsule($c1 = "
	            << c->C1().transpose().format(fmFormat)
	            << ", $c2 = "
	            << c->C2().transpose().format(fmFormat)
	            << ", $r1 = "
	            << c->R1()
	            << ", $r2 = "
	            << c->R2()
	            << ")\n";
}

void fmPolygon_show(const fort::myrmidon::Polygon * p) {
	Rcpp::Rcout << "fmPolygon";
	std::string prefix = "(";
	for ( const auto & v : p->Vertices() ) {
		Rcpp::Rcout << prefix << v.transpose().format(fmFormat);
		prefix= ", ";
	}
	Rcpp::Rcout << ")\n";
}


void fmShapeList_show(const fort::myrmidon::Shape::List * l) {
	size_t i = 0;
	for( const auto & s : *l) {
		Rcpp::Rcout << "[[" << ++i << "]]\n"
		            << "[1] ";
		fmShape_show(s.get());
	}
}


fort::myrmidon::Shape::Ptr fmShape_clone(const fort::myrmidon::Shape & s) {
	switch(s.ShapeType()) {
	case fort::myrmidon::Shape::Type::CIRCLE:
		return std::make_shared<fort::myrmidon::Circle>((const fort::myrmidon::Circle&)(s));
	case fort::myrmidon::Shape::Type::CAPSULE:
		return std::make_shared<fort::myrmidon::Capsule>((const fort::myrmidon::Capsule&)(s));
	case fort::myrmidon::Shape::Type::POLYGON:
		return std::make_shared<fort::myrmidon::Polygon>((const fort::myrmidon::Polygon&)(s));
	}

	return nullptr;
}

SEXP fmShape_wrap(fort::myrmidon::Shape * s) {
	switch(s->ShapeType()) {
	case fort::myrmidon::Shape::Type::CIRCLE:
		return Rcpp::wrap<fort::myrmidon::Circle>(*(fort::myrmidon::Circle *)(s));
	case fort::myrmidon::Shape::Type::CAPSULE:
		return Rcpp::wrap<fort::myrmidon::Capsule>(*(fort::myrmidon::Capsule *)(s));
	case fort::myrmidon::Shape::Type::POLYGON:
		return Rcpp::wrap<fort::myrmidon::Polygon>(*(fort::myrmidon::Polygon *)(s));
	}
	return R_NilValue;
}

SEXP fmShapeList_at(const fort::myrmidon::Shape::List * l, size_t index) {
	return fmShape_wrap(l->at(index-1).get());
}

void fmShapeList_set(fort::myrmidon::Shape::List * l, size_t index, const fort::myrmidon::Shape &s) {
	l->at(index-1) = fmShape_clone(s);
}

Rcpp::List fmShapeList_asList(fort::myrmidon::Shape::List * l) {
	Rcpp::List res(l->size());
	size_t i = 0;
	for(const auto & s : *l) {
		res[i] = fmShape_wrap(s.get());
		++i;
	}
	return res;
}


RCPP_MODULE(shapes) {
	using namespace Rcpp;
	class_<fort::myrmidon::Shape>("fmShape")
		.property("shapeType",&fort::myrmidon::Shape::ShapeType)
		.const_method("show",&fmShape_show)
		;

	class_<fort::myrmidon::Circle>("fmCircle")
		.derives<fort::myrmidon::Shape>("fmShape")
		.constructor<Eigen::Vector2d,double>()
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
		.constructor<Eigen::Vector2d,Eigen::Vector2d,double,double>()
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
		.constructor<fort::myrmidon::Vector2dList>()
		.const_method("show",&fmPolygon_show)
		.property("vertices",
		          &fort::myrmidon::Polygon::Vertices,
		          &fort::myrmidon::Polygon::SetVertices,
		          "the polygon vertices")
		;


	class_<fort::myrmidon::Shape::List>("fmShapeList")
		.const_method("show",
		              &fmShapeList_show)
		.const_method("size",&fort::myrmidon::Shape::List::size)
		.const_method("[[",&fmShapeList_at)
		.method("[[<-",&fmShapeList_set)
		.method("as.list",&fmShapeList_asList)
		;
}



//' Creates a fmShapeList
//' @param shapes the shape in the list. should be a list of fmCircle,
//'   fmCapsule or fmPolygon.
//' @return a closed \code{\link{fmShapeList}} with the given shapes
//' @family fmShape methods and classes
//[[Rcpp::export]]
fort::myrmidon::Shape::List fmShapeListCreate(SEXP shapes = R_NilValue) {
	using namespace Rcpp;
	if ( shapes == R_NilValue) {
		return fort::myrmidon::Shape::List();
	}
	Rcpp::List list(shapes);
	fort::myrmidon::Shape::List res;
	res.reserve(list.size());
	for ( const auto & s : list ) {
		auto casted = fmShape_as(s);
		res.push_back(casted);
	}
	return res;
}

namespace Rcpp {
template <> fort::myrmidon::Shape::List as(SEXP l) {
	if ( l == R_NilValue || Rcpp::is<Rcpp::List>(l) ) {
		return fmShapeListCreate(l);
	}
	fort::myrmidon::Shape::List * obj = Rcpp::internal::as_module_object<fort::myrmidon::Shape::List>(l);
	return *obj;
}
}

//[[Rcpp::export]]
void pfmIWantAShapeList(const fort::myrmidon::Shape::List & l) {
}
