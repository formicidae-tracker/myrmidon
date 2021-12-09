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

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        DeleteIdentification,
                        const fort::myrmidon::Identification::Ptr &, identification);


IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        Save,
                        const std::string &, filepath);

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        DeleteSpace,
                        fort::myrmidon::SpaceID, spaceID);

IMPLEMENT_GETTER_X(fmExperiment,ExperimentPtr,std::string, AbsoluteFilePath)

IMPLEMENT_METHOD_X(fmExperiment,ExperimentPtr,
                   std::string,
                   AddTrackingDataDirectory,
                   fort::myrmidon::SpaceID, spaceID,
                   const std::string &, filepath,
                   bool, fixCorruptedData);

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        RemoveTrackingDataDirectory,
                        const std::string &, URI);

IMPLEMENT_GETTER_X(fmExperiment,ExperimentPtr,const fort::myrmidon::AntByID &,Ants);

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        DeleteAnt,
                        fort::myrmidon::AntID, antID);


inline SEXP fmExperiment_FreeIdentificationRangeAt(ExperimentPtr * e,
                                                   fort::myrmidon::TagID tagID,
                                                   const fort::Time & time) {
	const auto & [start,end] = (*e)->FreeIdentificationRangeAt(tagID,time);
	return Rcpp::List::create(Rcpp::Named("start") = start,
	                          Rcpp::Named("end") = end);
}

IMPLEMENT_FIELD_X(fmExperiment,ExperimentPtr,const std::string &,Name)
IMPLEMENT_FIELD_X(fmExperiment,ExperimentPtr,const std::string &,Author)
IMPLEMENT_FIELD_X(fmExperiment,ExperimentPtr,const std::string &,Comment)
IMPLEMENT_FIELD_X(fmExperiment,ExperimentPtr,double,DefaultTagSize)

inline uint32_t fmExperiment_Family(ExperimentPtr *e) {
	return uint32_t((*e)->Family());
}

inline SEXP fmExperiment_MeasurementTypeNames(ExperimentPtr * e) {
	return wrapMap((*e)->MeasurementTypeNames());
}

IMPLEMENT_METHOD_X(fmExperiment,ExperimentPtr,
                   uint32_t,
                   CreateMeasurementType,
                   const std::string &, name);

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        SetMeasurementTypeName,
                        fort::myrmidon::MeasurementTypeID, typeID,
                        const std::string &, name);

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        DeleteMeasurementType,
                        fort::myrmidon::MeasurementTypeID, typeID);

inline SEXP fmExperiment_AntShapeTypeNames(ExperimentPtr * e) {
	return wrapMap((*e)->AntShapeTypeNames());
}

IMPLEMENT_METHOD_X(fmExperiment,ExperimentPtr,
                   uint32_t,
                   CreateAntShapeType,
                   const std::string &, name);

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        SetAntShapeTypeName,
                        fort::myrmidon::AntShapeTypeID, typeID,
                        const std::string &, name);

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        DeleteAntShapeType,
                        fort::myrmidon::AntShapeTypeID, typeID);

inline SEXP fmExperiment_MetaDataKeys(ExperimentPtr * e) {
	Rcpp::List res;
	for ( const auto & [ key, defaultValue ] : (*e)->MetaDataKeys() ) {
		res.push_back(Rcpp::wrap(defaultValue),key);
	}
	return res;
}

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        SetMetaDataKey,
                        const std::string &, key,
                        const fort::myrmidon::Value &, value);

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        DeleteMetaDataKey,
                        const std::string &, key);

IMPLEMENT_VOID_METHOD_X(fmExperiment,ExperimentPtr,
                        RenameMetaDataKey,
                        const std::string &, oldKey,
                        const std::string &, newKey);

inline SEXP fmExperiment_IdentificationsAt(ExperimentPtr * e,
                                          const fort::Time & time) {
	return wrapMap((*e)->IdentificationsAt(time,true));
}


RCPP_MODULE(experiment) {
	using namespace Rcpp;
	class_<ExperimentPtr> ("fmExperiment")
		.property("absoluteFilePath",
		          &fmExperiment_AbsoluteFilePath,
		          "the path to the experiment file")
		.property("spaces",
		          &fmExperiment_Spaces,
		          "Spaces in this experiment")
		.property("ants",&fmExperiment_Ants,"Ants in this experiment")
		.property("name",&fmExperiment_Name,&fmExperiment_SetName,"Name of the experiment")
		.property("author",&fmExperiment_Author,&fmExperiment_SetAuthor,"Author of the experiment")
		.property("comment",&fmExperiment_Comment,&fmExperiment_SetComment,"Comment of the experiment")
		.property("family",&fmExperiment_Family,"the family used in the experiment")
		.property("defaultTagSize",&fmExperiment_DefaultTagSize,&fmExperiment_SetDefaultTagSize,"Default physical tag size of the experiment")
		.property("measurementTypeNames",&fmExperiment_MeasurementTypeNames,"measurement types used in the experiment")
		.property("antShapeTypeNames",&fmExperiment_AntShapeTypeNames,"ant shape types used in the experiment")
		.property("metaDataKeys",&fmExperiment_MetaDataKeys,"user defined metadata keys used in the experiment")

		.method("save",&fmExperiment_Save)

		.method("createSpace",&fmExperiment_CreateSpace)
		.method("deleteSpace",&fmExperiment_DeleteSpace)

		.method("addTrackingDataDirectory",&fmExperiment_AddTrackingDataDirectory)
		.method("removeTrackingDataDirectory",&fmExperiment_RemoveTrackingDataDirectory)

		.method("createAnt",&fmExperiment_CreateAnt)
		.method("deleteAnt",&fmExperiment_DeleteAnt)

		.method("addIdentification",&fmExperiment_AddIdentification)
		.method("deleteIdentification",&fmExperiment_DeleteIdentification)
		.method("freeIdentificationRangeAt",&fmExperiment_FreeIdentificationRangeAt)

		.method("createMeasurementType",&fmExperiment_CreateMeasurementType)
		.method("setMeasurementTypeName",&fmExperiment_SetMeasurementTypeName)
		.method("deleteMeasurementType",&fmExperiment_DeleteMeasurementType)

		.method("createAntShapeType",&fmExperiment_CreateAntShapeType)
		.method("setAntShapeTypeName",&fmExperiment_SetAntShapeTypeName)
		.method("deleteAntShapeType",&fmExperiment_DeleteAntShapeType)

		.method("setMetaDataKey",&fmExperiment_SetMetaDataKey)
		.method("deleteMetaDataKey",&fmExperiment_DeleteMetaDataKey)
		.method("renameMetaDataKey",&fmExperiment_RenameMetaDataKey)

		.method("identificationsAt",&fmExperiment_IdentificationsAt)
		;
}



//' @title Creates an Experiement.
//' @description No file will be created a filepath, but it is
//'   required to determine relative path to the eventual Tracking
//'   Data Directory that will be added to the experiment.
//' @param filepath to use for the experiment.
//' @return a \code{\link{fmExperiment}} with the associated filepath
//' @family fmExperiment methods
//[[Rcpp::export]]
ExperimentPtr fmExperimentCreate(const std::string & filepath) {
	using namespace fort::myrmidon;
	return ExperimentPtr(new Experiment(Experiment::CreateUnsafe(filepath)));
}

//' Opens an Experiment
//' @param filepath the path to the '.myrmidon' file
//' @return a \code{\link{fmExperiment}}
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

template <> SEXP wrap(const fort::myrmidon::AntByID & ants) {
	return wrapMap(ants);
}


}


//' Formats an AntID
//' @param antID the AntID to format
//' @return a character with the formatted ID
//[[Rcpp::export]]
std::string fmFormatAntID(fort::myrmidon::AntID antID) {
	return fort::myrmidon::FormatAntID(antID);
}

//' Formats a TagID
//' @param tagID the TagID to format
//' @return a character with the formatted ID
//[[Rcpp::export]]
std::string fmFormatTagID(fort::myrmidon::TagID tagID) {
	return fort::myrmidon::FormatTagID(tagID);
}
