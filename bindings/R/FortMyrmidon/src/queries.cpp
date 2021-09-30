#include "../inst/include/FortMyrmidon_types.hpp"

#include <fort/myrmidon/Query.hpp>

#include "time.h"

#include "types.h"

#include "Rcpp.h"



using namespace Rcpp;
using namespace fort::myrmidon;

//' Queries manual measurements for an Ant
//' @param experiment the \code{\link{fmExperiment}} to query
//' @param antID the ant to compute for
//' @param typeID the wanted measurement type
//' @return a data.frame with the date of the measurement and
//'   estimated size in millimeters and pixels.
//' @family fmQuery methods
//[[Rcpp::export]]
DataFrame fmQueryComputeMeasurementFor(const ExperimentPtr & experiment,
                                       fort::myrmidon::AntID antID,
                                       fort::myrmidon::MeasurementTypeID typeID = 1) {
	auto cMeasurements = Query::ComputeMeasurementFor(*experiment,antID, typeID);

	Rcpp::DatetimeVector times(cMeasurements.size());
	Rcpp::NumericVector lengthMM(cMeasurements.size()),lengthPx(cMeasurements.size());
	for( size_t i = 0; i < cMeasurements.size(); ++i ) {
		times[i] = fmTime_asR(cMeasurements[i].Time);
		lengthMM[i] = cMeasurements[i].LengthMM;
		lengthPx[i] = cMeasurements[i].LengthPixel;
	}

	return DataFrame::create(_["date"] = times,
	                         _["length_mm"] = lengthMM,
	                         _["length_px"] = lengthPx);
}

namespace Rcpp {
template <>
SEXP wrap(const fort::myrmidon::TagStatistics::ByTagID & tagStats ) {
	size_t nTags  = tagStats.size();
	Rcpp::IntegerVector TagID(nTags),Count(nTags),Multiple(nTags),Gap500(nTags),Gap1s(nTags),Gap10s(nTags),Gap1m(nTags),Gap10m(nTags),Gap1h(nTags),Gap10h(nTags),GapMore(nTags);
	Rcpp::CharacterVector TagName(nTags);
	Rcpp::DatetimeVector FirstSeen(nTags),LastSeen(nTags);
	size_t i = 0;
	for(const auto & [tagID,stat] : tagStats) {
		TagID[i] = stat.ID;
		TagName[i] = fort::myrmidon::FormatTagID(stat.ID);
		FirstSeen[i] = fmTime_asR(stat.FirstSeen);
		LastSeen[i] = fmTime_asR(stat.LastSeen);
		Count[i] = stat.Counts(TagStatistics::TOTAL_SEEN);
		Multiple[i] = stat.Counts(TagStatistics::MULTIPLE_SEEN);
		Gap500[i] = stat.Counts(TagStatistics::GAP_500MS);
		Gap1s[i] = stat.Counts(TagStatistics::GAP_1S);
		Gap10s[i] = stat.Counts(TagStatistics::GAP_10S);
		Gap1m[i] = stat.Counts(TagStatistics::GAP_1M);
		Gap10m[i] = stat.Counts(TagStatistics::GAP_10M);
		Gap1h[i] = stat.Counts(TagStatistics::GAP_1H);
		Gap10h[i] = stat.Counts(TagStatistics::GAP_10H);
		GapMore[i] = stat.Counts(TagStatistics::GAP_MORE);
		++i;
	}

	auto res = Rcpp::DataFrame::create(Rcpp::_["tagDecimalValue"] = TagID,
	                                   Rcpp::_["firstSeen"] = FirstSeen,
	                                   Rcpp::_["lastSeen"] = LastSeen,
	                                   Rcpp::_["count"] = Count,
	                                   Rcpp::_["multipleSeen"] = Multiple,
	                                   Rcpp::_["gap500ms"] = Gap500,
	                                   Rcpp::_["gap1s"] = Gap1s,
	                                   Rcpp::_["gap10s"] = Gap10s,
	                                   Rcpp::_["gap1m"] = Gap1m,
	                                   Rcpp::_["gap10m"] = Gap10m,
	                                   Rcpp::_["gap1h"] = Gap1h,
	                                   Rcpp::_["gap10h"] = Gap10h,
	                                   Rcpp::_["gap10h"] = GapMore);

	res.attr("row.names") = TagName;
	return res;
}
}

//' Computes tag statistics for an experiment
//' @param experiment the \code{\link{fmExperiment}} to query
//' @return a \code{data.frame} with the detection statistics in
//'   **experiment**
//' @family fmQuery methods
//[[Rcpp::export]]
fort::myrmidon::TagStatistics::ByTagID fmQueryComputeTagStatistics(const ExperimentPtr & experiment) {
	return Query::ComputeTagStatistics(*experiment);
}

static void checkInt(void * dummy) {
	R_CheckUserInterrupt();
}

fort::Duration Round(const fort::Duration & a,
                     const fort::Duration & mod) {
	return a.Nanoseconds() / mod.Nanoseconds() * mod;
}


class FmProgress {
public:
	class StopIteration{};
	static const fort::Duration DISPLAY_PERIOD;
	static const fort::Duration CHECK_PERIOD;
	FmProgress(const fort::myrmidon::Experiment & experiment,
	           const fort::Time & start,
	           const fort::Time & end,
	           bool showProgress)
		: d_showProgress(showProgress)
		, d_start(start)
		, d_duration(0) {
		fort::Time d_end = end;
		if ( d_start.IsInfinite() || d_end.IsInfinite() ) {
			auto dataInfo = Query::GetDataInformations(experiment);
			if ( d_start.IsInfinite() ) {
				d_start = dataInfo.Start;
			}
			if ( d_end.IsInfinite() ) {
				d_end = dataInfo.End;
			}
		}
		d_duration = d_end.Sub(d_start);
		d_computeStart = fort::Time::Now();
		d_lastShown = d_computeStart.Add(-DISPLAY_PERIOD);
		d_last = d_start;
		if ( showProgress ) {
			Rcpp::Rcerr << "\n\n";
		}
	}



	void Increment(const fort::Time & time) {
		if ( R_ToplevelExec(checkInt,NULL) == FALSE ) {
			throw StopIteration();
		}
		if ( d_showProgress == false ) {
			return;
		}
		auto computeEllapsed = time.Sub(d_last);
		if ( computeEllapsed < CHECK_PERIOD) {
			return;
		}
		auto now = fort::Time::Now();
		auto realEllapsed = now.Sub(d_lastShown);
		if ( realEllapsed < DISPLAY_PERIOD ) {
			return;
		}

		computeEllapsed = time.Sub(d_start);
		realEllapsed = now.Sub(d_computeStart);
		fort::Duration reminder = d_duration - computeEllapsed;
		fort::Duration ETD = reminder.Seconds() * realEllapsed.Seconds() / computeEllapsed.Seconds() * 1e9;
		double ratio = computeEllapsed.Seconds() / d_duration.Seconds();

		std::ostringstream percent;
		percent << std::fixed << std::setprecision(1) << std::right << std::setfill(' ') << std::setw(5)
		        << (100 * ratio ) << "%";
		size_t pos = std::min(std::max(0,int(71 * ratio -1)),70);
		Rcpp::Rcerr << "\r\rProcessed until "
		            << time.Round(fort::Duration::Second)
		            << ", computed "
		            << Round(computeEllapsed,fort::Duration::Minute)
		            << " over "
		            << Round(d_duration,fort::Duration::Minute)
		            << ", ETA: " << Round(ETD,fort::Duration::Second) << "\n"
		            << "["
		            << std::string('=',pos)
		            << ">"
		            << std::string(' ', 70-pos)
		            << "] "
		            << percent.str()
		            << "\n";

		d_last = time;
		d_lastShown = now;
	}
private:
	bool d_showProgress;
	fort::Time d_start,d_computeStart,d_last,d_lastShown;
	fort::Duration d_duration;
};

const fort::Duration FmProgress::DISPLAY_PERIOD = 300 * fort::Duration::Millisecond;
const fort::Duration FmProgress::CHECK_PERIOD = 10 * fort::Duration::Minute;


//[[Rcpp::export]]
SEXP pfmQueryIdentifyFrames(const ExperimentPtr & experiment,
                            const fort::Time & start,
                            const fort::Time & end,
                            bool computeZones,
                            bool showProgress,
                            bool singleThreaded) {
	DatetimeVector times(0);
	IntegerVector width,height,space;
	List positions;


	Query::IdentifyFramesArgs args;
	args.Start = start;
	args.End = end;
	args.ComputeZones = computeZones;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = true;

	FmProgress progress(*experiment,start,end,showProgress);
	try {
		Query::IdentifyFramesFunctor(*experiment,
		                             [&](const IdentifiedFrame::Ptr & frame) {
			                             progress.Increment(frame->FrameTime);

			                             positions.push_back(fmIdentifiedFrame_Positions(*frame));
			                             times.push_back(fmTime_asR(frame->FrameTime));
			                             space.push_back(frame->Space);
			                             width.push_back(frame->Width);
			                             height.push_back(frame->Height);
		                             },
		                             args);
	} catch ( const FmProgress::StopIteration & ) {}
	times.attr("class") = "POSIXct";
	return List::create(_["frames"] = DataFrame::create(_["time"]  = times,
	                                                    _["space"]  = space,
	                                                    _["height"]  = height,
	                                                    _["width"]  = width),
	                    _["positions"] = positions);
}


//[[Rcpp::export]]
SEXP pfmQueryCollideFrames(const ExperimentPtr & experiment,
                           const fort::Time & start,
                           const fort::Time & end,
                           bool showProgress,
                           bool singleThreaded) {

	DatetimeVector times(0);
	IntegerVector width,height,space,ant1,ant2,zone,indexes;
	CharacterVector types;
	List positions;

	auto fillCollisions =
		[&] (const CollisionFrame & frame, size_t current) {
			for ( const auto & c : frame.Collisions) {
				indexes.push_back(current);
				ant1.push_back(c.IDs.first);
				ant2.push_back(c.IDs.second);
				zone.push_back(c.Zone);
				types.push_back(fmInteractionTypes_asStr(c.Types));
			}
		};

	Query::QueryArgs args;
	args.Start = start;
	args.End = end;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = true;

	FmProgress progress(*experiment,start,end,showProgress);
	try {
		Query::CollideFramesFunctor(*experiment,
		                            [&](const Query::CollisionData & data) {
			                            progress.Increment(std::get<0>(data)->FrameTime);
			                            positions.push_back(fmIdentifiedFrame_Positions(*std::get<0>(data)));
			                            fillCollisions(*std::get<1>(data),positions.size());

			                            times.push_back(fmTime_asR(std::get<0>(data)->FrameTime));
			                            space.push_back(std::get<0>(data)->Space);
			                            width.push_back(std::get<0>(data)->Width);
			                            height.push_back(std::get<0>(data)->Height);
		                            },
		                            args);
	} catch ( FmProgress::StopIteration & ) {}
	times.attr("class") = "POSIXct";
	return List::create(_["frames"] = DataFrame::create(_["time"]  = times,
	                                                    _["space"]  = space,
	                                                    _["height"]  = height,
	                                                    _["width"]  = width),
	                    _["positions"] = positions,
	                    _["collisions"] = DataFrame::create(_["ant1"] = ant1,
	                                                        _["ant2"] = ant2,
	                                                        _["zone"] = zone,
	                                                        _["types"] = types,
	                                                        _["frames_row_index"] = indexes));

}

//[[Rcpp::export]]
SEXP pfmQueryComputeAntTrajectories(const ExperimentPtr & experiment,
                                    const fort::Time & start,
                                    const fort::Time & end,
                                    const fort::Duration & maximumGap,
                                    const fort::myrmidon::Matcher::Ptr & matcher,
                                    bool computeZones,
                                    bool showProgress,
                                    bool singleThreaded) {
	IntegerVector antID,space;
	DatetimeVector tStart(0);
	List trajectories;

	Query::ComputeAntTrajectoriesArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.ComputeZones = computeZones;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = true;

	FmProgress progress(*experiment,start,end,showProgress);
	try {
		Query::ComputeAntTrajectoriesFunctor(*experiment,
		                                     [&](const AntTrajectory::Ptr & t) {
			                                     trajectories.push_back(fmAntTrajectory_Positions(*t));
			                                     antID.push_back(t->Ant);
			                                     space.push_back(t->Space);
			                                     tStart.push_back(fmTime_asR(t->Start));
		                                     },
		                                     args);
	} catch ( const FmProgress::StopIteration & ) {}
	tStart.attr("class") = "POSIXct";

	return List::create(_["trajectories_summary"] = DataFrame::create(_["antID"] = antID,
	                                                                  _["space"] = space,
	                                                                  _["start"] = tStart),
	                    _["trajectories"] = trajectories);
}


SEXP pfmQueryComputeAntInteractionsFull(const ExperimentPtr & experiment,
                                        const fort::Time & start,
                                        const fort::Time & end,
                                        const fort::Duration & maximumGap,
                                        const fort::myrmidon::Matcher::Ptr & matcher,
                                        bool showProgress,
                                        bool singleThreaded) {

	IntegerVector tAnt,iAnt1,iAnt2,tSpace,iSpace,iTrajRow1,iTrajStart1,iTrajEnd1,iTrajRow2,iTrajStart2,iTrajEnd2;
	DatetimeVector tStart(0),iStart(0),iEnd(0);
	CharacterVector types,zone1,zone2;
	std::vector<DataFrame> positions;

	Query::ComputeAntInteractionsArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.SingleThreaded = singleThreaded;
	args.ReportFullTrajectories = true;
	args.AllocationInCurrentThread = true;

	FmProgress progress(*experiment,start,end,showProgress);
	std::map<AntTrajectory*,std::pair<std::vector<size_t>,std::vector<size_t>>> needsIndexing;
	auto storeTrajectories =
		[&] (const AntTrajectory::Ptr & t) {
			positions.push_back(fmAntTrajectory_Positions(*t));
			tAnt.push_back(t->Ant);
			tSpace.push_back(t->Space);
			tStart.push_back(fmTime_asR(t->Start));
			size_t tIndex = positions.size();
			for ( const auto & iIndex : needsIndexing[t.get()].first ) {
				iTrajRow1[iIndex] = tIndex;
			}
			for ( const auto & iIndex : needsIndexing[t.get()].second ) {
				iTrajRow2[iIndex] = tIndex;
			}
			needsIndexing.erase(t.get());
			progress.Increment(t->End());
		};

	auto storeInteractions =
		[&] (const AntInteraction::Ptr & i) {
			size_t iIndex = iAnt1.size();
			needsIndexing[std::get<0>(i->Trajectories).first.Trajectory.get()].first.push_back(iIndex);
			needsIndexing[std::get<0>(i->Trajectories).second.Trajectory.get()].second.push_back(iIndex);
			iAnt1.push_back(i->IDs.first);
			iAnt2.push_back(i->IDs.second);
			iStart.push_back(fmTime_asR(i->Start));
			iEnd.push_back(fmTime_asR(i->End));
			iSpace.push_back(i->Space);
			types.push_back(fmInteractionTypes_asStr(i->Types));
			iTrajRow1.push_back(0);
			iTrajStart1.push_back(std::get<0>(i->Trajectories).first.Begin+1);
			iTrajEnd1.push_back(std::get<0>(i->Trajectories).first.End);
			iTrajRow2.push_back(0);
			iTrajStart2.push_back(std::get<0>(i->Trajectories).second.Begin+1);
			iTrajEnd2.push_back(std::get<0>(i->Trajectories).second.End);
		};

	try {
		Query::ComputeAntInteractionsFunctor(*experiment,
		                                     storeTrajectories,
		                                     storeInteractions,
		                                     args);
	} catch ( const FmProgress::StopIteration & ) {}

	tStart.attr("class") = "POSIXct";
	iStart.attr("class") = "POSIXct";
	iEnd.attr("class") = "POSIXct";
	return List::create(_["trajectories_summary"]  = DataFrame::create(_["antID"] = tAnt,
	                                                                   _["space"] = tSpace,
	                                                                   _["start"] = tStart),
	                    _["trajectories"] = positions,
	                    _["interactions"] = DataFrame::create(_["ant1"] = iAnt1,
	                                                          _["ant2"] = iAnt2,
	                                                          _["start"] = iStart,
	                                                          _["end"] = iEnd,
	                                                          _["space"] = iSpace,
	                                                          _["types"] = types,
	                                                          _["ant1.trajectory.row"]  = iTrajRow1,
	                                                          _["ant1.trajectory.start"]  = iTrajStart1,
	                                                          _["ant1.trajectory.end"]  = iTrajEnd1,
	                                                          _["ant2.trajectory.row"]  = iTrajRow2,
	                                                          _["ant2.trajectory.start"]  = iTrajStart2,
	                                                          _["ant2.trajectory.end"]  = iTrajEnd2)
	                    );
}


SEXP pfmQueryComputeAntInteractionsSummarized(const ExperimentPtr & experiment,
                                              const fort::Time & start,
                                              const fort::Time & end,
                                              const fort::Duration & maximumGap,
                                              const fort::myrmidon::Matcher::Ptr & matcher,
                                              bool showProgress,
                                              bool singleThreaded) {
	IntegerVector tAnt,iAnt1,iAnt2,tSpace,iSpace;
	NumericVector iMx1,iMy1,iMa1,iMx2,iMy2,iMa2;
	DatetimeVector tStart(0),iStart(0),iEnd(0);
	CharacterVector types,zone1,zone2;

	Query::ComputeAntInteractionsArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.SingleThreaded = singleThreaded;
	args.ReportFullTrajectories = false;
	args.AllocationInCurrentThread = true;

	FmProgress progress(*experiment,start,end,showProgress);
	auto storeTrajectories =
		[&progress] (const AntTrajectory::Ptr & t) {
			progress.Increment(t->End());
		};
	auto storeInteractions =
		[&] (const AntInteraction::Ptr & i) {
			iAnt1.push_back(i->IDs.first);
			iAnt2.push_back(i->IDs.second);
			iStart.push_back(fmTime_asR(i->Start));
			iEnd.push_back(fmTime_asR(i->End));
			iSpace.push_back(i->Space);
			types.push_back(fmInteractionTypes_asStr(i->Types));
			iMx1.push_back(std::get<1>(i->Trajectories).first.Mean.x());
			iMy1.push_back(std::get<1>(i->Trajectories).first.Mean.y());
			iMa1.push_back(std::get<1>(i->Trajectories).first.Mean.z());
			iMx2.push_back(std::get<1>(i->Trajectories).second.Mean.x());
			iMy2.push_back(std::get<1>(i->Trajectories).second.Mean.y());
			iMa2.push_back(std::get<1>(i->Trajectories).second.Mean.z());
			zone1.push_back(fmInteraction_ZoneList(std::get<1>(i->Trajectories).first.Zones));
			zone2.push_back(fmInteraction_ZoneList(std::get<1>(i->Trajectories).second.Zones));
		};

	try {
		Query::ComputeAntInteractionsFunctor(*experiment,
		                                     storeTrajectories,
		                                     storeInteractions,
		                                     args);
	} catch ( const FmProgress::StopIteration & ) {}
	iStart.attr("class") = "POSIXct";
	iEnd.attr("class") = "POSIXct";
	return DataFrame::create(_["ant1"] = iAnt1,
	                         _["ant2"] = iAnt2,
	                         _["start"] = iStart,
	                         _["end"] = iEnd,
	                         _["space"] = iSpace,
	                         _["types"] = types,
	                         _["ant1.mean.x"]  = iMx1,
	                         _["ant1.mean.y"]  = iMy1,
	                         _["ant1.mean.angle"]  = iMa1,
	                         _["ant2.mean.x"]  = iMx2,
	                         _["ant2.mean.y"]  = iMy2,
	                         _["ant2.mean.angle"]  = iMa2,
	                         _["ant1.zones"] = zone1,
	                         _["ant2.zones"] = zone2
	                         );
}


//[[Rcpp::export]]
SEXP pfmQueryComputeAntInteractions(const ExperimentPtr & experiment,
                                    const fort::Time & start,
                                    const fort::Time & end,
                                    const fort::Duration & maximumGap,
                                    const fort::myrmidon::Matcher::Ptr & matcher,
                                    bool reportFullTrajectories,
                                    bool showProgress,
                                    bool singleThreaded) {
	if ( reportFullTrajectories == true ) {
		return pfmQueryComputeAntInteractionsFull(experiment,start,end,maximumGap,matcher,showProgress,singleThreaded);
	}
	return pfmQueryComputeAntInteractionsSummarized(experiment,start,end,maximumGap,matcher,showProgress,singleThreaded);
}

//' Collects tracking data information on the fmExperiment
//' @param experiment the \code{\link{fmExperiment}} to query
//' @return a names list with the following items:
//'   \itemize{\code{$frames}: the total number of
//'   frames.\item\code{$start} the first tracked time in the
//'   experiment. \item\code{$end} the last tracked
//'   time. \item\code{$details}: a \code{data.frame} listing the start,
//'   end and number of frame in each tracking data directory and space of
//'   the experiment.}
//' @family fmQuery methods
//[[Rcpp::export]]
fort::myrmidon::ExperimentDataInfo fmQueryGetDataInformations(const ExperimentPtr & experiment) {
	return fort::myrmidon::Query::GetDataInformations(*experiment);
}

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::ExperimentDataInfo & infos) {
	CharacterVector tddURIs,tddPaths,spaceURIs,spaceNames;
	IntegerVector tddFrames,spaceIDs,spaceFrames;
	DatetimeVector tddStart(0),tddEnd(0),spaceStart(0),spaceEnd(0);

	for ( const auto & [spaceID,space] : infos.Spaces) {
		for ( const auto & tdd : space.TrackingDataDirectories ) {
			tddURIs.push_back(tdd.URI);
			tddPaths.push_back(tdd.AbsoluteFilePath);
			tddFrames.push_back(tdd.Frames);
			tddStart.push_back(fmTime_asR(tdd.Start));
			tddEnd.push_back(fmTime_asR(tdd.End));

			spaceIDs.push_back(spaceID);
			spaceURIs.push_back(space.URI);
			spaceNames.push_back(space.Name);
			spaceFrames.push_back(space.Frames);
			spaceStart.push_back(fmTime_asR(space.Start));
			spaceEnd.push_back(fmTime_asR(space.End));
		}
	}
	spaceStart.attr("class") = "POSIXct";
	spaceEnd.attr("class") = "POSIXct";
	tddStart.attr("class") = "POSIXct";
	tddEnd.attr("class") = "POSIXct";
	List res;
	res["frames"] = infos.Frames;
	res["start"]  = fmTime_asR(infos.Start);
	res["end"] = fmTime_asR(infos.End);
	res["details"] = DataFrame::create(_["space.ID"]     = spaceIDs,
	                                   _["space.name"]   = spaceNames,
	                                   _["space.frames"] = spaceFrames,
	                                   _["space.start"]  = spaceStart,
	                                   _["space.end"]    = spaceEnd,
	                                   _["tdd.URI"]      = tddURIs,
	                                   _["tdd.path"]     = tddPaths,
	                                   _["tdd.frames"]   = tddFrames,
	                                   _["tdd.start"]    = tddStart,
	                                   _["tdd.end"]      = tddEnd);
	return res;
}

}
