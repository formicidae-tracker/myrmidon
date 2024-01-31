#include "BindTypes.hpp"

#include <fort/myrmidon/Experiment.hpp>

#include <condition_variable>
#include <thread>

#include "Progress.hpp"

namespace py = pybind11;
using namespace pybind11::literals;

void EnsureAllDataIsLoaded(
    const fort::myrmidon::Experiment &e, bool fixCorruptedData
) {
	e.EnsureAllDataIsLoaded(
	    std::make_unique<ItemProgress>("loading data"),
	    fixCorruptedData
	);
}

void BindExperiment(py::module_ & m) {
	using namespace fort::myrmidon;

	py::enum_<fort::tags::Family>(m,
	                              "TagFamily",
	                              "Enumeration of available tag families available in FORT")
		.value("Tag36h11",fort::tags::Family::Tag36h11,"36h11 family")
		.value("Tag36h10",fort::tags::Family::Tag36h10,"36h10 family")
		.value("Tag36ARTag",fort::tags::Family::Tag36ARTag,"ARTag family")
		.value("Tag16h5",fort::tags::Family::Tag16h5,"16h5 family")
		.value("Tag25h9",fort::tags::Family::Tag25h9,"25h9 family")
		.value("Circle21h7",fort::tags::Family::Circle21h7,"Circle21h7 family")
		.value("Circle49h12",fort::tags::Family::Circle49h12,"Circle49h12 family")
		.value("Custom48h12",fort::tags::Family::Custom48h12,"Custom48h12 family")
		.value("Standard41h12",fort::tags::Family::Standard41h12,"Standard41h12 family")
		.value("Standard52h13",fort::tags::Family::Standard52h13,"Standard52h13 family")
		.value("Undefined",fort::tags::Family::Undefined,"Undefined family value");

	py::class_<Experiment>(m,
	                       "Experiment",
	                       R"pydoc(

An Experiment olds a collection of :class:`Ant`,
:class:`Identification`, :class:`Space` and :class:`Zone` and give
access to the identified tracking data instantaneous collision and
interaction detection through :class:`Query`.

Experiment are save to the filesystem in `.myrmidon` files. One can
use :meth:`Open`, :meth:`OpenDataLess`, and :meth:`Save` to interact
with these files.

One can use :meth:`AddTrackingDataDirectory` to link an Experiment
with some tracking data, organized by **fort-leto** in a tracking data
directory. This data must be assigned to a :class:`Space` (previously
created with :meth:`CreateSpace`).  Experiment saves relative links to
these tracking data directory. These paths are relative, so one can
rename a `.myrmidon` file on the filesystem with :meth:`Save`, but it
must remains in the same directory.

In **fort-myrmidon**, tags are not used directly. Instead user are
required to make :class:`Ant` object (through :meth:`CreateAnt`) and
use :class:`Identification` (through :meth:`AddIdentification`) to
relate a tag value to an Ant. To perform collision and interaction
detection, users can create for each Ant a virtual shape, made of a
collection of :class:`Capsule`. Each Capsule is assigned an
AntShapeTypeID (an integer starting from 1) which must be previously
defined using :meth:`CreateAntShapeType`. There is no predefined
AntShapeTypeID.

**fort-studio** allows to make measurement on close-up of each
:class:`Ant`. These measurement must be assigned to a type, created
with :meth:`CreateMeasurementType`. There is a predefined, undeletable
MeasurementTypeID: :attr:`HEAD_TAIL_MEASUREMENT_TYPE`. It is used to
automatically determine :attr:`Identification.AntPosition` and
:attr:`Identification.AntAngle` from **fort-studio** measurement.

Each :class:`Ant` can also holds a dictionnary of key/value pairs. The
key name, type and initial value for each Ant must be defined with
:meth:`SetMetaDataKey`. Through :meth:`Ant.SetValue`, individual,
timed value can be assigned to each Ant. There are no predefined keys.

An Experiment is also usuable without linking to any tracking
data. :meth:`OpenDataLess` can be used to open an existing Experiment,
previously linked with acquired data, but without requiring the data
to be present on the filesystem. Any :class:`Query` on such Experiment
object will report no data, but a :class:`TrackingSolver` (acquired
with :meth:`CompileTrackingSolver`) could be used, to perform, for
example identifications and collision detection on a live tracking
datastream of **fort-leto**. Also tracking and user-defined meta-data
can be manipulated without the need of the often very large tracking
data directory to be present on the filesystem.
)pydoc")
		.def(py::init(&fort::myrmidon::Experiment::Create),
		     py::arg("filepath"),
		     R"pydoc(
Initialize an experiment from a specified filesystem
location. This location will be used to determine relative path to
the tracking data.

Args:
    filepath (str): the wanted filesystem path to the experiment.
Returns:
    Experiment: a new empty Experiment associated with **filepath**
)pydoc")
		.def_static("Open",
		            &Experiment::Open,
		            py::arg("filepath"),
		            R"pydoc(
Opens an existing Experiment on the filesystem

Args:
    filepath (str): the filepath to open.

Returns:
    Experiment: the Experiment located at filepath

Raises:
    RuntimeError: if **filepath** does not contains a valid Experiment
        or associated tracking data is not accessible.
)pydoc")
		.def_static("OpenDataLess",
		            &Experiment::OpenDataLess,
		            py::arg("filepath"),
		            R"pydoc(
Opens an existing Experiment on the filesystem in dataless mode.

In dataless mode, no associated tracking data will be opened, but
a :class:`TrackingSolver` can be used to identify Ants in live tracking
situation.

Args:
    filepath (str): the filepath to open.

Returns:
    Experiment: the Experiment located at filepath

Raises:
    RuntimeError: if **filepath** does not contains a valid Experiment
)pydoc")

		.def("Save",
		     &Experiment::Save,
		     py::arg("filepath"),
		     R"pydoc(
Saves the experiment on the filesystem.

Args:
    filepath: the filepath to save the experiment to.

Raises:
    ValueError: if **filepath** would change the directory of the
        Experiment on the filesystem.
)pydoc")
		.def_property_readonly("AbsoluteFilePath",
		                       &Experiment::AbsoluteFilePath,
		                       "str: the absolute filepath of the Experiment")
		.def("CreateSpace",
		     &Experiment::CreateSpace,
		     py::arg("name"),
		     py::return_value_policy::reference_internal,
		     R"pydoc(
Creates a new Space in this Experiment.

Args:
    name (str): the name for the new space

Returns:
    Space: the newly created Space
)pydoc")
		.def("DeleteSpace",
		     &Experiment::DeleteSpace,
		     py::arg("spaceID"),
		     R"pydoc(
Deletes a Space from this Experiment.

Args:
    spaceID (str): the spaceID of this space

Raises:
    IndexError: if **spaceID** is not a valid for this Experiment.
)pydoc")
		.def_property_readonly("Spaces",
		                       &Experiment::Spaces,
		                       py::return_value_policy::reference_internal,
		                       "Dict[int,Space]: this Experiment space indexed by their SpaceID")

		.def("AddTrackingDataDirectory",
		     &Experiment::AddTrackingDataDirectory,
		     py::arg("spaceID"),
		     py::arg("filepath"),
		     py::arg("fixCorruptedData") = false,
		     R"pydoc(
Adds a tracking data directory to the Experiment.

Args:
    spaceID (int): the space to add the tracking data directory
        to.
    filepath (str): the filepath to the tracking data directory.
    fixCorruptedData (bool): In the event that some tracking data is
        corrupted, if False a FixableError will be raised. Otherwise
        an attempt to recover as much data as possible eill be made,
        but it may potentially remove a large chunk of data.

Returns:
    str: the relative path from self.AbsoluteFilePath to **filepath**,
        that will be the URI to identify the tracking data directory.

Raises:
    IndexError: if **spaceID** is not valid for this Experiment
    RuntimeError: if **filepath** is not a valid tracking data
        directory.
    FixableError: if **fixCorruptedData** is False and any data
        corruption is found.
    RuntimeError: if the data will overlap in time with another
        directory in the same space
    RuntimeError: if the data is used by another space
    ValueError: if the tag family does not match with other directory
        in the experiment
)pydoc")
		.def("RemoveTrackingDataDirectory",
		     &Experiment::RemoveTrackingDataDirectory,
		     py::arg("URI"),
		     R"pydoc(
Removes a tracking data directory from the Experiment.

Args:
    URI (str): the URI that identifies the tracking data directory

Raises:
    IndexError: if **URI** does not identifies a tracking data
        directory in this experiment.
)pydoc")

		.def_property_readonly("Ants",
		                       &Experiment::Ants,
		                       "Dict[int,Ant]: the Ant indexed by their AntID.")
		.def("CreateAnt",
		     &Experiment::CreateAnt,
		     R"pydoc(
Creates a new Ant in the Experiment.

Returns:
    Ant: the newly created Ant
)pydoc")
		.def("DeleteAnt",
		     &Experiment::DeleteAnt,
		     py::arg("antID"),
		     R"pydoc(
Deletes an Ant from the Experiment

Args:
    antID (int): the AntID of the Ant to remove
Raises:
    IndexError: if **antID** is invalid for the experiment
    RuntimeError: if the ant still have Identification targetting
        her
)pydoc")
		.def("AddIdentification",
		     &Experiment::AddIdentification,
		     py::arg("antID"),
		     py::arg("tagID"),
		     py::arg("start") = fort::Time::SinceEver(),
		     py::arg("end") = fort::Time::Forever(),
		     R"pydoc(
Adds an Identification to the Experiment

Args:
    antID (int): the ant to target
    tagID (int): the TagID to use
    start (Time): the first valid Time for the :class:`Identification`
    end (Time): the first invalid Time for the :class:`Identification`

Returns:
    Identification: the newly created Identification

Raises:
    IndexError: if **antID** is not valid for the Experiment
    OverlappingIdentification: if the resulting Identification would
        overlap in time with another one, either for a given **antID**
        or **tagID**.
)pydoc")
		.def("DeleteIdentification",
		     &Experiment::DeleteIdentification,
		     py::arg("identification"),
		     R"pydoc(
Deletes an Identification from this Experiment

Args:
    identification (Identification): the Identification to remove

Raises:
    ValueError: if **identification** is not a valid Identification
        object from this Experiment.
)pydoc")
		.def("FreeIdentificationRangeAt",
		     &Experiment::FreeIdentificationRangeAt,
		     py::arg("tagID"),
		     py::arg("time"),
		     R"pydoc(
Returns an available time range for a tag.

Args:
    tagID (int): the tagID to query for
    time (Time): a time that must be contained in the result.

Returns:
    Tuple[:class:`Time`, :class:`Time`]: A time range (can be
    (:meth:`Time.SinceEver` , :meth:`Time.Forever` )) containing
    **time** where **tagID** is not used by any Identification.

Raises:
    RuntimeError: if **tagID** already identifies an Ant at **time**
)pydoc")
		.def_property("Name",
		              &Experiment::Name,
		              &Experiment::SetName,
		              "str: the name of the Experiment")
		.def_property("Author",
		              &Experiment::Author,
		              &Experiment::SetAuthor,
		              "str: the author of the Experiment")
		.def_property("Comment",
		              &Experiment::Comment,
		              &Experiment::SetComment,
		              "str: a comment about the Experiment")
		.def_property_readonly("Family",
		                       &Experiment::Family,
		                       "TagFamily: the TagFamily used in the Experiment")
		.def_property("DefaultTagSize",
		              &Experiment::DefaultTagSize,
		              &Experiment::SetDefaultTagSize,
		              "float: the default tag size in mm used in the Experiment")

		.def_property_readonly("MeasurementTypeNames",
		                       &Experiment::MeasurementTypeNames,
		                       "Dict[int,str]: the measurement type name by their MeasurementTypeID")
		.def("CreateMeasurementType",
		     &Experiment::CreateMeasurementType,
		     py::arg("name"),
		     R"pydoc(
Creates a new measurement type

Args:
    name (str): the name of the measurement type

Returns:
    int: the MeasurementTypeID for the new type
)pydoc")
		.def_readonly_static("HEAD_TAIL_MEASUREMENT_TYPE_ID",
		                     &Experiment::HEAD_TAIL_MEASUREMENT_TYPE_ID,
		                     "int: the default available measurement type for tail - head measurements")
		.def("SetMeasurementTypeName",
		     &Experiment::SetMeasurementTypeName,
		     py::arg("measurementTypeID"),
		     py::arg("name"),
		     R"pydoc(
Sets the name for a measurement type

Args:
    measurementTypeID (int): the type to modify
    name (str): the wanted name

Raises:
    IndexError: if **measurementTypeID** is invalid for this
        Experiment
)pydoc")
		.def("DeleteMeasurementType",
		     &Experiment::DeleteMeasurementType,
		     py::arg("measurementTypeID"),
		     R"pydoc(
Deletes a measurement type

Args:
    measurementTypeID (int): the measurement type to delete

Raises:
    IndexError: if **measurementTypeID** is not valid for Experiment
    ValueError: if measurementTypeID is
        :attr:`HEAD_TAIL_MEASUREMENT_TYPE_ID`
)pydoc")
		.def_property_readonly("AntShapeTypeNames",
		                       &Experiment::AntShapeTypeNames,
		                       "Dict[int,str]: the ant shape type name by their AntShapeTypeID")
		.def("CreateAntShapeType",
		     &Experiment::CreateAntShapeType,
		     py::arg("name"),
		     R"pydoc(
Creates a new Ant shape type

Args:
    name (str): the name of the ant shape type

Returns:
    int: the AntShapeTypeID for the new type
)pydoc")
		.def("SetAntShapeTypeName",
		     &Experiment::SetAntShapeTypeName,
		     py::arg("antShapeTypeID"),
		     py::arg("name"),
		     R"pydoc(
Sets the name for an Ant shape type

Args:
    antShapeTypeID (int): the type to modify
    name (str): the wanted name

Raises:
    IndexError: if **antShapeTypeID** is invalid for this Experiment
)pydoc")
		.def("DeleteAntShapeType",
		     &Experiment::DeleteAntShapeType,
		     py::arg("antShapeTypeID"),
		     R"pydoc(
Deletes an Ant shape type

Args:
    antShapeTypeID (int): the type to delete

Raises:
    IndexError: if **antShapeTypeID** is not valid for Experiment
)pydoc")
		.def_property_readonly("MetaDataKeys",
		                       &Experiment::MetaDataKeys,
		                       "Dict[str,object]: metadata key default value by their unique keys. Object are bool, int, float, str or :class:`Time`")
		.def("SetMetaDataKey",
		     &Experiment::SetMetaDataKey,
		     py::arg("key"),
		     py::arg("defaultValue"),
		     R"pydoc(
Adds or modifies a meta data key

Args:
    key (str): the key to modify
    defaultValue (object): the default value for the key. It will sets
        its type. Must be a boolean, an int, a float, a str or a
        :class:`Time`

Raises:
    RuntimeError: if the following conditions are met: a) **key** is
        already registered, b) **defaultValue** would change the type
        of **key** and c) at least one :class:`Ant` has a value
        registered for **key**
)pydoc")
		.def("DeleteMetaDataKey",
		     &Experiment::DeleteMetaDataKey,
		     py::arg("key"),
		     R"pydoc(
Deletes a meta data key

Args:
    key (str): the key to delete

Raises:
    IndexError: if **key** is not valid for this Experiment
    RuntimeError: if any :class:`Ant` contains timed data for **key**
)pydoc")
		.def("RenameMetaDataKey",
		     &Experiment::RenameMetaDataKey,
		     py::arg("oldKey"),
		     py::arg("newKey"),
		     R"pydoc(
Renames a meta data key

Args:
    oldKey (str): the old name of the key
    newKey (str): the new name for the key

Raises:
    IndexError: if **oldKey** is invalid for the Experiment
    ValueError: if **newKey** is in use for the Experiment
)pydoc")
		.def("IdentificationsAt",
		     &Experiment::IdentificationsAt,
		     py::arg("time"),
		     py::kw_only(),
		     py::arg("removeUnidentifiedAnt") = true,
		     R"pydoc(
Gets AntID <-> TagID correspondances at a given Time

Args:

    time (Time): the wanted Time for the correspondance matching.

    removeUnidentifiedAnt (boolean): if true, :class:`Ant` without an
        :class:`Identification` at **time** will not be part of the
        result. Otherwise the associated tagID value will be 2^32-1.

 Returns:
    Dict[int,int]: TagID indexed by their associated AntID.
 )pydoc")
		.def("CompileTrackingSolver",
		     &Experiment::CompileTrackingSolver,
		     py::arg("collisionsIgnoreZones") = false,
		     R"pydoc(
Compiles a :class:`TrackingSolver` that can be used to identify and
collide ant from raw data.

Returns:
    TrackingSolver: the compiled tracking solver.
)pydoc")
		.def("EnsureAllDataIsLoaded",
		     [](const Experiment & e,bool fixCorruptedData) {
			     EnsureAllDataIsLoaded(e,fixCorruptedData);
		     },
		     "fixCorruptedData"_a = false,
		     R"pydoc(
Ensures all tracking data is loaded.

Ensures that all tracking data, statistics and close-up are available.

Args:
    fixCorruptedData (bool): if True, will silently fix any data
        corruption. This could lead to the loss of large chunck of
        tracking data. Otherwise a RuntimeError is raised summarizing
        all data corruption found.

Raises:
    RuntimeError: if fixCorruptedData is False and some data
        corruption is found.
)pydoc")
		;

	py::register_exception<FixableError>(m,"FixableError",PyExc_RuntimeError);
}
