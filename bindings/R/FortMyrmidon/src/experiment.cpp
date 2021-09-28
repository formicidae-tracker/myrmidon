#include "experiment.h"

#include "Rcpp.h"
#include "wrapMap.h"

inline fort::myrmidon::SpaceByID fmExperiment_Spaces(ExperimentPtr * e) {
	return (*e)->Spaces();
}


inline fort::myrmidon::Space::Ptr fmExperiment_CreateSpace(ExperimentPtr * e,
                                                           const std::string & name) {
	return (*e)->CreateSpace(name);
}


RCPP_MODULE(experiment) {
	using namespace Rcpp;
	class_<ExperimentPtr>("fmExperiment")
		.method("createSpace",&fmExperiment_CreateSpace)
		.property("spaces",&fmExperiment_Spaces,"Spaces in this experiment");
}

//' Creates an Experiement.
//' @description Creates a new fmExperiment. No file will be created a
//    filepath, but it is required to determine relative path to the
//    eventual Tracking Data Directory that will be added to the
//    experiment.
//' @param filepath to use for the experiment.
//' @return a \link{\code{fmExperiment}} with the associated filepath
//[[Rcpp::export]]
ExperimentPtr fmExperimentCreate(const std::string & filepath) {
	using namespace fort::myrmidon;
	return ExperimentPtr(new Experiment(Experiment::CreateUnsafe(filepath)));
}


//' Opens an Experiment
//' @param filepath the path to the '.myrmidon' file
//' @return a \link{\code{fmExperiment}}
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
