#include "BindTypes.hpp"

#include "Progress.hpp"

#include <condition_variable>
#include <thread>

#include <fort/myrmidon/Query.hpp>
#include <fort/myrmidon/Matchers.hpp>
#include <fort/myrmidon/Experiment.hpp>
#include <fort/myrmidon/Video.hpp>

namespace py = pybind11;

py::list QueryIdentifyFrames(
    const fort::myrmidon::Experiment &experiment,
    fort::Time                        start,
    fort::Time                        end,
    bool                              singleThreaded,
    bool                              computeZones,
    bool                              reportProgress
) {
	py::list                                  res;
	fort::myrmidon::Query::IdentifyFramesArgs args;
	args.Start          = start;
	args.End            = end;
	args.SingleThreaded = singleThreaded;
	args.ComputeZones   = computeZones;
	if (reportProgress) {
		args.Progress = std::make_unique<TimeProgress>("Identifiying frames");
	}
	fort::myrmidon::Query::IdentifyFramesFunctor(
	    experiment,
	    [&res](const fort::myrmidon::IdentifiedFrame::Ptr &f) {
		    res.append(f);
	    },
	    args
	);
	return res;
}

py::list QueryCollideFrames(
    const fort::myrmidon::Experiment &experiment,
    fort::Time                        start,
    fort::Time                        end,
    bool                              collisionsIgnoreZones,
    bool                              singleThreaded,
    bool                              reportProgress
) {
	py::list                                 res;
	fort::myrmidon::Query::CollideFramesArgs args;
	args.Start                 = start;
	args.End                   = end;
	args.SingleThreaded        = singleThreaded;
	args.CollisionsIgnoreZones = collisionsIgnoreZones;
	if (reportProgress) {
		args.Progress = std::make_unique<TimeProgress>("Colliding frames");
	}
	fort::myrmidon::Query::CollideFramesFunctor(
	    experiment,
	    [&](const fort::myrmidon::CollisionData &d) { res.append(d); },
	    args
	);
	return res;
}

py::list QueryComputeAntTrajectories(
    const fort::myrmidon::Experiment   &experiment,
    fort::Time                          start,
    fort::Time                          end,
    fort::Duration                      maximumGap,
    const fort::myrmidon::Matcher::Ptr &matcher,
    bool                                computeZones,
    bool                                segmentOnMatcherValueChange,
    bool                                singleThreaded,
    bool                                reportProgress
) {

	py::list                                          res;
	fort::myrmidon::Query::ComputeAntTrajectoriesArgs args;
	args.Start                       = start;
	args.End                         = end;
	args.MaximumGap                  = maximumGap;
	args.Matcher                     = matcher;
	args.ComputeZones                = computeZones;
	args.SingleThreaded              = singleThreaded;
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;
	if (reportProgress) {
		args.Progress =
		    std::make_unique<TimeProgress>("Computing ant trajectories");
	}
	fort::myrmidon::Query::ComputeAntTrajectoriesFunctor(
	    experiment,
	    [&](const fort::myrmidon::AntTrajectory::Ptr &t) { res.append(t); },
	    args
	);
	return res;
}

py::tuple QueryComputeAntInteractions(
    const fort::myrmidon::Experiment   &experiment,
    fort::Time                          start,
    fort::Time                          end,
    fort::Duration                      maximumGap,
    const fort::myrmidon::Matcher::Ptr &matcher,
    bool                                collisionsIgnoreZones,
    bool                                reportFullTrajectories,
    bool                                segmentOnMatcherValueChange,
    bool                                singleThreaded,
    bool                                reportProgress
) {

	py::list trajectories;
	py::list interactions;

	fort::myrmidon::Query::ComputeAntInteractionsArgs args;
	args.Start                       = start;
	args.End                         = end;
	args.MaximumGap                  = maximumGap;
	args.Matcher                     = matcher;
	args.ReportFullTrajectories      = reportFullTrajectories;
	args.SingleThreaded              = singleThreaded;
	args.CollisionsIgnoreZones       = collisionsIgnoreZones;
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;
	if (reportProgress) {
		args.Progress =
		    std::make_unique<TimeProgress>("Computing ant interactions");
	}
	fort::myrmidon::Query::ComputeAntInteractionsFunctor(
	    experiment,
	    [&](const fort::myrmidon::AntTrajectory::Ptr &t) {
		    trajectories.append(t);
	    },
	    [&](const fort::myrmidon::AntInteraction::Ptr &i) {
		    interactions.append(i);
	    },
	    args
	);
	return py::make_tuple(trajectories, interactions);
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

py::object
GetTagCloseUps(const fort::myrmidon::Experiment &e, bool fixCorruptedData) {
	using namespace fort::myrmidon;
	using namespace pybind11::literals;

	py::object               pd   = py::module_::import("pandas");
	py::object               tqdm = py::module_::import("tqdm");
	std::vector<std::string> paths;
	std::vector<TagID>       IDs;
	Eigen::MatrixXd          data;

	auto p = std::make_unique<ItemProgress>("Tag Close-Ups");
	std::tie(paths, IDs, data) =
	    Query::GetTagCloseUps(e, std::move(p), fixCorruptedData);

	py::object df = pd.attr("DataFrame"
	)("data"_a = py::dict("path"_a = paths, "ID"_a = IDs));
	py::list   cols;
	cols.append("X");
	cols.append("Y");
	cols.append("Theta");
	cols.append("c0_X");
	cols.append("c0_Y");
	cols.append("c1_X");
	cols.append("c1_Y");
	cols.append("c2_X");
	cols.append("c2_Y");
	cols.append("c3_X");
	cols.append("c3_Y");
	return df.attr("join"
	)(pd.attr("DataFrame")("data"_a = data, "columns"_a = cols));
}

void BindQuery(py::module_ &m) {
	using namespace pybind11::literals;

	fort::myrmidon::Query::IdentifyFramesArgs         identifyArgs;
	fort::myrmidon::Query::CollideFramesArgs          collideArgs;
	fort::myrmidon::Query::ComputeAntTrajectoriesArgs trajectoryArgs;
	fort::myrmidon::Query::ComputeAntInteractionsArgs interactionArgs;

	py::class_<fort::myrmidon::Query>(m, "Query")
	    .def_static(
	        "ComputeMeasurementFor",
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
)pydoc"
	    )
	    .def_static(
	        "GetDataInformations",
	        &fort::myrmidon::Query::GetDataInformations,
	        "experiment"_a
	    )
	    .def_static(
	        "ComputeTagStatistics",
	        [](const fort::myrmidon::Experiment &e, bool fixCorruptedData) {
		        return fort::myrmidon::Query::ComputeTagStatistics(
		            e,
		            std::make_unique<ItemProgress>("Tag Statistics"),
		            fixCorruptedData
		        );
	        },
	        "experiment"_a,
	        "fixCorruptedData"_a = false,
	        R"pydoc(
Computes tag detection statistics in an experiment.

Args:
    experiment (Experiment): the experiment to query.
    fixCorruptedData (bool): if True will silently fix any data
        corruption error found. This may lead to the loss of large
        chunck of tracking data. Otherwise, a RuntimeError will be
        raised.

Returns:
    Dict[int,TagStatistics]: the list of TagStatistics indexed by TagID.

Raises:
    RuntimeError: in vase of data corruption if fixCorruptedData == False
)pydoc"
	    )
	    .def_static(
	        "IdentifyFrames",
	        &QueryIdentifyFrames,
	        "experiment"_a,
	        py::kw_only(),
	        "start"_a          = identifyArgs.Start,
	        "end"_a            = identifyArgs.End,
	        "singleThreaded"_a = identifyArgs.SingleThreaded,
	        "computeZones"_a   = identifyArgs.ComputeZones,
	        "reportProgress"_a = true,
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
)pydoc"
	    )
	    .def_static(
	        "CollideFrames",
	        &QueryCollideFrames,
	        "experiment"_a,
	        py::kw_only(),
	        "start"_a                 = collideArgs.Start,
	        "end"_a                   = collideArgs.End,
	        "collisionsIgnoreZones"_a = collideArgs.CollisionsIgnoreZones,
	        "singleThreaded"_a        = collideArgs.SingleThreaded,
	        "reportProgress"_a        = true,
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
 )pydoc"
	    )
	    .def_static(
	        "ComputeAntTrajectories",
	        &QueryComputeAntTrajectories,
	        "experiment"_a,
	        py::kw_only(),
	        "start"_a        = trajectoryArgs.Start,
	        "end"_a          = trajectoryArgs.End,
	        "maximumGap"_a   = trajectoryArgs.MaximumGap,
	        "matcher"_a      = trajectoryArgs.Matcher,
	        "computeZones"_a = trajectoryArgs.ComputeZones,
	        "segmentOnMatcherValueChange"_a =
	            trajectoryArgs.SegmentOnMatcherValueChange,
	        "singleThreaded"_a = trajectoryArgs.SingleThreaded,
	        "reportProgress"_a = true,
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

)pydoc"
	    )
	    .def_static(
	        "ComputeAntInteractions",
	        &QueryComputeAntInteractions,
	        "experiment"_a,
	        py::kw_only(),
	        "start"_a                  = interactionArgs.Start,
	        "end"_a                    = interactionArgs.End,
	        "maximumGap"_a             = interactionArgs.MaximumGap,
	        "matcher"_a                = interactionArgs.Matcher,
	        "collisionsIgnoreZones"_a  = interactionArgs.CollisionsIgnoreZones,
	        "reportFullTrajectories"_a = interactionArgs.ReportFullTrajectories,
	        "segmentOnMatcherValueChange"_a =
	            interactionArgs.SegmentOnMatcherValueChange,
	        "singleThreaded"_a = interactionArgs.SingleThreaded,
	        "reportProgress"_a = true,
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
)pydoc"
	    )
	    .def_static(
	        "FindVideoSegments",
	        &FindVideoSegments,
	        "experiment"_a,
	        py::kw_only(),
	        "space"_a = 1,
	        "start"_a = fort::Time::SinceEver(),
	        "end"_a   = fort::Time::Forever(),
	        R"pydoc(
Finds :class:`VideoSegment` in a time range

Args:
    experiment (Experiment): the Experiment to query
    space (int): the SpaceID to ask videos for
    start (Time): the first time to query a video frame
    end (Time): the last time to query a video frame

Returns:
    VideoSegmentList: list of :class:`VideoSegment` in **space** that covers [**start**;**end**].
)pydoc"
	    )
	    .def_static(
	        "GetMetaDataKeyRanges",
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
)pydoc"
	    )
	    .def_static(
	        "GetTagCloseUps",
	        &GetTagCloseUps,
	        "experiment"_a,
	        "fixCorruptedData"_a = false,
	        R"pydoc(
Gets the tag close-up in this experiment

Args:
    experiment (Experiment): the Experiment to quer
    fixCorruptedData (bool): if True, data corruption will be silently
        fixed. In this case a few close-up may be lost. Otherwise it
        will raise an error.

Raises:
   RuntimeError: in case of data corruption and if fixCorruptedData == False.

Returns:
    pandas.DataFrame: the close-up data in the experiment
)pydoc"
	    )

	    ;
}
