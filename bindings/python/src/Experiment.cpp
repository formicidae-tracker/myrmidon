#include <pybind11/pybind11.h>

#include <fort/myrmidon/Experiment.hpp>

namespace py = pybind11;

void BindExperiment(py::module_ & m) {
	using namespace fort::myrmidon;

	py::enum_<fort::tags::Family>(m,"TagFamily")
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
	                       "An Experiment is the main entry point of `fort-myrmidon`")
		.def(py::init(&fort::myrmidon::Experiment::Create),
		     py::arg("filepath"),
		     R"pydoc(
    Initialize an experiment from a specified filesystem
    location. This location will be used to determine relative path to
    the tracking data.

    Args:
        filepath (str): the wanted filesystem path to the experiment.
    Returns:
        py_fort_myrmidon.Experiment: a new empty Experiment associated with filepath
)pydoc")
		.def_static("Open",
		            &Experiment::Open,
		            py::arg("filepath"),
		            R"pydoc(
    Opens an existing Experiment on the filesystem

    Args:
        filepath (str): the filepath to open.
    Returns:
        py_fort_myrmidon.Experiment: the Experiment located at filepath
    Raises:
        Error: if filepath does not contains a valid Experiment or
            associated TrackingData is not accessible.
)pydoc")
		.def_static("OpenDataLess",
		            &Experiment::OpenDataLess,
		            py::arg("filepath"),
		            R"pydoc(
    Opens an existing Experiment on the filesystem in dataless mode.

    In dataless mode, no associated tracking data will be opened, but
    a TrackingSolver can be used to identify Ants in live tracking
    situation.

    Args:
        filepath (str): the filepath to open.
    Returns:
        py_fort_myrmidon.Experiment: the Experiment located at filepath
    Raises:
        Error: if filepath does not contains a valid Experiment
)pydoc")

		.def("Save",
		     &Experiment::Save,
		     py::arg("filepath"),
		     R"pydoc(
    Saves the experiment on the filesystem.

    Args:
        filepath: the filepath to save the experiment to.
    Raises:
        ValueError: if filepath would change the directory of the
            Experiment on the filesystem.
)pydoc")
		.def_property_readonly("AbsoluteFilePath",
		                       &Experiment::AbsoluteFilePath,
		                       " (str): the absolute filepath of the Experiment")
		.def("CreateSpace",
		     &Experiment::CreateSpace,
		     py::arg("name"),
		     py::return_value_policy::reference_internal,
		     R"pydoc(
    Creates a new Space in this Experiment.

    Args:
        name (str): the name for the new space
    Returns:
        py_fort_myrmidon.Space: the newly created Space
)pydoc")
		.def("DeleteSpace",
		     &Experiment::DeleteSpace,
		     py::arg("spaceID"),
		     R"pydoc(
    Deletes a Space from this Experiment.

    Args:
        spaceID (str): the spaceID of this space
    Raises:
        IndexError: if spaceID is not a valid SpaceID for this Experiment.
)pydoc")
		.def_property_readonly("Spaces",
		                       &Experiment::Spaces,
		                       py::return_value_policy::reference_internal,
		                       " (Dict[int,py_fort_myrmidon.Space]): this Experiment space indexed by their SpaceID")

		.def("AddTrackingDataDirectory",
		     &Experiment::AddTrackingDataDirectory,
		     py::arg("spaceID"),
		     py::arg("filepath"),
		     R"pydoc(
    Adds a tracking data directory to the Experiment.

    Args:
        spaceID (int): the space to add the tracking data directory
            to.
        filepath (str): the filepath to the tracking data directory.
    Returns:
        str: the relative path from self.AbsoluteFilePath to filepath,
            that will be the URI to identify the tracking data
            directory.
    Raises:
        IndexError: if spaceID is not valid for this Experiment
        RuntimeError: if filepath is not a valid tracking data directory.
        RuntimeError: if the data will overlap in time with another directory in the same space
        RuntimeError: if the data is used by another space
        ValueError: if the tag family does not match with other directory in the experiment
)pydoc")
		.def("RemoveTrackingDataDirectory",
		     &Experiment::RemoveTrackingDataDirectory,
		     py::arg("URI"),
		     R"pydoc(
    Removes a tracking data directory from the Experiment.

    Args:
        URI (str): the URI that identifies the tracking data directory
    Raises:
        IndexError: if URI does not identifies a tracking data
            directory in this experiment.
)pydoc")
		.def("CreateAnt",
		     &Experiment::CreateAnt,
		     R"pydoc(
    Creates a new Ant in the Experiment.

    Returns:
        py_fort_myrmidon.Ant: the newly created Ant
)pydoc")
		.def("DeleteAnt",
		     &Experiment::DeleteAnt,
		     py::arg("antID"),
		     R"pydoc(
    Deletes an Ant from the Experiment

    Args:
        antID (int): the AntID of the Ant to remove
    Raises:
        IndexError: if antID is invalid for the experiment
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
        start (py_fort_myrmidon.Time): the first valid Time for the
        Identification
        end (py_fort_myrmidon.Time): the first invalid Time for the
        Identification

    Returns:
        py_fort_myrmidon.Identification: the newly created
            Identification

    Raises:
        IndexError: if antID is not valid for the Experiment

        RuntimeError: if this identification would overlap in time
                      with another one, either for a given antID or
                      tagID.
)pydoc")
		.def("DeleteIdentification",
		     &Experiment::DeleteIdentification,
		     py::arg("identification"),
		     R"pydoc(
    Deletes an Identification from this Experiment

    Args:
        identification (py_fort_myrmidon.Identification):
            the Identification to remove

    Raises:

        ValueError: if identification is not a valid Identification
                    object from this Experiment.
)pydoc")
		.def("FreeIdentificationRangeAt",
		     &Experiment::FreeIdentificationRangeAt,
		     py::arg("tagID"),
		     py::arg("time"),
		     R"pydoc(
    Computes a valid Identification time range for a tag

    Args:
        tagID (int): the tagID to query for
        time (py_fort_myrmidon.Time): a time that must be contained in
            the result.

    Returns:
        py_fort_myrmidon.Time: the first valid time, can be
            Time.SinceEver()
        py_fort_myrmidon.Time: the first invalid time, can be
            Time.Forever()

   Raises:
        RuntimeError: if tagID already identifies an Ant at time
)pydoc")
		.def_property("Name",
		              &Experiment::Name,
		              &Experiment::SetName,
		              " (str): the name of the Experiment")
		.def_property("Author",
		              &Experiment::Author,
		              &Experiment::SetAuthor,
		              " (str): the author of the Experiment")
		.def_property("Comment",
		              &Experiment::Comment,
		              &Experiment::SetComment,
		              " (str): a comment about the Experiment")
		.def_property_readonly("Family",
		                       &Experiment::Family,
		                       " (py_fort_myrmidon.TagFamily): the TagFamily used in the Experiment")
		.def_property("DefaultTagSize",
		              &Experiment::DefaultTagSize,
		              &Experiment::SetDefaultTagSize,
		              " (float): the default tag size in mm used in the Experiment")

		;
}
