#include "BindTypes.hpp"

#include <fort/myrmidon/Ant.hpp>
#include <fort/myrmidon/Identification.hpp>

namespace py = pybind11;


void BindAnt(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<Ant,Ant::Ptr> ant(m,
	                             "Ant",
	                             R"pydoc(

Ant are the main object of interest of an :class:`Experiment`. They
are identified from tags with :class:`Identification`, have a virtual
shape to perform collision and interaction detection, and holds user
defined metadata.

Ant can only be created from an :class:`Experiment` with
:meth:`Experiment.CreateAnt`.

Ant are uniquely identified by an :attr:`ID`. By convention we use
decimal notation with up to two ``0`` prefix to display these AntID, as
returned by :func:`FormatAntID`.

Instead of working directly with TagID **fort-myrmidon** uses
:class:`Identification` to relate a tag value to an Ant. An Ant could
have different Identifications, allowing us to use different tag ID to
refer to the same individual. One would use :meth:`IdentifiedAt` to
obtain the tag ID that identifies an Ant at a given :class:`Time`.

Each Ant has an associated virtual shape that is used to compute
instantaneous collision detection ( :func:`Query.CollideFrame` ), or
timed ant interactions ( :func:`Query.ComputeAntInteraction` ). These
shapes can be defined manually in **fort-studio** or programmatically
accessed and modified with :attr:`Capsules`, :meth:`AddCaspule`,
:meth:`DeleteCapsule` and :meth:`ClearCapsules`.

Basic visualization of Experiment data can be done through
**fort-studio**. Ants are displayed according to their
:attr:`DisplayStatus` and :attr:`DisplayColor`.

Ant can stores timed user defined metadata. These are modifiable
using :meth:`SetValue` and :meth:`DeleteValue` and accesible through
:meth:`GetValue`.

)pydoc");

	py::enum_<Ant::DisplayState>(ant,
	                             "DisplayState",
	                             "Enumerates the possible display state for an Ant")
		.value("VISIBLE",Ant::DisplayState::VISIBLE,"the Ant is visible")
		.value("HIDDEN",Ant::DisplayState::HIDDEN,"the Ant is hidden")
		.value("SOLO",Ant::DisplayState::SOLO,"the Ant is visible and all other non-solo ant are hidden");


	ant.def_property_readonly("Identifications",
		                       &Ant::Identifications,
	                          "List[Identification]: all Identification that target this Ant, ordered by validity time.")
		.def_property_readonly("ID",
		                       &Ant::ID,
		                       "int: the AntID for this Ant")
		.def_property("DisplayColor",
		              &Ant::DisplayColor,
		              &Ant::SetDisplayColor,
		              "Tuple[int,int,int]: the color used to display the Ant in **fort-studio**")
		.def_property("DisplayStatus",
		              &Ant::DisplayStatus,
		              &Ant::SetDisplayStatus,
		              "Ant.DisplayState: the DisplayState in **fort-studio** for this Ant")
		.def_property_readonly("Capsules",
		                       &Ant::Capsules,
		                       "List[Tuple[int,Capsule]]: a list of capsules and their type")
		.def("IdentifiedAt",
	        &Ant::IdentifiedAt,
	        py::arg("time"),
	        R"pydoc(
Gets the TagID identifiying this Ant at a given time

Args:
    time (Time): the time we want an identification for

Returns:
    int: the TagID that identifies this Ant at time.

Raises:
    Error: if no tag identifies this Ant at **time**.
)pydoc")
		.def("GetValue",
		     &Ant::GetValue,
		     py::arg("key"),
		     py::arg("time"),
		     R"pydoc(
Gets user defined timed metadata.

Args:
    key (str): the key to query
    time (Time): the time, possibly infinite to query for

Returns:
    bool, int, float, str or Time: the value for **key** at **time**,
    if defined, or the Experiment's default value.

Raises:
    IndexError: if **key** is not defined in :class:`Experiment`
)pydoc")
		.def("GetValues",
		     [](const Ant & self, const std::string & key) -> std::vector<std::pair<fort::Time,Value>> {
			     std::vector<std::pair<fort::Time,Value>> res;
			     const auto & values = self.GetValues(key);
			     std::copy(values.begin(),values.end(),std::back_inserter(res));
			     return res;
		     },
		     py::arg("key"),
		     R"pydoc(
Gets metadata key changes over time.

Args:
    key (str): the key to list changes

Raises:
    IndexError: if **key** is not defined in :class:`Experiment`
)pydoc")
		.def("SetValue",
		     &Ant::SetValue,
		     py::arg("key"),
		     py::arg("value"),
		     py::arg("time"),
		     R"pydoc(
Sets a user defined timed metadata

Args:
    key (str): the key to defined
    value (bool, int, float, str or Time): the wanted
        value.
    time (Time): the first Time where **key** will be set to
        **value**. It can be :meth:`Time.SinceEver`

Raises:
    IndexError: if **key** is not defined in the :class:`Experiment`
    ValueError: if **time** is :meth:`Time.Forever`
    RuntimeError: if **value** is not of the right type for **key**
)pydoc")
		.def("DeleteValue",
		     &Ant::DeleteValue,
		     py::arg("key"),
		     py::arg("time"),
		     R"pydoc(
Clears a user defined timed metadata.

Args:
    key (str): the key to clear
    time (Time): the time to clear a value for key

Raises:
    IndexError: if **key** was not previously set for **time** with
        :meth:`SetValue`.
)pydoc")
		.def("AddCapsule",
		     &Ant::AddCapsule,
		     py::arg("shapeTypeID"),
		     py::arg("capsule"),
		     R"pydoc(
Adds a Capsule to the Ant virtual shape.

Args:
    shapeTypeID (int): the AntShapeTypeID associated with the capsule
    capsule (Capsule): the capsule to add

Raises:
    ValueError: if **shapeTypeID** is not defined in the
        :class:`Experiment`
)pydoc")
		.def("DeleteCapsule",
		     &Ant::DeleteCapsule,
		     py::arg("index"),
		     R"pydoc(
Removes one of the shape

Args:
    index (int): the index to remove in :attr:`Capsules`

Raises:
    IndexError: if ``index >= len(self.Capsules())``
)pydoc")
		.def("ClearCapsules",
		     &Ant::ClearCapsules,
		     R"pydoc(
Removes all capsules for this Ant.
)pydoc")
		;

}
