#include "BindTypes.hpp"


namespace py = pybind11;

#include <fort/myrmidon/Space.hpp>

void BindSpace(py::module_ &m) {
	using namespace fort::myrmidon;
	py::class_<Space, Space::Ptr>(
	    m,
	    "Space",
	    R"pydoc(
An homogenous coordinate system for tracking data.

A Space represents an homogenous coordinate system for tracking
data. I.e. coordinates from two different Space cannot be
compared.

Spaces are uniquely identified with their :meth:`ID`

Spaces can only be created from a :class:`Experiment`
with :meth:`Experiment.CreateSpace`.

Within a single Space , it could be relevant to define
:class:`Zone`. These are manipulated with :meth:`CreateZone` and
:meth:`DeleteZone`.
)pydoc"
	)
	    .def_property_readonly(
	        "ID",
	        &Space::ID,
	        "(int): the unique SpaceID of this space"
	    )
	    .def_property(
	        "Name",
	        &Space::Name,
	        &Space::SetName,
	        "(str): the name for this space"
	    )
	    .def(
	        "CreateZone",
	        &Space::CreateZone,
	        py::arg("name"),
	        py::return_value_policy::reference_internal,
	        R"pydoc(
    Creates a new Zone in this space

    Args:
        name (str): the name for the new Zone
    Returns:
        fort_myrmidon.Zone: the newly created Zone
)pydoc"
	    )
	    .def(
	        "DeleteZone",
	        &Space::DeleteZone,
	        py::arg("zoneID"),
	        R"pydoc(
    Deletes a Zone of this Space

    Args:
        zoneID (int): the zoneID in self.Zones to delete
    Raises:
        IndexError: if zoneID is not in self.Zones
)pydoc"
	    )
	    .def_property_readonly(
	        "Zones",
	        &Space::Zones,
	        py::return_value_policy::reference_internal,
	        "Dict[int,Zone]: the Space's Zone by their ZoneID"
	    )
	    .def(
	        "LocateMovieFrame",
	        &Space::LocateMovieFrame,
	        py::arg("time"),
	        R"pydoc(
    Locates a movie file and frame for a given time.

    Args:
        time (fort_myrmidon.Time): the time to query for
    Returns:
        str: the absolute file path to the movie file
        int: the movie frame number that was acquired at or just after time
    Raises:
        IndexError: if time is outside of this Space tracking data
)pydoc"
	    );
}
