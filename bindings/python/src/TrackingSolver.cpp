#include "BindMethods.hpp"

#include <fort/myrmidon/TrackingSolver.hpp>

void BindTrackingSolver(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<TrackingSolver>(m,"TrackingSolver",
	                           R"pydoc(
    A TrackingSolver is used to Identify and Collide raw tracking data
)pydoc")
		.def("IdentifyAnt",
		     &TrackingSolver::IdentifyAnt,
		     py::arg("tagID"),
		     py::arg("time"),
		     R"pydoc(
    Identifies an Ant from a tagID at a given time

    Args:
        tagID (int): the tagID we want to identify for
        time (py_fort_myrmidon.Time): the time at wich we want to identify

    Returns:
        int: the AntID which is identified by tagID at time, or 0 if
            no ant is identified by tagID at time.
)pydoc")
		.def("IdentifyFrame",
		     [](const TrackingSolver & self,
		        const fort::hermes::FrameReadout & frame,
		        SpaceID spaceID) {
			     auto res = std::make_shared<IdentifiedFrame>();
			     self.IdentifyFrame(*res,frame,spaceID);
			     return res;
		     },
		     py::arg("frame"),
		     py::arg("spaceID"),
		     R"pydoc(
    Identifies Ant in a raw frame readout

    Args:
        frame (py_fort_hermes.FrameReadout): the raw data to identify
        spaceID (int): the SpaceID associated with frame

    Returns:

        py_fort_myrmidon.IdentifiedFrame: the Ant Identification
            without zone detection.
)pydoc")
		.def("CollideFrame",
		     [](const TrackingSolver & self,
		        IdentifiedFrame & frame) {
			     auto res = std::make_shared<CollisionFrame>();
			     self.CollideFrame(frame,*res);
			     return res;
		     },
		     py::arg("frame"),
		     R"pydoc(
    Runs Ant zone detection and collision detection on an IdentifiedFrame

    Args:
        frame (py_fort_myrmidon.IdentifiedFrame): the IdentifiedFrame
            containing position data for the Ant. It will be modified
            to hold the current detected zone for the Ant.
    Returns:
        py_fort_myrmidion.CollisionFrame: collision founds for the
            IdentifiedFrame
)pydoc")
		;
}
