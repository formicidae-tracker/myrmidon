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

		std::ostringstream firstLine;
		firstLine << "Processed until "
		          << time.Round(fort::Duration::Second)
		          << ", computed "
		          << Round(computeEllapsed,fort::Duration::Minute)
		          << " over "
		          << Round(d_duration,fort::Duration::Minute)
		          << ", ETA: " << Round(ETD,fort::Duration::Second);

		size_t pos = std::min(std::max(0,int(71 * ratio -1)),70);
		Rcpp::Rcerr << std::left << std::setw(80) << firstLine.str() << "\n"
		            << "["
		            << std::string(pos,'=')
		            << ">"
		            << std::string(70-pos,' ')
		            << "] "
		            << percent.str()
		            << "\r\r" << std::flush;

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
	std::vector<double> times;
	std::vector<int32_t> width,height,space;
	std::vector<DataFrame> positions;


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
			                             times.push_back(frame->FrameTime.Sub(fort::Time()).Seconds());
			                             space.push_back(frame->Space);
			                             width.push_back(frame->Width);
			                             height.push_back(frame->Height);
		                             },
		                             args);
	} catch ( const FmProgress::StopIteration & ) {}
	auto ttimes = DatetimeVector::import(times.begin(),times.end());
	ttimes.attr("class") = "POSIXct";
	return List::create(_["frames"] = DataFrame::create(_["time"]  = ttimes,
	                                                    _["space"]  = IntegerVector::import(space.begin(),space.end()),
	                                                    _["height"]  = IntegerVector::import(height.begin(),height.end()),
	                                                    _["width"]  = IntegerVector::import(width.begin(),width.end())),
	                    _["positions"] = List::import(positions.begin(),positions.end()));
}


//[[Rcpp::export]]
SEXP pfmQueryCollideFrames(const ExperimentPtr & experiment,
                           const fort::Time & start,
                           const fort::Time & end,
                           bool collisionsIgnoreZones,
                           bool showProgress,
                           bool singleThreaded) {

	std::vector<double> times(0);
	std::vector<int32_t> width,height,space,ant1,ant2,zone,indexes;
	std::vector<std::string> types;
	std::vector<DataFrame> positions;

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

	Query::CollideFramesArgs args;
	args.Start = start;
	args.End = end;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = true;
	args.CollisionsIgnoreZones = collisionsIgnoreZones;

	FmProgress progress(*experiment,start,end,showProgress);
	try {
		Query::CollideFramesFunctor(*experiment,
		                            [&](const CollisionData & data) {
			                            progress.Increment(std::get<0>(data)->FrameTime);
			                            positions.push_back(fmIdentifiedFrame_Positions(*std::get<0>(data)));
			                            fillCollisions(*std::get<1>(data),positions.size());

			                            times.push_back(std::get<0>(data)->FrameTime.Sub(fort::Time()).Seconds());
			                            space.push_back(std::get<0>(data)->Space);
			                            width.push_back(std::get<0>(data)->Width);
			                            height.push_back(std::get<0>(data)->Height);
		                            },
		                            args);
	} catch ( FmProgress::StopIteration & ) {}
	auto ttimes = DatetimeVector::import(times.begin(),times.end());
	ttimes.attr("class") = "POSIXct";
	return List::create(_["frames"] = DataFrame::create(_["time"]   = ttimes,
	                                                    _["space"]  = IntegerVector::import(space.begin(),space.end()),
	                                                    _["height"] = IntegerVector::import(height.begin(),height.end()),
	                                                    _["width"]  = IntegerVector::import(width.begin(),width.end())),
	                    _["positions"] = List::import(positions.begin(),positions.end()),
	                    _["collisions"] = DataFrame::create(_["ant1"] = IntegerVector::import(ant1.begin(),ant1.end()),
	                                                        _["ant2"] = IntegerVector::import(ant2.begin(),ant2.end()),
	                                                        _["zone"] = IntegerVector::import(zone.begin(),zone.end()),
	                                                        _["types"] = CharacterVector::import(types.begin(),types.end()),
	                                                        _["frames_row_index"] = IntegerVector::import(indexes.begin(),indexes.end())));
}

//[[Rcpp::export]]
SEXP pfmQueryComputeAntTrajectories(const ExperimentPtr & experiment,
                                    const fort::Time & start,
                                    const fort::Time & end,
                                    const fort::Duration & maximumGap,
                                    const fort::myrmidon::Matcher::Ptr & matcher,
                                    bool computeZones,
                                    bool segmentOnMatcherValueChange,
                                    bool showProgress,
                                    bool singleThreaded) {
	std::vector<int32_t> antID,space;
	std::vector<double> tStart;
	std::vector<DataFrame> trajectories;

	Query::ComputeAntTrajectoriesArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.ComputeZones = computeZones;
	args.SingleThreaded = singleThreaded;
	args.AllocationInCurrentThread = true;
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;
	FmProgress progress(*experiment,start,end,showProgress);
	try {
		Query::ComputeAntTrajectoriesFunctor(*experiment,
		                                     [&](const AntTrajectory::Ptr & t) {
			                                     trajectories.push_back(fmAntTrajectory_Positions(*t));
			                                     antID.push_back(t->Ant);
			                                     space.push_back(t->Space);
			                                     tStart.push_back(t->Start.Sub(fort::Time()).Seconds());
		                                     },
		                                     args);
	} catch ( const FmProgress::StopIteration & ) {}
	auto ttStart = DatetimeVector::import(tStart.begin(),tStart.end());
	ttStart.attr("class") = "POSIXct";

	return List::create(_["trajectories_summary"] = DataFrame::create(_["antID"] = IntegerVector::import(antID.begin(),antID.end()),
	                                                                  _["space"] = IntegerVector::import(space.begin(),space.end()),
	                                                                  _["start"] = ttStart),
	                    _["trajectories"] = List::import(trajectories.begin(),trajectories.end()));
}


SEXP pfmQueryComputeAntInteractionsFull(const ExperimentPtr & experiment,
                                        const fort::Time & start,
                                        const fort::Time & end,
                                        const fort::Duration & maximumGap,
                                        const fort::myrmidon::Matcher::Ptr & matcher,
                                        bool collisionsIgnoreZones,
                                        bool segmentOnMatcherValueChange,
                                        bool showProgress,
                                        bool singleThreaded) {

	std::vector<int32_t> tAnt,iAnt1,iAnt2,tSpace,iSpace,iTrajRow1,iTrajStart1,iTrajEnd1,iTrajRow2,iTrajStart2,iTrajEnd2;
	std::vector<double> tStart(0),iStart(0),iEnd(0);
	std::vector<std::string> types,zone1,zone2;
	std::vector<DataFrame> positions;

	Query::ComputeAntInteractionsArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.SingleThreaded = singleThreaded;
	args.ReportFullTrajectories = true;
	args.AllocationInCurrentThread = true;
	args.CollisionsIgnoreZones = collisionsIgnoreZones;
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;

	FmProgress progress(*experiment,start,end,showProgress);
	std::map<AntTrajectory*,std::pair<std::vector<size_t>,std::vector<size_t>>> needsIndexing;
	auto storeTrajectories =
		[&] (const AntTrajectory::Ptr & t) {
			positions.push_back(fmAntTrajectory_Positions(*t));
			tAnt.push_back(t->Ant);
			tSpace.push_back(t->Space);
			tStart.push_back(t->Start.Sub(fort::Time()).Seconds());
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
			iStart.push_back(i->Start.Sub(fort::Time()).Seconds());
			iEnd.push_back(i->End.Sub(fort::Time()).Seconds());
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

	auto ttStart = DatetimeVector::import(tStart.begin(),tStart.end());
	auto iiStart = DatetimeVector::import(iStart.begin(),iStart.end());
	auto iiEnd = DatetimeVector::import(iEnd.begin(),iEnd.end());
	ttStart.attr("class") = "POSIXct";
	iiStart.attr("class") = "POSIXct";
	iiEnd.attr("class") = "POSIXct";
	return List::create(_["trajectories_summary"]  = DataFrame::create(_["antID"] = IntegerVector::import(tAnt.begin(),tAnt.end()),
	                                                                   _["space"] = IntegerVector::import(tSpace.begin(),tSpace.end()),
	                                                                   _["start"] = ttStart),
	                    _["trajectories"] = positions,
	                    _["interactions"] = DataFrame::create(_["ant1"] = IntegerVector::import(iAnt1.begin(),iAnt1.end()),
	                                                          _["ant2"] = IntegerVector::import(iAnt2.begin(),iAnt2.end()),
	                                                          _["start"] = iiStart,
	                                                          _["end"] = iiEnd,
	                                                          _["space"] = IntegerVector::import(iSpace.begin(),iSpace.end()),
	                                                          _["types"] = CharacterVector::import(types.begin(),types.end()),
	                                                          _["ant1.trajectory.row"]  = IntegerVector::import(iTrajRow1.begin(),iTrajRow1.end()),
	                                                          _["ant1.trajectory.start"]  = IntegerVector::import(iTrajStart1.begin(),iTrajStart1.end()),
	                                                          _["ant1.trajectory.end"]  = IntegerVector::import(iTrajEnd1.begin(),iTrajEnd1.end()),
	                                                          _["ant2.trajectory.row"]  = IntegerVector::import(iTrajRow2.begin(),iTrajRow2.end()),
	                                                          _["ant2.trajectory.start"]  = IntegerVector::import(iTrajStart2.begin(),iTrajStart2.end()),
	                                                          _["ant2.trajectory.end"]  = IntegerVector::import(iTrajEnd2.begin(),iTrajEnd2.end()))
	                    );
}


SEXP pfmQueryComputeAntInteractionsSummarized(const ExperimentPtr & experiment,
                                              const fort::Time & start,
                                              const fort::Time & end,
                                              const fort::Duration & maximumGap,
                                              const fort::myrmidon::Matcher::Ptr & matcher,
                                              bool collisionsIgnoreZones,
                                              bool segmentOnMatcherValueChange,
                                              bool showProgress,
                                              bool singleThreaded) {
	std::vector<int32_t> tAnt,iAnt1,iAnt2,tSpace,iSpace;
	std::vector<double> iMx1,iMy1,iMa1,iMx2,iMy2,iMa2;
	std::vector<double> iStart(0),iEnd(0);
	std::vector<std::string> types,zone1,zone2;

	Query::ComputeAntInteractionsArgs args;
	args.Start = start;
	args.End = end;
	args.MaximumGap = maximumGap;
	args.Matcher = matcher;
	args.SingleThreaded = singleThreaded;
	args.ReportFullTrajectories = false;
	args.AllocationInCurrentThread = true;
	args.CollisionsIgnoreZones = collisionsIgnoreZones;
	args.SegmentOnMatcherValueChange = segmentOnMatcherValueChange;

	FmProgress progress(*experiment,start,end,showProgress);
	auto storeTrajectories =
		[&progress] (const AntTrajectory::Ptr & t) {
		};
	auto storeInteractions =
		[&] (const AntInteraction::Ptr & i) {
			iAnt1.push_back(i->IDs.first);
			iAnt2.push_back(i->IDs.second);
			iStart.push_back(i->Start.Sub(fort::Time()).Seconds());
			iEnd.push_back(i->End.Sub(fort::Time()).Seconds());
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
			progress.Increment(i->End);
		};

	try {
		Query::ComputeAntInteractionsFunctor(*experiment,
		                                     storeTrajectories,
		                                     storeInteractions,
		                                     args);
	} catch ( const FmProgress::StopIteration & ) {}

	auto iiStart = DatetimeVector::import(iStart.begin(),iStart.end());
	auto iiEnd = DatetimeVector::import(iEnd.begin(),iEnd.end());
	iiStart.attr("class") = "POSIXct";
	iiEnd.attr("class") = "POSIXct";
	return DataFrame::create(_["ant1"] = IntegerVector::import(iAnt1.begin(),iAnt1.end()),
	                         _["ant2"] = IntegerVector::import(iAnt2.begin(),iAnt2.end()),
	                         _["start"] = iiStart,
	                         _["end"] = iiEnd,
	                         _["space"] = IntegerVector::import(iSpace.begin(),iSpace.end()),
	                         _["types"] = CharacterVector::import(types.begin(),types.end()),
	                         _["ant1.mean.x"]  = NumericVector::import(iMx1.begin(),iMx1.end()),
	                         _["ant1.mean.y"]  = NumericVector::import(iMy1.begin(),iMy1.end()),
	                         _["ant1.mean.angle"]  = NumericVector::import(iMa1.begin(),iMa1.end()),
	                         _["ant2.mean.x"]  = NumericVector::import(iMx2.begin(),iMx2.end()),
	                         _["ant2.mean.y"]  = NumericVector::import(iMy2.begin(),iMy2.end()),
	                         _["ant2.mean.angle"]  = NumericVector::import(iMa2.begin(),iMa2.end()),
	                         _["ant1.zones"] = CharacterVector::import(zone1.begin(),zone1.end()),
	                         _["ant2.zones"] = CharacterVector::import(zone2.begin(),zone2.end())
	                         );
}


//[[Rcpp::export]]
SEXP pfmQueryComputeAntInteractions(const ExperimentPtr & experiment,
                                    const fort::Time & start,
                                    const fort::Time & end,
                                    const fort::Duration & maximumGap,
                                    const fort::myrmidon::Matcher::Ptr & matcher,
                                    bool collisionsIgnoreZones,
                                    bool reportFullTrajectories,
                                    bool segmentOnMatcherValueChange,
                                    bool showProgress,
                                    bool singleThreaded) {
	if ( reportFullTrajectories == true ) {
		return pfmQueryComputeAntInteractionsFull(experiment,start,end,maximumGap,matcher,collisionsIgnoreZones,segmentOnMatcherValueChange,showProgress,singleThreaded);
	}
	return pfmQueryComputeAntInteractionsSummarized(experiment,start,end,maximumGap,matcher,collisionsIgnoreZones,segmentOnMatcherValueChange,showProgress,singleThreaded);
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

template <std::size_t Index,typename Container,typename TupleType>
SEXP importVectorOfTuple(const std::vector<TupleType> & values) {
	Container res(values.size());
	for ( size_t i = 0; i < values.size(); ++i) {
		if constexpr (std::is_same_v<std::tuple_element_t<Index,TupleType>,fort::Time>) {
			res[i] = fmTime_asR(std::get<Index>(values[i]));
		} else {
			res[i] = std::get<Index>(values[i]);
		}
	}
	if constexpr( std::is_same_v<Container,Rcpp::DatetimeVector>) {
		res.attr("class") = "POSIXct";
	}
	return res;
}


//' get time ranges where a metadata key has a given value
//' @param experiment the \code{\link{fmExperiment}} to query
//' @param key the key to query for
//' @param value the value to test for
//' @return a data.frame with the antID, and the start and end time
//'    where key is equal to value.
//' @family fmQuery methods
//[[Rcpp::export]]
Rcpp::DataFrame fmQueryGetMetaDataKeyRanges(const ExperimentPtr & experiment,
                                            const std::string & key,
                                            const fort::myrmidon::Value & value) {
	auto res = fort::myrmidon::Query::GetMetaDataKeyRanges(*experiment,key,value);
	return Rcpp::DataFrame::create(Rcpp::_["antID"]  = importVectorOfTuple<0,Rcpp::IntegerVector,std::tuple<uint32_t,fort::Time,fort::Time>>(res),
	                               Rcpp::_["start"] = importVectorOfTuple<1,Rcpp::DatetimeVector,std::tuple<uint32_t,fort::Time,fort::Time>>(res),
	                               Rcpp::_["end"] = importVectorOfTuple<2,Rcpp::DatetimeVector,std::tuple<uint32_t,fort::Time,fort::Time>>(res));
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
