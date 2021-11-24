#include "BindTypes.hpp"

#include <fort/myrmidon/Video.hpp>

#include <opencv2/videoio.hpp>

using namespace pybind11::literals;

namespace py = pybind11;



void BindVideoFrameData(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<VideoFrameData>(m,"VideoFrameData")
		.def(py::init([](uint32_t position,
		                 const fort::Time & time) {
			              return std::make_unique<VideoFrameData>(VideoFrameData{.Position = position,.Time = time});
		              }),
		     "position"_a,
		     "time"_a)
		.def_readonly("Time",&VideoFrameData::Time)
		.def_readonly("Position",&VideoFrameData::Position)
		.def_readonly("Identified",
		              &VideoFrameData::Identified)
		.def_readonly("Collided",
		              &VideoFrameData::Collided)
		.def_readonly("Trajectories",
		              &VideoFrameData::Trajectories)
		.def_readonly("Interactions",
		              &VideoFrameData::Interactions)
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
		d_VideoCapture = py::module_::import("cv2").attr("VideoCapture");
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
			d_capture = d_VideoCapture("filename"_a = d_segmentIter->AbsoluteFilePath);
			d_capture.attr("set")(int(cv::CAP_PROP_POS_FRAMES),d_segmentIter->Begin);
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

	py::object d_capture,d_VideoCapture;
	fort::myrmidon::VideoSegment::List::const_iterator      d_segmentIter,d_segmentEnd;
	std::vector<fort::myrmidon::VideoFrameData>::const_iterator d_dataIter;
	int32_t d_moviePos;
};



void BindVideoSegment(py::module_ & m) {
	using namespace fort::myrmidon;
	BindVideoFrameData(m);
	py::class_<VideoSegment>(m,"VideoSegment")
		.def(py::init([](SpaceID space) {
			              return std::make_unique<VideoSegment>(VideoSegment{.Space = space});
		              }))
		.def_readonly("Space",
		              &VideoSegment::Space)
		.def_readonly("AbsoluteFilePath",
		              &VideoSegment::AbsoluteFilePath)
		.def_readonly("Begin",
		              &VideoSegment::Begin)
		.def_readwrite("End",
		               &VideoSegment::End)
		.def_readwrite("Data",
		               &VideoSegment::Data,
		               py::return_value_policy::reference_internal)
		.def_static("Match",
		            &Match<IdentifiedFrame::Ptr>)
		.def_static("Match",
		            &Match<CollisionData>)
		.def_static("Match",
		            &Match<AntTrajectory::Ptr>)
		.def_static("Match",
		            &Match<AntInteraction::Ptr>)
		;

	auto list = py::bind_vector<std::vector<VideoSegment>,std::shared_ptr<std::vector<VideoSegment>>>(m,"VideoSegmentList");
	list.def("deepcopy",
	         [](const std::vector<VideoSegment> & self) {
		         return std::make_shared<std::vector<VideoSegment>>(self);
	         });
	py::implicitly_convertible<py::list,
	                           std::vector<VideoSegment>>();

	py::class_<::VideoSequence>(m,"VideoSequence")
		.def(py::init([](const std::shared_ptr<std::vector<VideoSegment>> & l) {
			              return std::make_unique<::VideoSequence>(*l);
		              }),py::keep_alive<1,2>())
		.def("__enter__",&::VideoSequence::Enter)
		.def("__exit__",&::VideoSequence::Exit)
		.def("__iter__",&::VideoSequence::Iter)
		.def("__next__",&::VideoSequence::Next)
		;

}
