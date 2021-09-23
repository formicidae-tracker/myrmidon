#include "BindTypes.hpp"

namespace py = pybind11;

#include <fort/myrmidon/Zone.hpp>


void BindZoneDefinition(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<ZoneDefinition,
	           ZoneDefinition::Ptr>(m,
	                                "ZoneDefinition",
	                                "Defines the geometry of a Zone during a time interval")
		.def_property("Shapes",
		     &ZoneDefinition::Shapes,
		     &ZoneDefinition::SetShapes,
		     ":obj:`list` of :obj:`py_fort_myrmidon.Shape`: the list of Shape that defines the geometry")
		.def_property("Start",
		              &ZoneDefinition::Start,
		              &ZoneDefinition::SetStart,
		              "py_fort_myrmidon.Time: the first valid Time for this ZoneDefinition")
		.def_property("End",
		              &ZoneDefinition::End,
		              &ZoneDefinition::SetEnd,
		              "py_fort_myrmidon.Time: the first invalid Time for this ZoneDefinition")
		;
}

void BindZone(py::module_ & m) {
	using namespace fort::myrmidon;
	BindZoneDefinition(m);
	py::class_<Zone,Zone::Ptr>(m,
	                           "Zone",
	                           R"pydoc(
Defines a named region of interest for tracking and interactions

Zone defines a named region of interest for tracking and
interactions. It means that two Ant, which are lying in two separate
Zone will never report a collision or Interaction.

Zones are uniquely idetinfied trough their ID in an Experiment, but
they are managed and are related to a single Space. They also have a
user defined name, but it is never used internally.

By default an Ant lies in no Zone, which is identified by the ZoneID
0.

Zone have time valid ZoneDefinition which represents their
geometry. In most cases a Zone will have a single ZoneDefinition valid
for ]Time.SinceEver();Time.Forever()[. However it is possible to add
as many different definition to a Zone, as long as they do not overlap
in Time. The defintions are manipulated with AddDefinition() and
DeleteDefinition().
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
		                       ":obj:`list` of :obj:`py_fort_myrmidon.ZoneDefinition`: the definitions for this Zone")
		.def("AddDefinition",&Zone::AddDefinition,
		     py::arg("shapes") = py::list(),
		     py::arg("start") = fort::Time::SinceEver(),
		     py::arg("end") = fort::Time::Forever(),
		     R"pydoc(
    Adds a new ZoneDefinition to this Zone

    Args:
        shapes (:obj:`list` of :obj:`py_fort_myrmidon.Shape`): the
            geometry of the ZoneDefinition
        start (py_fort_myrmidon.Time): the first valid Time for the
            ZoneDefinition
        end (py_fort_myrmidon.Time): the first valid Time for the
            ZoneDefinition

    Returns:
        py_fort_myrmidon.ZoneDefinition: the new ZoneDefinition for this Zone

    Raises:
        ValueError: if start or end would make an overlapping
            definition with another Zone's ZoneDefinition
)pydoc")
		.def("DeleteDefinition",
		     &Zone::DeleteDefinition,
		     py::arg("index"),
		     R"pydoc(
    Removes a ZoneDefinition

    Args:
        index (int): the index in Zone.Definitions to remove.
    Raises:
        IndexError: if index >= len(Zone.Definitions)
)pydoc")
		;
}
