#include "utestdata.h"

#include <fort/myrmidon/utest-data/UTestData.hpp>

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::UTestData::TDDInfo & tddInfo);
template <> SEXP wrap(const fort::myrmidon::UTestData::ExperimentInfo & tddInfo);

}

#include "Rcpp.h"

namespace Rcpp {

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

template <> SEXP wrap(const fort::myrmidon::UTestData::ExperimentInfo & info) {
	auto res = List::create( Named("AbsoluteFilePath") = info.AbsoluteFilePath.c_str(),
	                         Named("Version") = info.Version.c_str());

	return res;
}

}






//' Creates data for unit test
//' @return internal data structure for unit test
//[[Rcpp::export]]
Rcpp::List fmUTestDataCreate() {
	using namespace fort::myrmidon;
	using namespace Rcpp;
	auto path = UTestData::TempDirName();
	UTestData udata(path);

	Rcpp::List res;
	res.push_back(udata.Basedir().c_str(),"Basedir");
	Rcpp::List NestDataDirs,ForagingDataDirs;
	for ( const auto & tddInfo : udata.NestDataDirs() ) {
		NestDataDirs.push_back(wrap(tddInfo));
	}
	res.push_back(NestDataDirs,"NestDataDirs");
	for ( const auto & tddInfo : udata.ForagingDataDirs() ) {
		ForagingDataDirs.push_back(wrap(tddInfo));
	}
	res.push_back(ForagingDataDirs,"ForagingDataDirs");
	res.push_back(wrap(udata.NoConfigDataDir()),"NoConfigDataDir");
	res.push_back(wrap(udata.NoFamilyDataDir()),"NoFamilyDataDir");
	res.push_back(wrap(udata.ARTagDataDir()),"ARTagDataDir");
	res.push_back(wrap(udata.WithVideoDataDir()),"WithVideoDataDir");

	res.push_back(wrap(udata.CurrentVersionFile()),"CurrentVersionFile");
	res.push_back(wrap(udata.V0_1_File()),"V0_1_File");

	return res;
}
