#include "BindMethods.hpp"

#include <fort/myrmidon/Types.hpp>
#include <fort/time/Time.hpp>

namespace py = pybind11;

template<class> inline constexpr bool always_false_v = false;


void BindComputedMeasurement(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<ComputedMeasurement>(m,
	                                "ComputedMeasurement",
	                                R"pydoc(
    A manual `fort-studio` measurement and its estimated value in
    millimeters.
)pydoc")
		.def_readonly("Time",
		              &ComputedMeasurement::Time,
		              "(py_fort_myrmidon.Time): the Time of the close-up this measurement.")
		.def_readonly("LengthMM",
		              &ComputedMeasurement::LengthMM,
		              "(float): its length in millimeters."
		              )
		.def_readonly("LengthPixel",
		              &ComputedMeasurement::LengthPixel,
		              "(float): its length in pixel.")
		;
}

void BindTagStatistics(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<TagStatistics>(m,
	                          "TagStatistics",
	                          "Tag detection statistics for a given TagID")
		.def_readonly("ID",
		              &TagStatistics::ID,
		              "(int): the TagID it refers to")
		.def_readonly("FirstSeen",
		              &TagStatistics::FirstSeen,
		              "(py_fort_myrmidon.Time): first time the tag was seen")
		.def_readonly("LastSeen",
		              &TagStatistics::LastSeen,
		              "(py_fort_myrmidon.Time): last time the tag was seen")
		.def_property_readonly("Counts",
		                       [](const TagStatistics & ts) -> const TagStatistics::CountVector & {
			                       return ts.Counts;
		                       },
		                       py::return_value_policy::reference_internal,
		                       R"pydoc(
    (numpy.ndarray(numpy.int64[1xN])): histogram of gaps of
        non-detection of this tag.
)pydoc")
		;

}
void BindIdentifiedFrame(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<IdentifiedFrame,std::shared_ptr<IdentifiedFrame> >(m,
	                                                              "IdentifiedFrame",
	                                                              R"pydoc(
    An IdentifiedFrame holds ant detection information associated with
one video frame.
)pydoc")
		.def_readonly("FrameTime",
		              &IdentifiedFrame::FrameTime,
		              "(py_fort_myrmidon.time): acquisition time of the frame")
		.def_readonly("Space",
		              &IdentifiedFrame::Space,
		              "(int): the SpaceID of the Space this frame comes from")
		.def_readonly("Height",
		              &IdentifiedFrame::Height,
		              "(int): height in pixel of the original video frame")
		.def_readonly("Width",
		              &IdentifiedFrame::Width,
		              "(int): width in pixel of the original video frame")
		.def_property_readonly("Positions",
		                       []( const IdentifiedFrame & f ) -> const IdentifiedFrame::PositionMatrix & {
			                       return f.Positions;
		                       },py::return_value_policy::reference_internal,
		                       R"pydoc(
    (numpy.ndarray(numpy.float64[N,5])): a N  array of
        (antID,x,y,angle,zone) row vectors for each detected ant in the
        frame. if Zone is undefined or non-computed, `zone` will be 0.
 )pydoc")
		.def("Contains",
		     &IdentifiedFrame::Contains,
		     py::arg("antID"),
		     R"pydoc(
   Tests if the frame contains a given antID

   Args:
       antID (int): the AntID to test for.
   Returns:
       bool: `true` if antID is present in this IdentifiedFrame
)pydoc")
		.def("At",
		     &IdentifiedFrame::At,
		     py::arg("index"),
		     R"pydoc(
   Returns ant information for a given row.

   Args:
       index (int): the index in Positions
   Returns:
       Tuple[int,numpy.ndarray(numpy.float64[3,1]),int] : the AntID, a
           vector with its (x,y,theta) position, and its current zone.
   Raises:
       IndexError: if index >= len(Positions)
)pydoc")
		;
}

void BindCollisionFrame(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<Collision>(m,
	                      "Collision",
	                      "A Collision describe an instantaneous contact between two ants")
		.def_readonly("IDs",
		              &Collision::IDs,
		              "(Tuple[int,int]) : the AntIDs of the two ants")
		.def_readonly("Zone",
		              &Collision::Zone,
		              "(int) : the ZoneID where the collision happens")
		.def_property_readonly("Types",
		                       [](const Collision & c) -> const InteractionTypes & {
			                       return c.Types;
		                       },
		                       py::return_value_policy::reference_internal,
		                       R"pydoc(

    (numpy.ndarray(numpy.int32[N,2])): an N row array describing the
        colliding AntShapeTypeID. First column refers to shape type of
        the first Ant, which are colliding with a part of the second
        Ant in the second column.
)pydoc")
		;

	py::class_<CollisionFrame,std::shared_ptr<CollisionFrame> >(m,
	                                                            "CollisionFrame",
	                                                            "A CollisionFrame regroups all Collision that happen in a video frame")
		.def_readonly("FrameTime",
		              &CollisionFrame::FrameTime,
		              "(py_fort_myrmidon.Time) : the Time the video frame was acquired")
		.def_readonly("Space",
		              &CollisionFrame::Space,
		              "(int) : the Space the video frame belongs to")
		.def_readonly("Collisions",
		              &CollisionFrame::Collisions,
		              "(List[py_fort_myrmidon.Collision]): the list of Collision in the frame")
		;
}

void BindAntTrajectory(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<AntTrajectory,std::shared_ptr<AntTrajectory> >(m,
	                                                          "AntTrajectory",
	                                                          "An Ant trajectory represents a continuous spatial trajectory of an Ant")
		.def_readonly("Ant",
		              &AntTrajectory::Ant,
		              "(int) : the AntID of the Ant")
		.def_readonly("Space",
		              &AntTrajectory::Space,
		              "(int) : the SpaceID where the trajectory takes place.")
		.def_readonly("Start",
		              &AntTrajectory::Start,
		              "(py_fort_myrmidon.Time): the starting time of this trajectory.")
		.def("End",
		     &AntTrajectory::End,
		     R"pydoc(
    Computes the End time of the AntTrajectory.

    Returns:
        py_fort_myrmidon.Time: the last Time found in this trajectory

)pydoc")
		.def_property_readonly("Positions",
		                       [](const AntTrajectory & t) -> const Eigen::Matrix<double,Eigen::Dynamic,5> & {
			                       return t.Positions;
		                       },
		                       py::return_value_policy::reference_internal,
		                       R"pydoc(
    (numpy.ndarray(numpy.float64[N,5])) : a N row array of
        position. Columns are (t,x,y,angle,zone), where t is the
        offset from Start in seconds.
)pydoc")
		;

}

void BindAntInteraction(py::module_ & m) {
	using namespace fort::myrmidon;

	py::class_<AntTrajectorySegment>(m,
	                                 "AntTrajectorySegment",
	                                 R"pydoc(
Represents a section  of an :class:`AntTrajectory`.
)pydoc")
		.def_readonly("Trajectory",
		              &AntTrajectorySegment::Trajectory,
		              "Trajectory: the AntTrajectory it refers to.")
		.def_readonly("Begin",
		              &AntTrajectorySegment::Begin,
		              "int: the first index in Trajectory this segment refers to.")
		.def_readonly("End",
		              &AntTrajectorySegment::End,
		              "int: the last index+1 in Trajectory this segment refers to.")
		.def("StartTime",
		     &AntTrajectorySegment::StartTime,
		     R"pydoc(
Computes the starting Time of the AntTrajectorySegment

Returns:
    Time: the starting Time of the AntTrajectorySegment.
)pydoc")
		.def("EndTime",
		     &AntTrajectorySegment::EndTime,
		     R"pydoc(
Computes the ending Time of the AntTrajectorySegment

Returns:
    Time: the ending Time of the AntTrajectorySegment.
)pydoc")
;
	py::class_<AntTrajectorySummary>(m,
	                                 "AntTrajectorySummary",
	                                 R"pydoc(
Represents a summary  of an :class:`AntTrajectory` section.
)pydoc")
		.def_readonly("Mean",
		              &AntTrajectorySummary::Mean,
		              "Trajectory: the AntTrajectory it refers to.")
		.def_readonly("Zones",
		              &AntTrajectorySummary::Zones,
		              "List[int]: all the ZoneID the trajectory crossed.")
;


	py::class_<AntInteraction,std::shared_ptr<AntInteraction>>(m,
	                                                           "AntInteraction",
	                                                           "Represent an interaction between two Ant")
		.def_readonly("IDs",
		              &AntInteraction::IDs,
		              "(Tuple[int,int]): the AntIDs of the two Ant interaction")
		.def_property_readonly("Types",
		                       [](const AntInteraction & i) -> const InteractionTypes & {
			                       return i.Types;
		                       },
		                       py::return_value_policy::reference_internal,
		                       R"pydoc(
    (numpy.ndarray(numpy.int32(N,2))): The AntShapeTypeID that were in
        contact during the interaction. Any body part interacting at
        least at one instant will add a row in this array. The first
        column refers to the first Ant, and the second column to the
        other Ant. Therefore, there is a difference between a (1,2)
        and a (2,1) interaction.
  )pydoc")
		.def_readonly("Trajectories",
		              &AntInteraction::Trajectories,
		              "Union[Tuple[AntTrajectorySegment,AntTrajectorySegment],Tuple[AntTrajectorySummary,AntTrajetorySummary]]: The two section of trajectory for the two Ant during this interaction. Summarized or full.")
		.def_readonly("Start",
		              &AntInteraction::Start,
		              "Time: the start Time of the interaction.")
		.def_readonly("End",
		              &AntInteraction::End,
		              "Time: the end Time of the interaction.")
		.def_readonly("Space",
		              &AntInteraction::Space,
		              "int: the SpaceID of the Space the interaction takes place.")
		;
}

void BindExperimentDataInfo(py::module_ & m) {
	using namespace fort::myrmidon;

	py::class_<TrackingDataDirectoryInfo>(m,
	                                      "TrackingDataDirectoryInfo",
	                                      "Tracking Data informations summary for a Tracking Data Directory.")
		.def_readonly("URI",
		              &TrackingDataDirectoryInfo::URI,
		              "(str): The internal URI for the Tracking Data Directory")
		.def_readonly("AbsoluteFilePath",
		              &TrackingDataDirectoryInfo::AbsoluteFilePath,
		              "(str): Absolute filepath of the Tracking Data Directory on the system")
		.def_readonly("Frames",
		              &TrackingDataDirectoryInfo::Frames,
		              "(int): Number of frames found in this Tracking Data Directory")
		.def_readonly("Start",
		              &TrackingDataDirectoryInfo::Start,
		              "(py_fort_myrmidon.Time): The Time of the first frame found in this Tracking Data Directory.")
		.def_readonly("End",
		              &TrackingDataDirectoryInfo::End,
		              "(py_fort_myrmidon.Time): The Time plus a nanosecond, of the last frame found in This Tracking Data Directory")
		;

	py::class_<SpaceDataInfo>(m,
	                          "SpaceDataInfo",
	                          "Tracking Data information summary for a Space.")
		.def_readonly("URI",
		              &SpaceDataInfo::URI,
		              "The internal URI for the Space")
		.def_readonly("Name",
		              &SpaceDataInfo::Name,
		              "The name of the space")
		.def_readonly("Frames",
		              &SpaceDataInfo::Frames,
		              "(int): Total number of frame found in this Space")
		.def_readonly("Start",
		              &SpaceDataInfo::Start,
		              "(py_fort_myrmidon.Time): the Time of the first frame available in this space.")
		.def_readonly("End",&SpaceDataInfo::End,
		              "(py_fort_myrmidon.Time): the Time of the last frame available in this space.")
		.def_readonly("TrackingDataDirectories",
		              &SpaceDataInfo::TrackingDataDirectories,
		              "(List[py_fort_myrmidon.Time]): The TrackingDataDirectoryInfo present in this Space")
		;

	py::class_<ExperimentDataInfo>(m,
	                               "ExperimentDataInfo",
	                               "Tracking Data information summary for an Experiment")
		.def_readonly("Frames",
		              &ExperimentDataInfo::Frames,
		              "(int): Total number of Frames accessible in this Experiment.")
		.def_readonly("Start",
		              &ExperimentDataInfo::Start,
		              "(py_fort_myrmidon.Time): the Time of the first frame available in this Experiement.")
		.def_readonly("End",
		              &ExperimentDataInfo::End,
		              "(py_fort_myrmidon.Time): the Time of the first frame available in this Experiement.")
		.def_readonly("Spaces",
		              &ExperimentDataInfo::Spaces,
		              "(Dict[int,py_fort_myrmidon.Time]): the SpaceDataInfo indexed by SpaceId.")
		;
}

void BindTypes(py::module_ & m) {
	BindTime(m);

	using namespace fort::myrmidon;
	py::class_<AntStaticValue>(m,"AntStaticValue")
		.def(py::init<bool>())
		.def(py::init<int>())
		.def(py::init<double>())
		.def(py::init<std::string>())
		.def(py::init<fort::Time>())
		;
	py::implicitly_convertible<bool,AntStaticValue>();
	py::implicitly_convertible<int,AntStaticValue>();
	py::implicitly_convertible<double,AntStaticValue>();
	py::implicitly_convertible<std::string,AntStaticValue>();
	py::implicitly_convertible<fort::Time,AntStaticValue>();
	py::enum_<AntMetaDataType>(m,"AntMetaDataType")
		.value("BOOL",AntMetaDataType::BOOL)
		.value("INT",AntMetaDataType::INT)
		.value("DOUBLE",AntMetaDataType::DOUBLE)
		.value("STRING",AntMetaDataType::STRING)
		.value("TIME",AntMetaDataType::TIME)
		;
	BindColor(m);

	BindTagStatistics(m);
	BindComputedMeasurement(m);
	BindIdentifiedFrame(m);
	BindCollisionFrame(m);
	BindAntTrajectory(m);
	BindAntInteraction(m);
	BindExperimentDataInfo(m);

	m.def("FormatAntID",
	      &fort::myrmidon::FormatAntID,
	      py::arg("antID"),
	      R"pydoc(
    Formats an AntID to the conventional format.

    Args:
        antID (int): the AntID to format
    Returns:
        str: antID formatted in a string
)pydoc");
	m.def("FormatTagID",
	      &fort::myrmidon::FormatTagID,
	      py::arg("tagID"),
	      R"pydoc(
    Formats a TagID to the conventional format.

    Args:
        tagID (int): the TagID to format
    Returns:
        str: tagID formatted in a string
)pydoc");

}
