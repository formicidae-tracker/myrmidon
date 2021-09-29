#include "BindTypes.hpp"

#include <fort/myrmidon/Identification.hpp>

namespace py = pybind11;

void BindIdentification(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<Identification,Identification::Ptr>(m,
	                                               "Identification",
	                                               R"pydoc(Relates tag to Ant

Identifications relates tag to :class:`Ant` with time and geometric
data.

Identification can only be created from an :class:`Experiment` with
:meth:`Experiment.AddIdentification`.

Identifications are bounded in :class:`Time` in the range
[:attr:`Start`,:attr:`End`[. Those attributes can respectively be set
to :meth:`Time.SinceEver` and :meth:`Time.Forever`. Internally
**fort-myrmidon** ensure the validity of all Identifications. It means
that:

* Two Identifications using the same TagValue cannot overlap in Time.
* Two Identifications using targeting the same :class:`Ant` cannot overlap in Time.

If any modification through
:meth:`Experiment.AddIdentification`,ttr:`Start` or :attr:`End` would
violate one of this condition, a :class:`OverlappingIdentification`
exception will be raised.


Identifications also contains geometric information on how the
detected tag is related to the observerd Ant. These are the
translation and rotation of the Ant, in the tag coordinate
reference. Usually, this information is automatically generated from
the manual measurement of type
:obj:`py_fort_myrmidon.HEAD_TAIL_MEASUREMENT_TYPE` made in
**fort-studio**. Alternatively, users can override this behavior by
setting themselves this pose using
:meth:`SetUserDefinedAntPose`. :meth:`ClearUserDefinedAntPose` can be
used to revert to the internally computed pose.

Note:
    Any angle is measured in radians, with a standard mathematical
    convention. Since in image processing the y-axis is pointing from
    the top of the image to the bottom, positive angles appears
    clockwise.

Identifications also contains the information of the physical tag size
used to identify the individual. It can be accessed and set with
:meth:`TagSize` and :meth:`SetTagSize`. The value
:attr:`DEFAULT_TAG_SIZE` (i.e. 0.0) indicates that the
:attr:`Experiment::DefaultTagSize` should be used. Therefore, for most
Ant, this field should be kept to :attr:`DEFAULT_TAG_SIZE`, excepted
for a few individuals, for examples, Queens.
)pydoc")
		.def_readonly_static("DEFAULT_TAG_SIZE",
		                     &Identification::DEFAULT_TAG_SIZE,
		                     "float: a value that marks the Identification to use Experiment.DefaultTagSize")
		.def_property_readonly("TagValue",
		                       &Identification::TagValue,
		                       "int: the associated TagID of this identification")
		.def_property_readonly("TargetAntID",
		                       &Identification::TargetAntID,
		                       "int: the associated AntID of this identification")
		.def_property_readonly("AntPosition",
		                       &Identification::AntPosition,
		                       ":obj:`numpy.ndarray`: Ant center position relative to the tag center. (float64, size[2,1])")
		.def_property_readonly("AntAngle",
		                       &Identification::AntAngle,
		                       "float: orientation difference between the ant orientation and its physical tag, expressed in radians.")
		.def_property("Start",
		              &Identification::Start,
		              &Identification::SetStart,
		              "Time: the first valid Time fort this identification, it can be Time.SinceEver()")
		.def_property("End",
		              &Identification::End,
		              &Identification::SetEnd,
		              "Time: the first invalid Time fort this identification, it can be Time.Forever()")
		.def_property("TagSize",
		              &Identification::TagSize,
		              &Identification::SetTagSize,
		              "float: the Identification tag size in millimeters, could be :attr`:`Identification.DEFAULT_TAG_SIZE` to use :attr:`Experiment.DefaultTagSize`")
		.def("HasDefaultTagSize",
		     &Identification::HasDefaultTagSize,
		     R"pydoc(
    Returns:
        bool: `true` if this Identification use
            `Experiment.DefaultTagSize`, i.e. `self.TagSize ==
            Identification.DEFAULT_TAG_SIZE`
)pydoc")
		.def("HasUserDefinedAntPose",
		     &Identification::HasUserDefinedAntPose,
		     R"pydoc(
    Returns:
        bool: `true` if the ant position relatively by its tag has
            been set by the user
)pydoc")
		.def("SetUserDefinedAntPose",
		     &Identification::SetUserDefinedAntPose,
		     py::arg("antPosition"),
		     py::arg("antAngle"),
		     R"pydoc(
    Sets an user defined ant position relatively to its tag, overiding
    the one computed from manual measurements.

    To revert to the automatically computed ones, use
    `py_fort_myrmidon.Identification.ClearUserDefinedAntPose(self)`

    Args:
        antPosition (numpy.ndarray(numpy.float64(2,1))): the position
            of the ant relatively to the tag center, in the tag
            reference frame
        antAngle (float): the ant angle relatively to the tag angle,
            in radians
 )pydoc")
		.def("ClearUserDefinedAntPose",
		     &Identification::ClearUserDefinedAntPose,
		     R"pydoc(
    Removes any user-defined ant-tag relative geometry data and
    re-enable the one computed from manual measurement in
    `fort-studio`.
)pydoc")
		.def("__str__",[](const Identification::Ptr & i) -> std::string {
			               std::ostringstream oss;
			               oss << *i;
			               return oss.str();
		               });
		;

		py::register_exception<OverlappingIdentification>(m,"OverlappingIdentification",PyExc_RuntimeError);
}
