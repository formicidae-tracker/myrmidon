#include "BindTypes.hpp"
#include <pybind11/stl_bind.h>

namespace py = pybind11;

void BindShapes(py::module_ &m) {
	using namespace fort::myrmidon;
	py::class_<Shape, Shape::Ptr> shape(
	    m,
	    "Shape",
	    R"pydoc(
A Generic class for a Shape
)pydoc"
	);
	shape.def_property_readonly(
	    "ShapeType",
	    &Shape::ShapeType,
	    "(fort_myrmidon.Shape.Type): the type of the shape"
	);

	py::enum_<Shape::Type>(shape, "Type", "Enum for the type of a Shape")
	    .value("CIRCLE", Shape::Type::CIRCLE, "int: a circle")
	    .value("CAPSULE", Shape::Type::CAPSULE, "int: a capsule")
	    .value("POLYGON", Shape::Type::POLYGON, "int: a polygon");

	py::class_<Circle, Circle::Ptr>(
	    m,
	    "Circle",
	    shape,
	    R"pydoc(
Represents a circle
)pydoc"
	)
	    .def(
	        py::init<Eigen::Vector2d, double>(),
	        py::arg("Center") = Eigen::Vector2d(0, 0),
	        py::arg("Radius") = 1.0
	    )
	    .def_property(
	        "Center",
	        &Circle::Center,
	        &Circle::SetCenter,
	        "numpy.ndarray: the center of the circle (float64, size [2,1])"
	    )
	    .def_property(
	        "Radius",
	        &Circle::Radius,
	        &Circle::SetRadius,
	        "float: the radius of the circle"
	    );

	py::class_<Capsule, Capsule::Ptr>(
	    m,
	    "Capsule",
	    shape,
	    R"pydoc(
Represents a capsule

A capsule is the region inside and between two given circles.
)pydoc"
	)
	    .def(
	        py::init<Eigen::Vector2d, Eigen::Vector2d, double, double>(),
	        py::arg("C1") = Eigen::Vector2d(0, 0),
	        py::arg("C2") = Eigen::Vector2d(1, 1),
	        py::arg("R1") = 1.0,
	        py::arg("R2") = 1.0
	    )
	    .def_property(
	        "C1",
	        &Capsule::C1,
	        &Capsule::SetC1,
	        "numpy.ndarray: the center of the first circle (float64, size "
	        "[2,1])"
	    )
	    .def_property(
	        "C2",
	        &Capsule::C2,
	        &Capsule::SetC2,
	        "numpy.ndarray: the center of the second circle (float64, size "
	        "[2,1])"
	    )
	    .def_property(
	        "R1",
	        &Capsule::R1,
	        &Capsule::SetR1,
	        "float: the radius of the first circle"
	    )
	    .def_property(
	        "R2",
	        &Capsule::R2,
	        &Capsule::SetR2,
	        "float: the radius of the second circle"
	    );

	py::bind_vector<Vector2dList>(m, "Vector2dList");
	py::implicitly_convertible<py::list, Vector2dList>();

	py::class_<Polygon, Polygon::Ptr>(
	    m,
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

        square = fort_myrmidon.Polygon(Vertices = [[1,1],[-1,1],[-1,-1],[1,-1]])
        hourglass = fort_myrmidon.Polygon(Vertices = [[1,1],[-1,-1],[1,-1],[-1,1]])
)pydoc"
	)
	    .def(py::init<Vector2dList>(), py::arg("Vertices"))
	    .def(py::init<>([]() {
		    return Polygon(Vector2dList(
		        {Eigen::Vector2d(1, 1),
		         Eigen::Vector2d(-1, 1),
		         Eigen::Vector2d(-1, -1),
		         Eigen::Vector2d(1, -1)}
		    ));
	    }))
	    .def_property(
	        "Vertices",
	        &Polygon::Vertices,
	        &Polygon::SetVertices,
	        "List[numpy.ndarray]: a list of the polygon vertices (float64 , "
	        "size [2,1])"
	    );

	py::bind_vector<Shape::List>(m, "ShapeList");
	py::implicitly_convertible<py::list, Shape::List>();
}
