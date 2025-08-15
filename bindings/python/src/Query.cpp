#include "BindTypes.hpp"

#include "Progress.hpp"

#include <condition_variable>
#include <fort/myrmidon/types/AntInteraction.hpp>
#include <fort/myrmidon/types/AntTrajectory.hpp>
#include <fort/myrmidon/types/IdentifiedFrame.hpp>
#include <thread>

#include <fort/myrmidon/Experiment.hpp>
#include <fort/myrmidon/Matchers.hpp>
#include <fort/myrmidon/Query.hpp>
#include <fort/myrmidon/Video.hpp>

namespace py = pybind11;

std::optional<py::list> QueryIdentifyFrames(
    const fort::myrmidon::Experiment                            &experiment,
    fort::Time                                                   start,
    fort::Time                                                   end,
    bool                                                         singleThreaded,
    size_t                                                       zoneDepth,
    fort::myrmidon::ZonePriority                                 zoneOrder,
    bool                                                         reportProgress,
    std::optional<fort::myrmidon::Query::IdentifyFramesCallback> onEachFrame
) {

	fort::myrmidon::Query::IdentifyFramesArgs args;
	args.Start          = start;
	args.End            = end;
	args.SingleThreaded = singleThreaded;
	args.ZoneDepth      = zoneDepth;
	args.Order          = zoneOrder;
	if (reportProgress) {
		args.Progress = std::make_unique<TimeProgress>("Identifiying frames");
	}
	if (onEachFrame.has_value() == false) {
		py::list res;
		fort::myrmidon::Query::IdentifyFramesFunctor(
		    experiment,
		    [&res](const fort::myrmidon::IdentifiedFrame::Ptr &f) {
			    res.append(f);
		    },
		    args
		);
		return res;
	} else {
		fort::myrmidon::Query::IdentifyFramesFunctor(
		    experiment,
		    onEachFrame.value(),
		    args
		);
		return std::nullopt;
	}
}

std::optional<py::list> QueryCollideFrames(
    const fort::myrmidon::Experiment &experiment,
    fort::Time                        start,
    fort::Time                        end,
    size_t                            zoneDepth,
    fort::myrmidon::ZonePriority      zoneOrder,
    bool                              collisionsIgnoreZones,
    bool                              singleThreaded,
    bool                              reportProgress,
    std::optional<fort::myrmidon::Query::CollideFramesCallback> onEachFrame
) {
	fort::myrmidon::Query::CollideFramesArgs args;
	args.Start                 = start;
	args.End                   = end;
	args.SingleThreaded        = singleThreaded;
	args.CollisionsIgnoreZones = collisionsIgnoreZones;
	if (reportProgress) {
		args.Progress = std::make_unique<TimeProgress>("Colliding frames");
	}

	if (onEachFrame.has_value()) {
		fort::myrmidon::Query::CollideFramesFunctor(
		    experiment,
		    onEachFrame.value(),
		    args
		);
		return std::nullopt;
	} else {
		py::list res;

		fort::myrmidon::Query::CollideFramesFunctor(
		    experiment,
		    [&](const fort::myrmidon::CollisionData &d) { res.append(d); },
		    args
		);
		return res;
	}
}

std::optional<py::list> QueryComputeAntTrajectories(
    const fort::myrmidon::Experiment   &experiment,
    fort::Time                          start,
    fort::Time                          end,
    fort::Duration                      maximumGap,
    const fort::myrmidon::Matcher::Ptr &matcher,
    size_t                              zoneDepth,
    fort::myrmidon::ZonePriority        zoneOrder,
    bool                                segmentOnMatcherValueChange,
    bool                                reportSmall,
    bool                                singleThreaded,
    bool                                reportProgress,
    std::optional<fort::myrmidon::Query::NewTrajectoryCallback> onNewTrajectory
) {

	fort::myrmidon::Query::ComputeAntTrajectoriesArgs args;
	args.Start                       = start;
	args.End                         = end;
	args.MaximumGap                  = maximumGap;
	args.Matcher                     = matcher;
	args.ZoneDepth                   = zoneDepth;
	args.Order                       = zoneOrder;
	args.SingleThreaded              = singleThreaded;
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;
	args.ReportSmall                 = reportSmall;
	if (reportProgress) {
		args.Progress =
		    std::make_unique<TimeProgress>("Computing ant trajectories");
	}

	if (onNewTrajectory.has_value() == false) {
		py::list res;
		fort::myrmidon::Query::ComputeAntTrajectoriesFunctor(
		    experiment,
		    [&](const fort::myrmidon::AntTrajectory::Ptr &t) { res.append(t); },
		    args
		);
		return res;
	}
	fort::myrmidon::Query::ComputeAntTrajectoriesFunctor(
	    experiment,
	    onNewTrajectory.value(),
	    args
	);
	return std::nullopt;
}

std::optional<std::tuple<py::list, py::list>> QueryComputeAntInteractions(
    const fort::myrmidon::Experiment   &experiment,
    fort::Time                          start,
    fort::Time                          end,
    fort::Duration                      maximumGap,
    const fort::myrmidon::Matcher::Ptr &matcher,
    size_t                              zoneDepth,
    fort::myrmidon::ZonePriority        zoneOrder,
    bool                                collisionsIgnoreZones,
    bool                                reportFullTrajectories,
    bool                                segmentOnMatcherValueChange,
    bool                                reportSmall,
    bool                                singleThreaded,
    bool                                reportProgress,
    std::optional<fort::myrmidon::Query::NewTrajectoryCallback> onNewTrajectory,
    std::optional<fort::myrmidon::Query::NewInteractionCallback>
        onNewInteraction
) {

	fort::myrmidon::Query::ComputeAntInteractionsArgs args;
	args.Start                       = start;
	args.End                         = end;
	args.MaximumGap                  = maximumGap;
	args.Matcher                     = matcher;
	args.ReportFullTrajectories      = reportFullTrajectories;
	args.SingleThreaded              = singleThreaded;
	args.CollisionsIgnoreZones       = collisionsIgnoreZones;
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;
	args.ReportSmall                 = reportSmall;
	if (reportProgress) {
		args.Progress =
		    std::make_unique<TimeProgress>("Computing ant interactions");
	}

	if (onNewInteraction.has_value() == false &&
	    onNewTrajectory.has_value() == false) {
		py::list trajectories;
		py::list interactions;

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
		return std::make_tuple(trajectories, interactions);
	}

	if (onNewInteraction.has_value() == false) {
		onNewInteraction = [](const fort::myrmidon::AntInteraction::Ptr &) {};
	}
	if (onNewTrajectory.has_value() == false) {
		onNewTrajectory = [](const fort::myrmidon::AntTrajectory::Ptr &) {};
	}
	fort::myrmidon::Query::ComputeAntInteractionsFunctor(
	    experiment,
	    onNewTrajectory.value(),
	    onNewInteraction.value(),
	    args
	);

	return std::nullopt;
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
	        "zoneDepth"_a      = identifyArgs.ZoneDepth,
	        "zoneOrder"_a      = identifyArgs.Order,
	        "reportProgress"_a = true,
	        "onEachFrame"_a    = nullptr,
	        R"pydoc(
Gets Ant positions in tracked frames.There is two modes of operation: using a
onEachFrame callback, that will allow you to perform computation on each tracked
frame, or using the return value. The latter may require a lot of memory, so it
will be safer to only query a small time subset.

Args:
    experiment (Experiment): the experiment to query
    start (Time): the first video acquisition time to consider
    end (Time): the last video acquisition time to consider
    singleThreaded (bool): limits computation to happen in a single thread.
    zoneDepth (int): number of zones that will be computed for each ant.
    zoneOrder (fort_myrmidon.ZonePriority): priority of zone in case of conflict.
    onEachFrame (Callable[fort_myrmidon.IdentifiedFrame,None]): a callback
        function for each Identified frames. If specified, IdentifyFrames() will
        return None. If you only care about a few informations, this callback
        can be used to remove memory pressure.

Returns:
    List[IdentifiedFrame] | None: the detected position of the Ant in video frames in [ **start** ; **end** [ when **onEachFrame** is `None`.
)pydoc"
	    )
	    .def_static(
	        "CollideFrames",
	        &QueryCollideFrames,
	        "experiment"_a,
	        py::kw_only(),
	        "start"_a                 = collideArgs.Start,
	        "end"_a                   = collideArgs.End,
	        "zoneDepth"_a             = collideArgs.ZoneDepth,
	        "zoneOrder"_a             = collideArgs.Order,
	        "collisionsIgnoreZones"_a = collideArgs.CollisionsIgnoreZones,
	        "singleThreaded"_a        = collideArgs.SingleThreaded,
	        "reportProgress"_a        = true,
	        "onEachFrame"_a           = std::nullopt,
	        R"pydoc(
Gets Ant collision in tracked frames. There is two modes of operation: using a
onEachFrame callback, that will allow you to perform computation on each tracked
frame, or using the return value. The latter may require a lot of memory, so it
will be safer to only query a small time subset.

Args:
    experiment (Experiment): the experiment to query
    start (Time): the first video acquisition time to consider
    end (Time): the last video acquisition time to consider
    singleThreaded (bool): limits computation to happen in a single thread.
    zoneDepth (int): number of zones that will be computed for each ant.
    zoneOrder (fort_myrmidon.ZonePriority): priority of zone in case of conflict.
    collisionsIgnoreZones (bool): collision detection ignore zones definition
    onNewFrame(Callable[Tuple[fort_myrmidon.IdentifiedFrame,fort_myrmidon.CollisionFrame],None]):
        a callback function to get the result for each frames. If specified,
        this function will return None. It could be used to reduce the memory
        pressure of parsing large datasets.

Returns:
    List[Tuple[IdentifiedFrame,CollisionFrame]] | None: If **onNewFrame** is `None`, the detected position and collision of the Ants in tracked frames in [ **start** ; **end** [.
)pydoc"
	    )
	    .def_static(
	        "ComputeAntTrajectories",
	        &QueryComputeAntTrajectories,
	        "experiment"_a,
	        py::kw_only(),
	        "start"_a      = trajectoryArgs.Start,
	        "end"_a        = trajectoryArgs.End,
	        "maximumGap"_a = trajectoryArgs.MaximumGap,
	        "matcher"_a    = trajectoryArgs.Matcher,
	        "zoneDepth"_a  = trajectoryArgs.ZoneDepth,
	        "zoneOrder"_a  = trajectoryArgs.Order,
	        "segmentOnMatcherValueChange"_a =
	            trajectoryArgs.SegmentOnMatcherValueChange,
	        "reportSmall"_a     = trajectoryArgs.ReportSmall,
	        "singleThreaded"_a  = trajectoryArgs.SingleThreaded,
	        "reportProgress"_a  = true,
	        "onNewTrajectory"_a = std::nullopt,
	        R"pydoc(
Conputes Ant Trajectories between two times. There is two modes of operation:
using a **onNewTrajectory** callback, that will allow you to perform computation on
each computed trajectories, or using the return value. The latter may require a
lot of memory, so it will be safer to only query a small time subset.


Args:
    experiment (Experiment): the experiment to query
    start (Time): the first video acquisition time to consider
    end (Time): the last video acquisition time to consider
    maximumGap (Duration): maximum tracking gap allowed in a :class:`AntTrajectory` object.
    matcher (Matcher): a :class:`Matcher` that reduces down the query to more specific use case.
    zoneDepth (int): number of zones that will be computed for each ant.
    zoneOrder (fort_myrmidon.ZonePriority): priority of zone in case of conflict.
    singleThreaded (bool): limits computation to happen in a single thread.
    segmentOnMatcherValueChange (bool): if True, when a combined
        matcher ( "behavior" == "grooming" || "behavior" = "sleeping"
        ) value change, create a new trajectory.
    reportSmall (bool): Reports trajectories with a single time point.
    onNewTrajectory (Callable[fort_myrmidon.AntTrajectory,None]): If specified,
        no data will be returned, but this callabled will be called for each
        results. It allows to reduce memory pressure when only a few metrics are
        needed from the results.

Returns:
    List[AntTrajectory]|None: a list of all :class:`AntTrajectory` taking place in [ **start** ; **end** [ given the **matcher** and **maximumGap** criterions. If **onNewTrajectory** is specified, it will return None.

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
	        "zoneDepth"_a              = trajectoryArgs.ZoneDepth,
	        "zoneOrder"_a              = trajectoryArgs.Order,
	        "collisionsIgnoreZones"_a  = interactionArgs.CollisionsIgnoreZones,
	        "reportFullTrajectories"_a = interactionArgs.ReportFullTrajectories,
	        "segmentOnMatcherValueChange"_a =
	            interactionArgs.SegmentOnMatcherValueChange,
	        "reportSmall"_a      = interactionArgs.ReportSmall,
	        "singleThreaded"_a   = interactionArgs.SingleThreaded,
	        "reportProgress"_a   = true,
	        "onNewTrajectory"_a  = std::nullopt,
	        "onNewInteraction"_a = std::nullopt,
	        R"pydoc(

Computes Ant Interactions between two times. There is two modes of operation:
using a **onNewTrajectory** *and/or* **onNewInteraction** callbacks, that will
allow you to perform computation on each type of result as they are queried, or
using the return value. The latter may require a lot of memory, so it will be
safer to only query a small time subset.


Args:
    experiment (Experiment): the experiment to query
    start (Time): the first video acquisition time to consider
    end (Time): the last video acquisition time to consider
    maximumGap (Duration): maximum tracking gap allowed in
        :class:`AntInteraction` or :class:`AntTrajectory` objects.
    matcher (Matcher): a Matcher that reduces down the query to more specific
        use case.
    zoneDepth (int): number of zones that will be computed for each ant.
    zoneOrder (fort_myrmidon.ZonePriority): priority of zone in case of conflict.
    collisionsIgnoreZones (bool): collision detection ignore zones definition
    reportFullTrajectories (bool): if true, full AntTrajectories
        will be computed and returned. Otherwise, none will be
        returned and only the average Ants position will be
        returned in AntTrajectorySegment.
    singleThreaded (bool): limits computation to happen in a single thread.
    segmentOnMatcherValueChange (bool): if True, when a combined
        matcher ( "behavior" == "grooming" || "behavior" = "sleeping"
        ) value change, create a new trajectory.
    reportSmall (bool): Reports trajectories and interactions with a single time
        point.
    onNewTrajectory (Callable[fort_myrmidon.AntTrajectory,None]): If specified,
        this query will return None, and any discovered trajectory will be
        passed to this callback as they are computed.
    onNewInteraction (Callable[fort_myrmidon.AntInteraction,None]): If
        specified, this query will return None, and any discovered interactions
        will be passed to this callback as they are computed.

Returns:
    Tuple[List[AntTrajectory],List[AntInteraction]] | None: If neither **onNewTrajectory**, nor **onNewInteraction** is specified, it will return:
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
