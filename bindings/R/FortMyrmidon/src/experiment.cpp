#include "experiment.h"

#include "Rcpp.h"
#include "helper.h"
#include "wrapMap.h"

IMPLEMENT_METHOD_X(fmExperiment,
                   ExperimentPtr,
                   fort::myrmidon::SpaceByID,
                   Spaces);
IMPLEMENT_METHOD_X(fmExperiment,ExperimentPtr,
                   fort::myrmidon::Space::Ptr,
                   CreateSpace,
                   const std::string &,name);
IMPLEMENT_METHOD_X(fmExperiment,ExperimentPtr,
                   fort::myrmidon::Ant::Ptr,
                   CreateAnt);
IMPLEMENT_METHOD_X(fmExperiment,ExperimentPtr,
                   fort::myrmidon::Identification::Ptr,
                   AddIdentification,
                   fort::myrmidon::AntID, antID,
                   fort::myrmidon::TagID, tagID,
                   const fort::Time &, start,
                   const fort::Time &,  end);


RCPP_MODULE(experiment) {
	using namespace Rcpp;
	class_<ExperimentPtr>("fmExperiment")
		.method("createSpace",&fmExperiment_CreateSpace)
		.method("createAnt",&fmExperiment_CreateAnt)
		.method("addIdentification",&fmExperiment_AddIdentification)
		.property("spaces",&fmExperiment_Spaces,"Spaces in this experiment");
}



//' @title Creates an Experiement.
//' @description No file will be created a filepath, but it is
//'   required to determine relative path to the eventual Tracking
//'   Data Directory that will be added to the experiment.
//' @param filepath to use for the experiment.
//' @return a \link{\code{fmExperiment}} with the associated filepath
//' @family fmExperiment methods
//[[Rcpp::export]]
ExperimentPtr fmExperimentCreate(const std::string & filepath) {
	using namespace fort::myrmidon;
	return ExperimentPtr(new Experiment(Experiment::CreateUnsafe(filepath)));
}

//' Opens an Experiment
//' @param filepath the path to the '.myrmidon' file
//' @return a \link{\code{fmExperiment}}
//' @family fmExperiment methods
//[[Rcpp::export]]
ExperimentPtr fmExperimentOpen(const std::string & filepath) {
	using namespace fort::myrmidon;
	return ExperimentPtr(new Experiment(Experiment::OpenUnsafe(filepath)));
}

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::SpaceByID & spaces) {
	return wrapMap(spaces);
}

}
