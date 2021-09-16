#include <pybind11/eigen.h>

#include <fort/myrmidon/Shapes.hpp>

namespace py = pybind11;


void BindShapes(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<Shape,Shape::Ptr> shape(m,"Shape",
	                                   R"pydoc(
A Generic class for a Shape

Attributes:
    ShapeType (py_fort_myrmidon.Shape.Type): the type of the shape
)pydoc");

	shape.def_property_readonly("ShapeType",
	                            &Shape::ShapeType,
	                            " (py_fort_myrmidon.Shape.Type): the type of the shape");

	py::enum_<Shape::Type>(shape,
	                       "Type",
	                       R"pydoc(
Rhe type of a Shape
)pydoc")
		.value("CIRCLE",Shape::Type::CIRCLE,"a circle")
		.value("CAPSULE",Shape::Type::CAPSULE,"a capsule")
		.value("POLYGON",Shape::Type::POLYGON,"a polygon");

	py::class_<Circle,Circle::Ptr>(m,"Circle",
	                               shape,
	                               R"pydoc(
Represents a circle

Attributes:
    Center (:obj:numpy.ndarray[numpy.float64[2,1]]): the center of the circle
    Radius (float): the radius of the circle
)pydoc")
		.def(py::init<Eigen::Vector2d,double>(),py::arg("center"),py::arg("radius"))
		.def_property("Center",&Circle::Center,&Circle::SetCenter,
		              " (numpy.ndarray[numpy.float64[2,1]]): the center of the circle")
		.def_property("Radius",&Circle::Radius,&Circle::SetRadius,
		              " (float): the radius of the circle")
		;

	py::class_<Capsule,Capsule::Ptr>(m,
	                                 "Capsule",
	                                 shape,
	                                 R"pydoc(
Represents a capsule

A capsule is the region inside and between two given circles.

Attributes:
    C1 (:obj:numpy.ndarray[numpy.float64[2,1]]): the center of the first circle
    C2 (:obj:numpy.ndarray[numpy.float64[2,1]]): the center of the second circle
    R1 (float): the radius of the first circle
    R2 (float): the radius of the second circle
)pydoc")
		.def(py::init<Eigen::Vector2d,Eigen::Vector2d,double,double>(),py::arg("c1"),py::arg("c2"),py::arg("r1"),py::arg("r2"))
		.def_property("C1",&Capsule::C1,&Capsule::SetC1,
		              " (numpy.ndarray[numpy.float64[2,1]]): the center of the first circle")
		.def_property("C2",&Capsule::C2,&Capsule::SetC2,
		              " (numpy.ndarray[numpy.float64[2,1]]): the center of the second circle")
		.def_property("R1",&Capsule::R1,&Capsule::SetR1,
		              " (float): the radius of the first circle")
		.def_property("R2",&Capsule::R2,&Capsule::SetR2,
		              " (float): the radius of the second circle")

		;

	py::class_<Vector2dList>(m,"Vector2dList")
		.def(py::init([](const py::list & vertices) {
			              Vector2dList l;
			              l.reserve(vertices.size());
			              for ( const auto & v : vertices ) {
				              l.push_back(v.cast<Eigen::Vector2d>());
			              }
			              return l;
		              }))
		;

	py::implicitly_convertible<py::list,Vector2dList>();


	py::class_<Polygon,Polygon::Ptr>(m,"Polygon",shape)
		.def(py::init<Vector2dList>(),py::arg("vertices"))
		.def_property("Vertices",&Polygon::Vertices,&Polygon::SetVertices)
		;


}
