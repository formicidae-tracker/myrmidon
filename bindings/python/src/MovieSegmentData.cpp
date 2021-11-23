#include "BindTypes.hpp"

#include <fort/myrmidon/MovieSegmentData.hpp>

namespace py = pybind11;



void BindMovieFrameData(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<MovieFrameData>(m,"MovieFrameData")
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

	py::bind_vector<std::vector<MovieFrameData>>(m,"MovieFrameDataList");
	py::implicitly_convertible<py::list,std::vector<MovieFrameData>>();
}

template <typename T>
void MatchData(fort::myrmidon::MovieSegmentData::List & list,
               const std::vector<T> & data ) {
	fort::myrmidon::MovieSegmentData::MatchData(list,data.begin(),data.end());
}



void BindMovieSegmentData(py::module_ & m) {
	using namespace fort::myrmidon;
	BindMovieFrameData(m);
	py::class_<MovieSegmentData>(m,"MovieSegmentData")
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
		.def_static("MatchData",
		            &MatchData<IdentifiedFrame::Ptr>)
		.def_static("MatchData",
		            &MatchData<CollisionData>)
		.def_static("MatchData",
		            &MatchData<AntTrajectory::Ptr>)
		.def_static("MatchData",
		            &MatchData<AntInteraction::Ptr>)
		;

	py::bind_vector<MovieSegmentData::List>(m,"MovieSegmentDataList");
	py::implicitly_convertible<py::list,
	                           MovieSegmentData::List>();

}
