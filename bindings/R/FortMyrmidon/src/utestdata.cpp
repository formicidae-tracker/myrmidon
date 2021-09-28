#include "utestdata.h"

#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::UTestData::TDDInfo & tddInfo);
template <> SEXP wrap(const fort::myrmidon::UTestData::ExperimentInfo & tddInfo);
template <> SEXP wrap(const fs::path & path);
template <> SEXP wrap(const std::vector<fort::myrmidon::UTestData::TDDInfo> & infos);
}

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::UTestData);

#include "Rcpp.h"

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

}

fort::myrmidon::UTestData* fmUTestDataCreate() {
	auto path = fort::myrmidon::UTestData::TempDirName();
	return new fort::myrmidon::UTestData(path);
}


RCPP_MODULE(utestdata) {
	using namespace Rcpp;
	using namespace fort::myrmidon;

	class_<fort::myrmidon::UTestData>("fmUTestData")
		.property("Basedir",&UTestData::Basedir,"")
		.property("NestDataDirs",&UTestData::NestDataDirs,"")
		.property("ForagingDataDirs",&UTestData::ForagingDataDirs,"")
		.property("NoConfigDataDir",&UTestData::NoConfigDataDir,"")
		.property("ARTagDataDir",&UTestData::ARTagDataDir,"")
		.property("WithVideoDataDir",&UTestData::WithVideoDataDir,"")
		.property("CurrentVersionFile",&UTestData::CurrentVersionFile,"")
		.property("V0_1_File",&UTestData::V0_1_File,"")
		;

	function("fmUTestDataCreate",&fmUTestDataCreate);
}
