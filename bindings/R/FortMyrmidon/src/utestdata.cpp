#include "utestdata.h"

#include <fort/myrmidon/utest-data/UTestData.hpp>

#include "types.h"
#include "time.h"

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::UTestData::TDDInfo & tddInfo);
template <> SEXP wrap(const fort::myrmidon::UTestData::ExperimentInfo & tddInfo);
template <> SEXP wrap(const fs::path & path);
template <> SEXP wrap(const std::vector<fort::myrmidon::UTestData::TDDInfo> & infos);
template <> SEXP wrap(const fort::myrmidon::UTestData::UTestData::ExpectedResult &);
}

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::UTestData);

#include "Rcpp.h"

std::tuple<SEXP,SEXP,SEXP>
wrapFull(const std::vector<fort::myrmidon::AntTrajectory::Ptr> & trajectories,
         const std::vector<fort::myrmidon::AntInteraction::Ptr> & interactions);
std::tuple<SEXP,SEXP>
wrapTrajectories(const std::vector<fort::myrmidon::AntTrajectory::Ptr> & trajectories);

SEXP wrapSummarized(const std::vector<fort::myrmidon::AntInteraction::Ptr> & interactions);

namespace Rcpp {

template <> SEXP wrap(const fs::path & path) {
	return wrap(path.c_str());
}

template <> SEXP wrap(const fort::myrmidon::UTestData::TDDInfo & tddInfo) {
	auto res = List::create( Named("AbsoluteFilePath") = tddInfo.AbsoluteFilePath.c_str(),
	                         Named("Family") = int(tddInfo.Family),
	                         Named("HasFullFrame") = tddInfo.HasFullFrame,
	                         Named("HasMovie") = tddInfo.HasMovie,
	                         Named("HasConfig") = tddInfo.HasConfig,
	                         Named("Start") = wrap(tddInfo.Start),
	                         Named("End") = wrap(tddInfo.End),
	                         Named("StartFrame") = tddInfo.StartFrame,
	                         Named("EndFrame") = tddInfo.EndFrame);

	return res;
}

template <> SEXP wrap(const std::vector<fort::myrmidon::UTestData::TDDInfo> & infos) {
	return wrap(infos.begin(),infos.end());
}

template <> SEXP wrap(const fort::myrmidon::UTestData::ExperimentInfo & info) {
	auto res = List::create( Named("AbsoluteFilePath") = info.AbsoluteFilePath.c_str(),
	                         Named("Version") = info.Version.c_str());

	return res;
}


template <> SEXP wrap(const fort::myrmidon::UTestData::UTestData::ExpectedResult & result) {
	auto [interactions_summary,interactions_trajectories,interactions] = wrapFull(result.InteractionTrajectories,result.Interactions);
	auto [trajectories_summary,trajectories] = wrapTrajectories(result.Trajectories);
	return List::create(_["Start"] = result.Start,
	                    _["End"] = result.End,
	                    _["MaximumGap"] = result.MaximumGap,
	                    _["Matches"] = result.Matches,
	                    _["trajectories_summary"] = trajectories_summary,
	                    _["trajectories"] = trajectories,
	                    _["interactions_summary"] = interactions_summary,
	                    _["interactions_trajectories"] = interactions_trajectories,
	                    _["interactions"] = interactions,
	                    _["summarized_interactions"] = wrapSummarized(result.Summarized()));
}


}

fort::myrmidon::UTestData* fmUTestDataCreate() {
	auto path = fort::myrmidon::UTestData::TempDirName();
	return new fort::myrmidon::UTestData(path);
}


SEXP pfmUTestData_ExpectedFrames(fort::myrmidon::UTestData * ud) {
	using namespace Rcpp;
	DatetimeVector times(0);
	IntegerVector width,height,space,ant1,ant2,zone,indexes;
	CharacterVector types;
	List positions;

	auto fillCollisions =
		[&] (const fort::myrmidon::CollisionFrame & frame, size_t current) {
			for ( const auto & c : frame.Collisions) {
				indexes.push_back(current);
				ant1.push_back(c.IDs.first);
				ant2.push_back(c.IDs.second);
				zone.push_back(c.Zone);
				types.push_back(fmInteractionTypes_asStr(c.Types));
			}
		};

	for ( const auto & [identified,collided] : ud->ExpectedFrames() ) {
		positions.push_back(fmIdentifiedFrame_Positions(*identified));
		fillCollisions(*collided,positions.size());

		times.push_back(fmTime_asR(identified->FrameTime));
		space.push_back(identified->Space);
		width.push_back(identified->Width);
		height.push_back(identified->Height);
	}
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



SEXP pfmUTestData_ExpectedResults(fort::myrmidon::UTestData * ud) {
	return Rcpp::wrap(ud->ExpectedResults().begin(),ud->ExpectedResults().end());
}

RCPP_MODULE(utestdata) {
	using namespace Rcpp;
	using namespace fort::myrmidon;

	class_<fort::myrmidon::UTestData>("pfmUTestData")
		.property("Basedir",&UTestData::Basedir,"")
		.property("NestDataDirs",&UTestData::NestDataDirs,"")
		.property("ForagingDataDirs",&UTestData::ForagingDataDirs,"")
		.property("NoConfigDataDir",&UTestData::NoConfigDataDir,"")
		.property("ARTagDataDir",&UTestData::ARTagDataDir,"")
		.property("WithVideoDataDir",&UTestData::WithVideoDataDir,"")
		.property("CurrentVersionFile",&UTestData::CurrentVersionFile,"")
		.property("ExpectedTagStatistics",&UTestData::ExpectedTagStatistics,"")
		.property("ExpectedFrames",&pfmUTestData_ExpectedFrames,"")
		.property("ExpectedResults",&pfmUTestData_ExpectedResults,"")
		.property("V0_1_File",&UTestData::V0_1_File,"")
		;

	function("pfmUTestDataCreate",&fmUTestDataCreate);
}

std::tuple<SEXP,SEXP> wrapTrajectories(const std::vector<fort::myrmidon::AntTrajectory::Ptr> & trajectories) {
	using namespace Rcpp;
	IntegerVector antID,space;
	DatetimeVector start(0);
	List trajs;
	for ( const auto & t : trajectories ) {
		antID.push_back(t->Ant);
		start.push_back(fmTime_asR(t->Start));
		space.push_back(t->Space);
		trajs.push_back(fmAntTrajectory_Positions(*t));
	}
	start.attr("class") = "POSIXct";
	return { DataFrame::create(_["antID"] = antID,
	                           _["space"] = space,
	                           _["start"] = start),
	         trajs };
}



std::tuple<SEXP,SEXP,SEXP> wrapFull(const std::vector<fort::myrmidon::AntTrajectory::Ptr> & trajectories,
                                    const std::vector<fort::myrmidon::AntInteraction::Ptr> & interactions) {
	using namespace Rcpp;
	auto [summary,trajs] = wrapTrajectories(trajectories);
	auto findIndex =
		[&trajectories](const fort::myrmidon::AntTrajectory::Ptr & t) {
			return size_t(std::find_if(trajectories.begin(),
			                           trajectories.end(),
			                           [&t](const fort::myrmidon::AntTrajectory::Ptr & it) {
				                           return it.get() == t.get();
			                           }) - trajectories.begin()) + 1;
		};

	IntegerVector iAnt1,iAnt2,tSpace,iSpace,iTrajRow1,iTrajStart1,iTrajEnd1,iTrajRow2,iTrajStart2,iTrajEnd2;
	DatetimeVector iStart(0),iEnd(0);
	CharacterVector types;
	for ( const auto & i : interactions ) {

		iAnt1.push_back(i->IDs.first);
		iAnt2.push_back(i->IDs.second);
		iStart.push_back(fmTime_asR(i->Start));
		iEnd.push_back(fmTime_asR(i->End));
		iSpace.push_back(i->Space);
		types.push_back(fmInteractionTypes_asStr(i->Types));
		iTrajRow1.push_back(findIndex(std::get<0>(i->Trajectories).first.Trajectory));
		iTrajStart1.push_back(std::get<0>(i->Trajectories).first.Begin+1);
		iTrajEnd1.push_back(std::get<0>(i->Trajectories).first.End);
		iTrajRow2.push_back(findIndex(std::get<0>(i->Trajectories).second.Trajectory));
		iTrajStart2.push_back(std::get<0>(i->Trajectories).second.Begin+1);
		iTrajEnd2.push_back(std::get<0>(i->Trajectories).second.End);
	}

	iStart.attr("class") = "POSIXct";
	iEnd.attr("class") = "POSIXct";
	SEXP inters = DataFrame::create(_["ant1"] = iAnt1,
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
	                                _["ant2.trajectory.end"]  = iTrajEnd2);

	return {summary,trajs,inters};
}

SEXP wrapSummarized(const std::vector<fort::myrmidon::AntInteraction::Ptr> & interactions) {
	using namespace Rcpp;
	IntegerVector tAnt,iAnt1,iAnt2,tSpace,iSpace;
	NumericVector iMx1,iMy1,iMa1,iMx2,iMy2,iMa2;
	DatetimeVector tStart(0),iStart(0),iEnd(0);
	CharacterVector types,zone1,zone2;

	for ( const auto & i : interactions ) {
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
	}

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
