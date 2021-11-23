#include "BindTypes.hpp"

#include <fort/myrmidon/Query.hpp>
#include <fort/myrmidon/Matchers.hpp>
#include <fort/myrmidon/Experiment.hpp>
#include <fort/myrmidon/MovieSegmentData.hpp>

namespace py = pybind11;

#define check_py_interrupt() do { \
		if ( PyErr_CheckSignals() != 0 ) { \
			throw py::error_already_set(); \
		} \
	} while(0)

py::list QueryIdentifyFrames(const fort::myrmidon::Experiment & experiment,
                             fort::Time start,
                             fort::Time end,
                             bool singleThreaded,
                             bool computeZones) {
	py::list res;
	fort::myrmidon::Query::IdentifyFramesArgs args;
	args.Start = start;
	args.End = end;
	args.SingleThreaded = singleThreaded;
	args.ComputeZones = computeZones;
	args.AllocationInCurrentThread = false;
	fort::myrmidon::Query::IdentifyFramesFunctor(experiment,
	                                             [&res](const fort::myrmidon::IdentifiedFrame::Ptr & f) {
		                                             res.append(f);
		                                             check_py_interrupt();
	                                             },
	                                             args);
	return res;
}

py::list QueryCollideFrames(const fort::myrmidon::Experiment & experiment,
                            fort::Time start,
                            fort::Time end,
                            bool collisionsIgnoreZones,
                            bool singleThreaded) {
	py::list res;
	fort::myrmidon::Query::CollideFramesArgs args;
	args.Start = start;
	args.End = end;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = false;
	args.CollisionsIgnoreZones = collisionsIgnoreZones;
	fort::myrmidon::Query::CollideFramesFunctor(experiment,
	                                            [&res](const fort::myrmidon::CollisionData & d) {
		                                             res.append(d);
		                                             check_py_interrupt();
	                                             },
	                                             args);
	return res;
}


py::list QueryComputeAntTrajectories(const fort::myrmidon::Experiment & experiment,
                                     fort::Time start,
                                     fort::Time end,
                                     fort::Duration maximumGap,
                                     const fort::myrmidon::Matcher::Ptr & matcher,
                                     bool computeZones,
                                     bool singleThreaded) {

	py::list res;
	fort::myrmidon::Query::ComputeAntTrajectoriesArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.ComputeZones = computeZones;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = false;
	fort::myrmidon::Query::ComputeAntTrajectoriesFunctor(experiment,
	                                                     [&res](const fort::myrmidon::AntTrajectory::Ptr & t) {
		                                                     res.append(t);
		                                                     check_py_interrupt();
	                                                     },
	                                                     args);
	return res;
}

py::tuple QueryComputeAntInteractions(const fort::myrmidon::Experiment & experiment,
                                      fort::Time start,
                                      fort::Time end,
                                      fort::Duration maximumGap,
                                      const fort::myrmidon::Matcher::Ptr & matcher,
                                      bool collisionsIgnoreZones,
                                      bool reportFullTrajectories,
                                      bool singleThreaded) {

	py::list trajectories;
	py::list interactions;

	fort::myrmidon::Query::ComputeAntInteractionsArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.ReportFullTrajectories = reportFullTrajectories;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = false;
	args.CollisionsIgnoreZones = collisionsIgnoreZones;
	fort::myrmidon::Query::ComputeAntInteractionsFunctor(experiment,
	                                                     [&trajectories](const fort::myrmidon::AntTrajectory::Ptr & t) {
		                                                     trajectories.append(t);
		                                                     check_py_interrupt();
	                                                     },
	                                                     [&interactions](const fort::myrmidon::AntInteraction::Ptr & i) {
		                                                     interactions.append(i);
		                                                     check_py_interrupt();
	                                                     },
	                                                     args);
	return py::make_tuple(trajectories,interactions);
}


fort::myrmidon::MovieSegmentData::List
FindMovieSegment(const fort::myrmidon::Experiment & e,
                 fort::myrmidon::SpaceID space,
                 const fort::Time & start,
                 const fort::Time & end) {
	std::vector<fort::myrmidon::MovieSegmentData> segments;
	fort::myrmidon::Query::FindMovieSegments(e,segments,space,start,end);
	return segments;
}



void BindQuery(py::module_ & m) {
	using namespace pybind11::literals;

	fort::myrmidon::Query::IdentifyFramesArgs identifyArgs;
	fort::myrmidon::Query::CollideFramesArgs collideArgs;
	fort::myrmidon::Query::ComputeAntTrajectoriesArgs trajectoryArgs;
	fort::myrmidon::Query::ComputeAntInteractionsArgs interactionArgs;

	py::class_<fort::myrmidon::Query>(m,"Query")
		.def_static("ComputeMeasurementFor",
		            &fort::myrmidon::Query::ComputeMeasurementFor,
		            "experiment"_a,
		            py::kw_only(),
		            "antID"_a,
		            "measurementTypeID"_a,
		            R"pydoc(
    Computes Ant manual measurement in millimeters.

    Computes the list of manual measurements made in `fort-studio` for
    a given Ant in millimeters.

    Args:
        experiment (py_fort_myrmidon.Experiment): the experiment to
            query
        antID (int): the Ant to consider
        measurementTypeID (int): the kind of measurement to consider
    Returns:
        List[py_fort_myrmidon.Measurement]: the list of measurement
            for antID and measurementTypeID
)pydoc")
		.def_static("GetDataInformations",
		            &fort::myrmidon::Query::GetDataInformations,
		            "experiment"_a)
		.def_static("ComputeTagStatistics",
		            &fort::myrmidon::Query::ComputeTagStatistics,
		            "experiment"_a,
		            R"pydoc(
    Computes tag detection statistics in an experiment.

    Args:
        experiment (py_fort_myrmidon.Experiment): the experiment to
            query
    Returns:
        Dict[int,py_fort_myrmidon.TagStatistics]: the list of
            TagStatistics indexed by TagID.
)pydoc")
		.def_static("IdentifyFrames",&QueryIdentifyFrames,
		            "experiment"_a,
		            py::kw_only(),
		            "start"_a = identifyArgs.Start,
		            "end"_a = identifyArgs.End,
		            "singleThreaded"_a = identifyArgs.SingleThreaded,
		            "computeZones"_a = identifyArgs.ComputeZones,
		            R"pydoc(
    Gets Ant positions in video frames.

    Args:
        experiment (py_fort_myrmidon.Experiment): the experiment to
            query
        start (py_fort_myrmidon.Time): the first video acquisition
            time to consider
        end (py_fort_myrmidon.Time): the last video acquisition
            time to consider
        singleThreaded (bool): limits computation to happen in a
            single thread.
        computeZones (bool): computes the zone for the Ant, otherwise
            0 will always be returned for the ants' current ZoneID.
    Returns:
        List[py_fort_myrmidon.IdentifiedFrame]: the detected position
            of the Ant in video frames in [start;end[
)pydoc")
		.def_static("CollideFrames",
		            &QueryCollideFrames,
		            "experiment"_a,
		            py::kw_only(),
		            "start"_a = collideArgs.Start,
		            "end"_a = collideArgs.End,
		            "collisionsIgnoreZones"_a = collideArgs.CollisionsIgnoreZones,
		            "singleThreaded"_a = collideArgs.SingleThreaded,
		            R"pydoc(
    Gets Ant collision in video frames.

    Args:
        experiment (py_fort_myrmidon.Experiment): the experiment to
            query
        start (py_fort_myrmidon.Time): the first video acquisition
            time to consider
        end (py_fort_myrmidon.Time): the last video acquisition
            time to consider
        singleThreaded (bool): limits computation to happen in a
            single thread.
    Returns:
        List[Tuple[py_fort_myrmidon.IdentifiedFrame,py_fort_myrmidon.CollisionFrame]]:
            the detected position and collision of the Ants in video frames in
            [start;end[
 )pydoc")
		.def_static("ComputeAntTrajectories",
		            &QueryComputeAntTrajectories,
		            "experiment"_a,
		            py::kw_only(),
		            "start"_a = trajectoryArgs.Start,
		            "end"_a = trajectoryArgs.End,
		            "maximumGap"_a = trajectoryArgs.MaximumGap,
		            "matcher"_a = trajectoryArgs.Matcher,
		            "computeZones"_a = trajectoryArgs.ComputeZones,
		            "singleThreaded"_a = trajectoryArgs.SingleThreaded,
		            R"pydoc(
    Conputes Ant Trajectories between two times.

    Args:
        experiment (py_fort_myrmidon.Experiment): the experiment to
            query
        start (py_fort_myrmidon.Time): the first video acquisition
            time to consider
        end (py_fort_myrmidon.Time): the last video acquisition
            time to consider
        maximumGap (py_fort_myrmidon.Duration): maximum tracking gap
            allowed in a AntTrajectory object.
        matcher (py_fort_myrmidon.Matcher): a Matcher that reduces
            down the query to more specific use case.
        computeZones (bool): computes the zone of the Ant. Otherwise 0
            will always be returned.
        singleThreaded (bool): limits computation to happen in a
            single thread.
    Returns:
        List[py_fort_myrmidon.AntTrajectory]: a list of all
            AntTrajectory taking place in [start;end[ given the
            matcher criterion and maximumGap.
)pydoc")
		.def_static("ComputeAntInteractions",
		            &QueryComputeAntInteractions,
		            "experiment"_a,
		            py::kw_only(),
		            "start"_a = interactionArgs.Start,
		            "end"_a = interactionArgs.End,
		            "maximumGap"_a = interactionArgs.MaximumGap,
		            "matcher"_a = interactionArgs.Matcher,
		            "collisionsIgnoreZones"_a = interactionArgs.CollisionsIgnoreZones,
		            "reportFullTrajectories"_a = interactionArgs.ReportFullTrajectories,
		            "singleThreaded"_a = interactionArgs.SingleThreaded,
		            R"pydoc(
    Conputes Ant Interctions between two times.

    Args:
        experiment (py_fort_myrmidon.Experiment): the experiment to
            query
        start (py_fort_myrmidon.Time): the first video acquisition
            time to consider
        end (py_fort_myrmidon.Time): the last video acquisition
            time to consider
        maximumGap (py_fort_myrmidon.Duration): maximum tracking gap
            allowed in a AntTrajectory object.
        matcher (py_fort_myrmidon.Matcher): a Matcher that reduces
            down the query to more specific use case.
        reportFullTrajectories (bool): if true, full AntTrajectory
            will be computed and returned. Otherwise, none will be
            returned and only the average Ants position will be
            returned in AntTrajectorySegment.
        singleThreaded (bool): limits computation to happen in a
            single thread.
    Returns:
        List[py_fort_myrmidon.AntTrajectory]: a list of all
            AntTrajectory taking place in [start;end[ given the
            matcher criterion and maximumGap if reportFullTrajectories
            is `true`. Otherwise it will be an empty list.
        List[py_fort_myrmidon.AntInteraction]: a list of all
            AntInteraction taking place in [start;end[ given the
            matcher criterion and maximumGap
  )pydoc")
		.def_static("FindMovieSegment",
		            &FindMovieSegment,
		            "experiment"_a,
		            py::kw_only(),
		            "space"_a = 1,
		            "start"_a = fort::Time::SinceEver(),
		            "end"_a = fort::Time::Forever(),
		            R"pydoc(

)pydoc")
		;


}
