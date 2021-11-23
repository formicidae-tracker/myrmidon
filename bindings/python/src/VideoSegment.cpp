#include "BindTypes.hpp"

#include <fort/myrmidon/MovieSegmentData.hpp>

namespace py = pybind11;



void BindVideoFrameData(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<MovieFrameData>(m,"VideoFrameData")
		.def_readonly("Time",&MovieFrameData::Time)
		.def_readonly("FramePosition",&MovieFrameData::FramePosition)
		.def_readonly("Identified",
		              &MovieFrameData::Identified)
		.def_readonly("Collided",
		              &MovieFrameData::Collided)
		.def_readonly("Trajectories",
		              &MovieFrameData::Trajectories)
		.def_readonly("Interactions",
		              &MovieFrameData::Interactions)
		;

	py::bind_vector<std::vector<MovieFrameData>>(m,"VideoFrameDataList");
	py::implicitly_convertible<py::list,std::vector<MovieFrameData>>();
}

template <typename T>
void Match(fort::myrmidon::MovieSegmentData::List & list,
           const std::vector<T> & data ) {
	fort::myrmidon::MovieSegmentData::MatchData(list,data.begin(),data.end());
}



void BindVideoSegment(py::module_ & m) {
	using namespace fort::myrmidon;
	BindVideoFrameData(m);
	py::class_<MovieSegmentData>(m,"VideoSegment")
		.def(py::init([](SpaceID space) {
			              return std::make_unique<MovieSegmentData>(MovieSegmentData{.Space = space});
		              }))
		.def_readonly("Space",
		              &MovieSegmentData::Space)
		.def_readonly("AbsoluteFilePath",
		              &MovieSegmentData::AbsoluteFilePath)
		.def_readonly("Begin",
		              &MovieSegmentData::Begin)
		.def_readonly("End",
		              &MovieSegmentData::End)
		.def_readwrite("Data",
		               &MovieSegmentData::Data,
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

	py::bind_vector<MovieSegmentData::List>(m,"VideoSegmentList");
	py::implicitly_convertible<py::list,
	                           MovieSegmentData::List>();

}
