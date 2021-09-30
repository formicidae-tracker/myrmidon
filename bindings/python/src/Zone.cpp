#include "BindTypes.hpp"

namespace py = pybind11;

#include <fort/myrmidon/Zone.hpp>


void BindZoneDefinition(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<ZoneDefinition,
	           ZoneDefinition::Ptr>(m,
	                                "ZoneDefinition",
	                                "Defines the geometry of a :class:`Zone` during a time interval")
		.def_property("Shapes",
		     &ZoneDefinition::Shapes,
		     &ZoneDefinition::SetShapes,
		     "List[Shape]: the list of Shape that defines the geometry")
		.def_property("Start",
		              &ZoneDefinition::Start,
		              &ZoneDefinition::SetStart,
		              "Time: the first valid Time for this ZoneDefinition")
		.def_property("End",
		              &ZoneDefinition::End,
		              &ZoneDefinition::SetEnd,
		              "Time: the first invalid Time for this ZoneDefinition")
		;
}

void BindZone(py::module_ & m) {
	using namespace fort::myrmidon;
	BindZoneDefinition(m);
	py::class_<Zone,Zone::Ptr>(m,
	                           "Zone",
	                           R"pydoc(
Defines a named region of interest for tracking and interactions

Zones defines a named region of interest for tracking and
interactions. It means that two :class:`Ant`, which are lying in two
separate Zones will never report a collision or interaction.

Zones are uniquely identified trough their :meth:`ID` in an
:class:`Experiment`, but they are managed and are related to a single
:class:`Space`. They also have a user defined :attr:`Name`, but it is
never used internally.

By default an :class:`Ant` lies in no Zone at all, which is identified
by the ZoneID 0.

Zone have time valid :class:`ZoneDefinition` which represents their
geometries. In most cases a Zone will have a single
:class:`ZoneDefinition` valid for
] :func:`Time.SinceEver` ; :func:`Time.Forever` [. However it is possible
to add as many different ZoneDefinitions to a Zone, as long as they do
not overlap in Time. The definitions are manipulated with
:meth:`AddDefinition` and :meth:`DeleteDefinition`.
)pydoc")
		.def_property("Name",
		     &Zone::Name,
		     &Zone::SetName,
		     "str: the name of the Zone")
		.def_property_readonly("ID",
		                       &Zone::ID,
		                       "int: the unique ID for this Zone")
		.def_property_readonly("Definitions",
		                       &Zone::Definitions,
		                       py::return_value_policy::reference_internal,
		                       "List[ZoneDefinition]: the definitions for this Zone")
		.def("AddDefinition",&Zone::AddDefinition,
		     py::arg("shapes") = py::list(),
		     py::arg("start") = fort::Time::SinceEver(),
		     py::arg("end") = fort::Time::Forever(),
		     R"pydoc(
    Adds a new ZoneDefinition to this Zone

    Args:
        shapes (List[Shape]): the
            geometry of the ZoneDefinition
        start (Time): the first valid Time for the
            ZoneDefinition
        end (Time): the first valid Time for the
            ZoneDefinition

    Returns:
        ZoneDefinition: the new :class:`ZoneDefinition` for this Zone

    Raises:
        ValueError: if start or end would make an overlapping
            definition with another ZoneDefinition of this Zone
)pydoc")
		.def("DeleteDefinition",
		     &Zone::DeleteDefinition,
		     py::arg("index"),
		     R"pydoc(
    Removes a ZoneDefinition

    Args:
        index (int): the index in :attr:`Definitions` to remove.
    Raises:
        IndexError: if index >= len(self.Definitions)
)pydoc")
		;
}
