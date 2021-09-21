#include "experiment.h"

#include "Rcpp.h"



inline fort::myrmidon::Space::Ptr fmExperiment_CreateSpace(ExperimentPtr * e,
                                                           const std::string & name) {
	return (*e)->CreateSpace(name);
}


RCPP_MODULE(experiment) {
	using namespace Rcpp;
	class_<ExperimentPtr>("fmExperiment")
		.method("createSpace",&fmExperiment_CreateSpace)
		;
}

//' Creates an Experiement
//' @param filepath the name for the newly created space
//' @return a \link{\code{fmExperiment}} with name
//[[Rcpp::export]]
ExperimentPtr fmExperimentCreate(const std::string & filepath) {
	using namespace fort::myrmidon;
	return ExperimentPtr(new Experiment(Experiment::CreateUnsafe(filepath)));
}
