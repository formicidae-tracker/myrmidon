#include <pybind11/eigen.h>

#include <fort/myrmidon/Shapes.hpp>

namespace py = pybind11;


void BindShapes(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<Shape,Shape::Ptr> shape(m,"Shape",
	                                   R"pydoc(
A Generic class for a Shape
)pydoc");

	shape.def_property_readonly("ShapeType",
	                            &Shape::ShapeType,
	                            "(py_fort_myrmidon.Shape.Type): the type of the shape");

	py::enum_<Shape::Type>(shape,
	                       "Type",
	                       "Enum for the type of a Shape")
		.value("CIRCLE",Shape::Type::CIRCLE,"int: a circle")
		.value("CAPSULE",Shape::Type::CAPSULE,"int: a capsule")
		.value("POLYGON",Shape::Type::POLYGON,"int: a polygon")
		;

	py::class_<Circle,Circle::Ptr>(m,"Circle",
	                               shape,
	                               R"pydoc(
Represents a circle
)pydoc")
		.def(py::init<Eigen::Vector2d,double>(),
		     py::arg("Center") = Eigen::Vector2d(0,0),
		     py::arg("Radius") = 1.0)
		.def_property("Center",&Circle::Center,&Circle::SetCenter,
		              "numpy.ndarray: the center of the circle (float64, size [2,1])")
		.def_property("Radius",&Circle::Radius,&Circle::SetRadius,
		              "float: the radius of the circle")
		;

	py::class_<Capsule,Capsule::Ptr>(m,
	                                 "Capsule",
	                                 shape,
	                                 R"pydoc(
Represents a capsule

A capsule is the region inside and between two given circles.
)pydoc")
		.def(py::init<Eigen::Vector2d,Eigen::Vector2d,double,double>(),
		     py::arg("C1") = Eigen::Vector2d(0,0),
		     py::arg("C2") = Eigen::Vector2d(1,1),
		     py::arg("R1") = 1.0,
		     py::arg("R2") = 1.0)
		.def_property("C1",&Capsule::C1,&Capsule::SetC1,
		              "numpy.ndarray: the center of the first circle (float64, size [2,1])")
		.def_property("C2",&Capsule::C2,&Capsule::SetC2,
		              "numpy.ndarray: the center of the second circle (float64, size [2,1])")
		.def_property("R1",&Capsule::R1,&Capsule::SetR1,
		              "float: the radius of the first circle")
		.def_property("R2",&Capsule::R2,&Capsule::SetR2,
		              "float: the radius of the second circle")

		;

	py::class_<Vector2dList>(m,"Vector2dList")
		.def(py::init())
		.def(py::init([](const py::list & points) {
			              Vector2dList l;
			              l.reserve(points.size());
			              for ( const auto & p : points ) {
				              l.push_back(p.cast<Eigen::Vector2d>());
			              }
			              return l;
		              }),
			py::arg("points"))
		.def("append",
		     [](Vector2dList & list,const Eigen::Vector2d & p) {
			     list.push_back(p);
		     })
		.def("extend",
		     [](Vector2dList & list, const Vector2dList & other) {
			     list.reserve(list.size() + other.size());
			     for ( const auto & v : other) {
				     list.push_back(v);
			     }
		     })
		.def("copy",
		     [](const Vector2dList & list) -> Vector2dList {
			     return list;
		     })
		.def("clear",&Vector2dList::clear)
		.def("pop",[](Vector2dList & list,int pos) {
			           if ( pos < 0 ) {
				           pos = list.size() + pos;
			           }
			           if ( pos < 0 || pos >= list.size() ) {
				           throw std::out_of_range(std::to_string(pos)
				                                   + " should be in [0;"
				                                   + std::to_string(list.size())
				                                   + "[");
			           }
			           Eigen::Vector2d res = list[pos];
			           list.erase(list.begin() + pos);
			           return res;
		           },py::arg("pos") = -1)
		.def("__len__",&Vector2dList::size)
		.def("__iter__",
		     [](const Vector2dList & list) {
			     return py::make_iterator(list.begin(),list.end());
		     })
		.def("__getitem__",
		     [](Vector2dList & list,int pos) -> Eigen::Vector2d &{
			     return list.at(pos);
		     })
		;

	py::implicitly_convertible<py::list,Vector2dList>();


	py::class_<Polygon,Polygon::Ptr>(m,
	                                 "Polygon",
	                                 shape,
	                                 R"pydoc(
Represents a closed polygon.

Represents a closed polygon from a list of vertices. The polygon is
always considered closed. i.e. ``[[1,1],[-1,1],[-1,-1],[1,-1]]`` is a
closed square.

Note:
    Order matters as ``[[1,1],[-1,-1],[1,-1],[-1,1]]`` represents an
    hourglass.

Example:
    .. code-block:: python

        square = py_fort_myrmidon.Polygon(Vertices = [[1,1],[-1,1],[-1,-1],[1,-1]])
        hourglass = py_fort_myrmidon.Polygon(Vertices = [[1,1],[-1,-1],[1,-1],[-1,1]])
)pydoc")
		.def(py::init<Vector2dList>(),py::arg("Vertices"))
		.def_property("Vertices",
		              &Polygon::Vertices,
		              &Polygon::SetVertices,
		              ":obj:`list` of :obj:`numpy.ndarray`: a list of the polygon vertices (float64 , size [2,1])")
		;


}
