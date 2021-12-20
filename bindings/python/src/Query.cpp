#include "BindTypes.hpp"

#include <fort/myrmidon/Query.hpp>
#include <fort/myrmidon/Matchers.hpp>
#include <fort/myrmidon/Experiment.hpp>
#include <fort/myrmidon/Video.hpp>

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
                                     bool segmentOnMatcherValueChange,
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
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;
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
                                      bool segmentOnMatcherValueChange,
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
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;
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


std::shared_ptr<fort::myrmidon::VideoSegment::List>
FindVideoSegments(const fort::myrmidon::Experiment & e,
                  fort::myrmidon::SpaceID space,
                  const fort::Time & start,
                  const fort::Time & end) {
	auto segments = std::make_shared<std::vector<fort::myrmidon::VideoSegment>>();
	fort::myrmidon::Query::FindVideoSegments(e,*segments,space,start,end);
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

Computes the list of manual measurements made in `fort-studio` for a
given Ant in millimeters.

Args:
    experiment (Experiment): the experiment to query
    antID (int): the Ant to consider
    measurementTypeID (int): the kind of measurement to consider

Returns:
        List[Measurement]: the list of measurement for **antID** and **measurementTypeID**
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
    experiment (Experiment): the experiment to query

Returns:
    Dict[int,TagStatistics]: the list of TagStatistics indexed by TagID.
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
    experiment (Experiment): the experiment to query
    start (Time): the first video acquisition time to consider
    end (Time): the last video acquisition time to consider
    singleThreaded (bool): limits computation to happen in a single thread.
    computeZones (bool): computes the zone for the Ant, otherwise 0 will always be returned for the ants' current ZoneID.

Returns:
    List[IdentifiedFrame]: the detected position of the Ant in video frames in [**start**;**end**[
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
    experiment (Experiment): the experiment to query
    start (Time): the first video acquisition time to consider
    end (Time): the last video acquisition time to consider
    singleThreaded (bool): limits computation to happen in a single thread.
    collisionsIgnoreZones (bool): collision detection ignore zones definition

Returns:
    List[Tuple[IdentifiedFrame,CollisionFrame]]: the detected position and collision of the Ants in video frames in [**start**;**end**[
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
		            "segmentOnMatcherValueChange"_a = trajectoryArgs.SegmentOnMatcherValueChange,
		            "singleThreaded"_a = trajectoryArgs.SingleThreaded,
		            R"pydoc(
Conputes Ant Trajectories between two times.

Args:
    experiment (Experiment): the experiment to query
    start (Time): the first video acquisition time to consider
    end (Time): the last video acquisition time to consider
    maximumGap (Duration): maximum tracking gap allowed in a :class:`AntTrajectory` object.
    matcher (Matcher): a :class:`Matcher` that reduces down the query to more specific use case.
    computeZones (bool): computes the zone of the Ant. Otherwise 0 will always be returned.
    singleThreaded (bool): limits computation to happen in a single thread.
    segmentOnMatcherValueChange (bool): if True, when a combined
        matcher ( "behavior" == "grooming" || "behavior" = "sleeping"
        ) value change, create a new trajectory.
Returns:
    List[AntTrajectory]: a list of all :class:`AntTrajectory` taking place in [**start**;**end**[ given the **matcher** and **maximumGap** criterions.

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
		            "segmentOnMatcherValueChange"_a = interactionArgs.SegmentOnMatcherValueChange,
		            "singleThreaded"_a = interactionArgs.SingleThreaded,
		            R"pydoc(
Conputes Ant Interctions between two times.

Args:
    experiment (Experiment): the experiment to query
    start (Time): the first video acquisition time to consider
    end (Time): the last video acquisition time to consider
    maximumGap (Duration): maximum tracking gap allowed in  :class:`AntInteraction` or :class:`AntTrajectory` objects.
    matcher (Matcher): a Matcher that reduces down the query to more specific use case.
    reportFullTrajectories (bool): if true, full AntTrajectories
        will be computed and returned. Otherwise, none will be
        returned and only the average Ants position will be
        returned in AntTrajectorySegment.
    singleThreaded (bool): limits computation to happen in a single thread.
    segmentOnMatcherValueChange (bool): if True, when a combined
        matcher ( "behavior" == "grooming" || "behavior" = "sleeping"
        ) value change, create a new trajectory.
Returns:
    Tuple[List[AntTrajectory],List[AntInteraction]]:
        * a list of all AntTrajectory taking place in [start;end[
          given the matcher criterion and maximumGap if
          reportFullTrajectories is `true`. Otherwise it will be an
          empty list.
        * a list of all AntInteraction taking place
          in [start;end[ given the matcher criterion and maximumGap
)pydoc")
		.def_static("FindVideoSegments",
		            &FindVideoSegments,
		            "experiment"_a,
		            py::kw_only(),
		            "space"_a = 1,
		            "start"_a = fort::Time::SinceEver(),
		            "end"_a = fort::Time::Forever(),
		            R"pydoc(
Finds :class:`VideoSegment` in a time range

Args:
    experiment (Experiment): the Experiment to query
    space (int): the SpaceID to ask videos for
    start (Time): the first time to query a video frame
    end (Time): the last time to query a video frame

Returns:
    VideoSegmentList: list of :class:`VideoSegment` in **space** that covers [**start**;**end**].
)pydoc")
		.def_static("GetMetaDataKeyRanges",
		            &fort::myrmidon::Query::GetMetaDataKeyRanges,
		            "experiment"_a,
		            py::kw_only(),
		            "key"_a,
		            "value"_a,
		            R"pydoc(
Gets the time ranges where metadata key has a given value

Args:
    experiment (Experiment): the Experiment to query
    key (str): the metadata key to test
    value (str): the value to test for equality

Returns:
    List[Tuple[int,Time,Time]]: time ranges for each AntID where **key** == **value**

Raises:
    IndexError: if **key** is not defined in Experiment
    ValueError: if **value** is not the right type for **key**
)pydoc")
		;


}
