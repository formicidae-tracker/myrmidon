#include "BindTypes.hpp"

#include <condition_variable>
#include <thread>

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

class QueryProgress {
private:
	py::object d_progress;
	fort::Time d_start;
	uint64_t d_lastMinuteReported;
public:
	QueryProgress(const fort::myrmidon::Experiment & e,
	           fort::Time start,
	           fort::Time end,
	           const std::string & description,
	           bool verbose = true)
		: d_progress(pybind11::none()) {
		if ( verbose == false) {
			return;
		}
		if ( start.IsInfinite() || end.IsInfinite() ) {
			auto dataInfo = fort::myrmidon::Query::GetDataInformations(e);
			if ( start.IsInfinite() ) {
				start = dataInfo.Start;
			}
			if ( end.IsInfinite() ) {
				end = dataInfo.End;
			}
		}

		using namespace pybind11::literals;

		d_start = start;
		uint64_t minutes =  std::ceil(end.Sub(start).Minutes());
		d_lastMinuteReported = 0;

		d_progress = py::module_::import("tqdm").attr("tqdm")("total"_a = minutes,
		                                                      "desc"_a = description,
		                                                      "ncols"_a = 80,
		                                                      "unit"_a = "tracked min");
	}

	~QueryProgress() {
		if ( d_progress.is_none() == false ) {
			d_progress.attr("close")();
		}
	}

	void Update(const fort::Time & t) {
		check_py_interrupt();
		if ( d_progress.is_none() == true ) {
			return;
		}
		using namespace pybind11::literals;

		uint64_t minuteEllapsed = std::floor(t.Sub(d_start).Minutes());
		if ( minuteEllapsed > d_lastMinuteReported) {
			d_lastMinuteReported = minuteEllapsed;
			d_progress.attr("update")("n"_a = minuteEllapsed - d_lastMinuteReported);
		}
	}
};

py::list QueryIdentifyFrames(const fort::myrmidon::Experiment & experiment,
                             fort::Time start,
                             fort::Time end,
                             bool singleThreaded,
                             bool computeZones,
                             bool reportProgress) {
	py::list res;
	fort::myrmidon::Query::IdentifyFramesArgs args;
	args.Start = start;
	args.End = end;
	args.SingleThreaded = singleThreaded;
	args.ComputeZones = computeZones;
	args.AllocationInCurrentThread = reportProgress;
	QueryProgress progress(experiment,start,end,"Identifiying frames",reportProgress);
	fort::myrmidon::Query::IdentifyFramesFunctor(experiment,
	                                             [&res,&progress](const fort::myrmidon::IdentifiedFrame::Ptr & f) {
		                                             res.append(f);
		                                             progress.Update(f->FrameTime);
	                                             },
	                                             args);
	return res;
}

py::list QueryCollideFrames(const fort::myrmidon::Experiment & experiment,
                            fort::Time start,
                            fort::Time end,
                            bool collisionsIgnoreZones,
                            bool singleThreaded,
                            bool reportProgress) {
	py::list res;
	fort::myrmidon::Query::CollideFramesArgs args;
	args.Start = start;
	args.End = end;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = reportProgress;
	args.CollisionsIgnoreZones = collisionsIgnoreZones;
	QueryProgress progress(experiment,start,end,"Colliding frames",reportProgress);
	fort::myrmidon::Query::CollideFramesFunctor(experiment,
	                                            [&](const fort::myrmidon::CollisionData & d) {
		                                             res.append(d);
		                                             progress.Update(std::get<0>(d)->FrameTime);
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
                                     bool singleThreaded,
                                     bool reportProgress) {

	py::list res;
	fort::myrmidon::Query::ComputeAntTrajectoriesArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.ComputeZones = computeZones;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = reportProgress;
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;
	QueryProgress progress(experiment,start,end,"Computing ant trajectories",reportProgress);
	fort::myrmidon::Query::ComputeAntTrajectoriesFunctor(experiment,
	                                                     [&](const fort::myrmidon::AntTrajectory::Ptr & t) {
		                                                     res.append(t);
		                                                     progress.Update(t->End());
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
                                      bool singleThreaded,
                                      bool reportProgress) {

	py::list trajectories;
	py::list interactions;

	fort::myrmidon::Query::ComputeAntInteractionsArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.ReportFullTrajectories = reportFullTrajectories;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = reportProgress;
	args.CollisionsIgnoreZones = collisionsIgnoreZones;
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;
	QueryProgress progress(experiment,start,end,"Computing ant interactions",reportProgress);
	fort::myrmidon::Query::ComputeAntInteractionsFunctor(experiment,
	                                                     [&](const fort::myrmidon::AntTrajectory::Ptr & t) {
		                                                     trajectories.append(t);
		                                                     progress.Update(t->End());
	                                                     },
	                                                     [&](const fort::myrmidon::AntInteraction::Ptr & i) {
		                                                     interactions.append(i);
		                                                     progress.Update(i->End);
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


py::object GetTagCloseUps(const fort::myrmidon::Experiment & e,
                          bool verbose) {
	using namespace fort::myrmidon;
	using namespace pybind11::literals;

	py::object pd = py::module_::import("pandas");
	py::object tqdm = py::module_::import("tqdm");
	int total(0),current(0);
	bool done(false);
	std::mutex m;
	std::condition_variable cv;

	std::vector<std::string> paths;
	std::vector<TagID> IDs;
	Eigen::MatrixXd data;

	std::thread op([&](){
		               std::tie(paths,IDs,data)
			               = Query::GetTagCloseUps(e,
			                                       [&](int current_,int total_) {
				                                       {
					                                       std::lock_guard<std::mutex> lock(m);
					                                       current = current_;
					                                       total = total_;
				                                       }
				                                       cv.notify_all();
			                                       },
			                                       [&](const char * what) {
				                                       if ( verbose == false ) {
					                                       return;
				                                       }
				                                       std::lock_guard<std::mutex> lock(m);
				                                       std::cerr << what << std::endl;
			                                       });
		               {
			               std::lock_guard<std::mutex> lock(m);
			               done = true;
		               }
		               cv.notify_all();

	               });

	py::object progress = pybind11::none();
	int lastCurrent = 0;
	do {

		std::unique_lock<std::mutex> lock(m);
		cv.wait(lock,[&]() { return total > 0 || done == true; });
		if ( done == true ) {
			if ( progress.is_none() == false ) {
				progress.attr("close")();
			}
			break;
		}
		if ( PyErr_CheckSignals() != 0 ) {
			op.detach();
			throw py::error_already_set();
		}
		if ( progress.is_none() == true ) {
			progress = tqdm.attr("tqdm")("total"_a = total,
			                             "desc"_a = "Computing Close-Ups",
			                             "ncols"_a = 80);
		}
		if ( current != lastCurrent ) {
			progress.attr("update")("n"_a = current - lastCurrent);
			lastCurrent = current;
		}
	} while( true );
	op.join();

	py::object df = pd.attr("DataFrame")("data"_a = py::dict("path"_a = paths,
	                                                         "ID"_a = IDs));
	py::list cols;
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
	return df.attr("join")(pd.attr("DataFrame")("data"_a = data,
	                                            "columns"_a = cols));
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
)pydoc")
		.def_static("CollideFrames",
		            &QueryCollideFrames,
		            "experiment"_a,
		            py::kw_only(),
		            "start"_a = collideArgs.Start,
		            "end"_a = collideArgs.End,
		            "collisionsIgnoreZones"_a = collideArgs.CollisionsIgnoreZones,
		            "singleThreaded"_a = collideArgs.SingleThreaded,
		            "reportProgress"_a = true,
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
		.def_static("GetTagCloseUps",
		            &GetTagCloseUps,
		            "experiment"_a,
		            "verbose"_a = false,
		            R"pydoc(
Gets the tag close-up in this experiment

Args:
    experiment (Experiment): the Experiment to query

Returns:
    pandas.DataFrame: the close-up data in the experiment
)pydoc")

		;


}
