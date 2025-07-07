#include "BindTypes.hpp"

#include <fort/myrmidon/Video.hpp>

using namespace pybind11::literals;

namespace py = pybind11;



void BindVideoFrameData(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<VideoFrameData>(m,
	                           "VideoFrameData",
	                           R"pydoc(
The tracking data and query results associated with a Video frame.

Note:
    After a call to :meth:`Query.FindMovieSegments` this structure will
    contain no matched data. One must call :meth:`VideoSequence.Match`
    to associate query results with the :class:`VideoFrameData`
    present in the :class:`VideoSegmentList`.

Warning:
    In the unlikely case of a :class:`VideoFrameData` without any
    tracking data ( the movie frame was exported but no tracking was
    reported ), the value of :attr:`VideoFrameData.Time` will be set
    to :meth:`Time.SinceEver` and all other query result field will be
    set to `None`.
)pydoc")
		.def(py::init([](uint32_t position,
		                 const fort::Time & time) {
			              return std::make_unique<VideoFrameData>(VideoFrameData{.Position = position,.Time = time});
		              }),
		     "position"_a,
		     "time"_a)
		.def_readonly("Time",
		              &VideoFrameData::Time,
		              "Time: the video frame acquisition time (if available)")
		.def_readonly("Position",
		              &VideoFrameData::Position,
		              "int: the frame position in the video file")
		.def_readonly("Identified",
		              &VideoFrameData::Identified,
		              "IdentifiedFrame: the ants position (if previously matched)")
		.def_readonly("Collided",
		              &VideoFrameData::Collided,
		              "CollisionFrame: the ants collision (if previously matched)")
		.def_readonly("Trajectories",
		              &VideoFrameData::Trajectories,
		              "List[AntTrajectory]: the trajectories in this frame (if previously matched)")
		.def_readonly("Interactions",
		              &VideoFrameData::Interactions,
		              "List[AntInteraction]: the interactions in this frame (if previously matched)")
		.def("Empty",
		     &VideoFrameData::Empty,
		     R"pydoc(
Indicates the (unlikely) case where no tracking data is associated
with this video frame.

Returns:
    bool: `True` if there is no tracking data (even a timeout / frame drop report) associated with this video frame.
)pydoc")
		;

	py::bind_vector<std::vector<VideoFrameData>>(m,"VideoFrameDataList");
	py::implicitly_convertible<py::list,std::vector<VideoFrameData>>();
}

template <typename T>
void Match(fort::myrmidon::VideoSegment::List & list,
           const std::vector<T> & data ) {
	fort::myrmidon::VideoSegment::Match(list,data.begin(),data.end());
}

class VideoSequence {
public:
	VideoSequence(const fort::myrmidon::VideoSegment::List & list)
		: d_segmentIter(list.begin())
		, d_segmentEnd(list.end()) {
		d_cv2 = py::module_::import("cv2");
		d_capture = py::none();
		d_moviePos = -1;
	}

	VideoSequence & Enter() {
		return *this;
	}

	VideoSequence & Iter() {
		return *this;
	}

	py::tuple Next() {
		if ( d_segmentIter == d_segmentEnd ) {
			throw pybind11::stop_iteration();
		}
		if ( d_moviePos >= int(d_segmentIter->End) ) {
			IncrementSegment();
			return Next();
		}

		if ( d_capture.is_none() == true ) {
			d_capture = d_cv2.attr("VideoCapture")("filename"_a = d_segmentIter->AbsoluteFilePath);
			d_capture.attr("set")(d_cv2.attr("CAP_PROP_POS_FRAMES"),d_segmentIter->Begin);
			d_moviePos = d_segmentIter->Begin - 1;
			d_dataIter = d_segmentIter->Data.begin();
		}
		py::tuple readVal = d_capture.attr("read")();
		d_moviePos++;
		if ( readVal[0].cast<bool>() == false ) {
			IncrementSegment();
			return Next();
		}

		while(d_dataIter != d_segmentIter->Data.end() && d_dataIter->Position < d_moviePos) {
			++d_dataIter;
		}

		if ( d_dataIter != d_segmentIter->Data.end() && d_dataIter->Position == d_moviePos) {
			return py::make_tuple(readVal[1],*d_dataIter);
		} else {
			return py::make_tuple(readVal[1],
			                      fort::myrmidon::VideoFrameData{.Position = uint32_t(d_moviePos),
				                                                     .Time = fort::Time::SinceEver()});
		}
	}

	bool Exit(const py::object & type,const py::object & value,const py::object & traceback) {
		ResetCapture();
		return false;
	}
private:
	void ResetCapture() {
		if ( d_capture.is_none() == false ) {
			d_capture.attr("release")();
			d_capture = py::none();
		}
		d_moviePos = -1;
	}

	void IncrementSegment() {
		ResetCapture();
		++d_segmentIter;
	}

	py::object d_capture,d_cv2;
	fort::myrmidon::VideoSegment::List::const_iterator      d_segmentIter,d_segmentEnd;
	std::vector<fort::myrmidon::VideoFrameData>::const_iterator d_dataIter;
	int32_t d_moviePos;
};

void BindVideoSegment(py::module_ &m) {
	using namespace fort::myrmidon;
	BindVideoFrameData(m);
	py::class_<VideoSegment> c(
	    m,
	    "VideoSegment",
	    R"pydoc(

A VideoSegment represents a part of a Video file with its associated
tracking data.

VideoSegment are most often queried with
:meth:`Query.FindVideoSegments`. Once queried, they are blank, i.e. no
query results will appears in :attr:`VideoSegment.Data`. One would
call :meth:`Match` to associate queries results with these
segments. Finally a :class:`VideoSequence` context manager can be used
to iterate over each video frame of the :class:`VideoSegmentList`.

Note:
    :meth:`Query.FindVideoSegment`, :meth:`Match` and
    :class:`VideoSequence` use a :class:`VideoSegmentList` as return
    value or arguments. Indeed it could happen that the desired
    sequence of images span over multiple video file.

Example:
    .. code-block:: python

        import fort_myrmidon as fm

        e = fm.Experiment.Open("file.myrmidon")

        # note it would be extremly computationally intensive to
        # iterate over the whole experiment, we select a time region.
        start = fm.Time.Parse('2019-11-02T20:00:00.000Z')
        end = start.Add(30 * fm.Duration.Second)

        ## step 0: make some query on the experiment
        trajectories = fm.Query.ComputeAntTrajectories(e,start=start,end=end)

        ## step 1: look up segments bet
        segments = fm.Query.FindVideoSegments(e,
                                              space = 1,
                                              start = start,
                                              end = end)

        ## step 2: match queried data with video frames
        fm.VideoSegment.Match(segments,trajectories)

        ## step 3: iterate over all video frame and matched data
        with fm.VideoSequence(segments) as sequence:
            for frame,data in sequence:
                ## step 3.1 on each cv2.Mat `frame` do something based
                ## on `data`
                pass

)pydoc"
	);
	auto list = py::bind_vector<
	    std::vector<VideoSegment>,
	    std::shared_ptr<std::vector<VideoSegment>>>(
	    m,
	    "VideoSegmentList",
	    R"pydoc(
An opaque list of :class:`VideoSegment`.

It works just as a :class:`list` of :class:`VideoSegment`.

)pydoc"
	);
	list.def(
	    "deepcopy",
	    [](const std::vector<VideoSegment> &self) {
		    return std::make_shared<std::vector<VideoSegment>>(self);
	    },
	    R"pydoc(
Performs a deepcopy of the list.

The main purpose is for the unittest implementation.
)pydoc"
	);

	py::implicitly_convertible<py::list, std::vector<VideoSegment>>();

	c.def(py::init([](SpaceID space) {
		return std::make_unique<VideoSegment>(VideoSegment{.Space = space});
	}));
	c.def_readonly(
	    "Space",
	    &VideoSegment::Space,
	    "int: the :class:`Space` the segment belongs to"
	);
	c.def_readonly(
	    "AbsoluteFilePath",
	    &VideoSegment::AbsoluteFilePath,
	    "str: the absolute filepath to the video file"
	);
	c.def_readonly(
	    "Begin",
	    &VideoSegment::Begin,
	    "int: the first video frame position in the video file corresponding "
	    "to the segment"
	);
	c.def_readwrite(
	    "End",
	    &VideoSegment::End,
	    "int: the last+1 video frame postion in the video file corresponding "
	    "to the segment"
	);
	c.def_readwrite(
	    "Data",
	    &VideoSegment::Data,
	    py::return_value_policy::reference_internal,
	    "List[VideoFrameData]: matched query result with the video frame in "
	    "the list. If no tracking data is associated with a given frame, there "
	    "will be no corresponding object in this field."
	);
	c.def_static(
	    "Match",
	    &Match<IdentifiedFrame::Ptr>,
	    "segments"_a,
	    "identifiedFrames"_a,
	    R"pydoc(
Matches :class:`IdentifiedFrame` with a :class:`VideoSegmentList`

Args:
    segments (VideoSegmentList): the segments to associate data with
    identifiedFrames (List[IdentifiedFrame]): the IdentifiedFrames to match with `segments`.

Raises:
    ValueError: if the all segments are not from the same :class:`Space`
)pydoc"
	);
	c.def_static(
	    "Match",
	    &Match<CollisionData>,
	    "segments"_a,
	    "collisionData"_a,
	    R"pydoc(
Matches :class:`CollisionData` with a :class:`VideoSegmentList`

Args:
    segments (VideoSegmentList): the segments to associate data with
    collisionData (List[CollisionData]): the CollisionData to match with `segments`.

Raises:
    ValueError: if the all segments are not from the same :class:`Space`
)pydoc"
	);
	c.def_static(
	    "Match",
	    &Match<AntTrajectory::Ptr>,
	    "segments"_a,
	    "trajectories"_a,
	    R"pydoc(
Matches :class:`AntTrajectory` with a :class:`VideoSegmentList`

Args:
    segments (VideoSegmentList): the segments to associate data with
    trajectories (List[AntTrajectory]): the AntTrajectories to match with `segments`.

Raises:
    ValueError: if the all segments are not from the same :class:`Space`
)pydoc"
	);
	c.def_static(
	    "Match",
	    &Match<AntInteraction::Ptr>,
	    "segments"_a,
	    "interactions"_a,
	    R"pydoc(
Matches :class:`AntInteraction` with a :class:`VideoSegmentList`

Args:
    segments (VideoSegmentList): the segments to associate data with
    interactions (List[AntInteraction]): the AntInteractions to match with `segments`.

Raises:
    ValueError: if the all segments are not from the same :class:`Space`
)pydoc"
	);

	py::class_<::VideoSequence>(
	    m,
	    "VideoSequence",
	    R"pydoc(
A contextmanager for iterating over Video Frame and matched data.

Examples:
    .. code-block:: python

        import fort_myrmidon as fm

        ## will iterate over all video frame of space 1
        segments = fm.Query.FindVideoSegments(e,space = 1)
        with fm.VideoSequence(segments) as sequence:
            for frame, data in sequence:
                pass

)pydoc"
	)
	    .def(
	        py::init([](const std::shared_ptr<std::vector<VideoSegment>> &l) {
		        return std::make_unique<::VideoSequence>(*l);
	        }),
	        py::keep_alive<1, 2>()
	    )
	    .def("__enter__", &::VideoSequence::Enter)
	    .def("__exit__", &::VideoSequence::Exit)
	    .def("__iter__", &::VideoSequence::Iter)
	    .def("__next__", &::VideoSequence::Next);
}
