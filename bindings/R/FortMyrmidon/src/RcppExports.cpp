// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include "../inst/include/FortMyrmidon_types.hpp"
#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// pfmValueType
int pfmValueType(const fort::myrmidon::Value& v);
RcppExport SEXP _FortMyrmidon_pfmValueType(SEXP vSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const fort::myrmidon::Value& >::type v(vSEXP);
    rcpp_result_gen = Rcpp::wrap(pfmValueType(v));
    return rcpp_result_gen;
END_RCPP
}
// pfmBool
fort::myrmidon::Value pfmBool();
RcppExport SEXP _FortMyrmidon_pfmBool() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(pfmBool());
    return rcpp_result_gen;
END_RCPP
}
// pfmInt
fort::myrmidon::Value pfmInt();
RcppExport SEXP _FortMyrmidon_pfmInt() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(pfmInt());
    return rcpp_result_gen;
END_RCPP
}
// pfmDouble
fort::myrmidon::Value pfmDouble();
RcppExport SEXP _FortMyrmidon_pfmDouble() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(pfmDouble());
    return rcpp_result_gen;
END_RCPP
}
// pfmString
fort::myrmidon::Value pfmString();
RcppExport SEXP _FortMyrmidon_pfmString() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(pfmString());
    return rcpp_result_gen;
END_RCPP
}
// pfmTime
fort::myrmidon::Value pfmTime();
RcppExport SEXP _FortMyrmidon_pfmTime() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(pfmTime());
    return rcpp_result_gen;
END_RCPP
}
// fmDefaultPalette
SEXP fmDefaultPalette();
RcppExport SEXP _FortMyrmidon_fmDefaultPalette() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(fmDefaultPalette());
    return rcpp_result_gen;
END_RCPP
}
// fmDefaultPaletteColor
fort::myrmidon::Color fmDefaultPaletteColor(int index);
RcppExport SEXP _FortMyrmidon_fmDefaultPaletteColor(SEXP indexSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type index(indexSEXP);
    rcpp_result_gen = Rcpp::wrap(fmDefaultPaletteColor(index));
    return rcpp_result_gen;
END_RCPP
}
// fmVector2dListCreate
fort::myrmidon::Vector2dList fmVector2dListCreate(SEXP points);
RcppExport SEXP _FortMyrmidon_fmVector2dListCreate(SEXP pointsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type points(pointsSEXP);
    rcpp_result_gen = Rcpp::wrap(fmVector2dListCreate(points));
    return rcpp_result_gen;
END_RCPP
}
// pfmIWantAVector2dList
void pfmIWantAVector2dList(const fort::myrmidon::Vector2dList& l);
RcppExport SEXP _FortMyrmidon_pfmIWantAVector2dList(SEXP lSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const fort::myrmidon::Vector2dList& >::type l(lSEXP);
    pfmIWantAVector2dList(l);
    return R_NilValue;
END_RCPP
}
// fmExperimentCreate
ExperimentPtr fmExperimentCreate(const std::string& filepath);
RcppExport SEXP _FortMyrmidon_fmExperimentCreate(SEXP filepathSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type filepath(filepathSEXP);
    rcpp_result_gen = Rcpp::wrap(fmExperimentCreate(filepath));
    return rcpp_result_gen;
END_RCPP
}
// fmExperimentOpen
ExperimentPtr fmExperimentOpen(const std::string& filepath);
RcppExport SEXP _FortMyrmidon_fmExperimentOpen(SEXP filepathSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type filepath(filepathSEXP);
    rcpp_result_gen = Rcpp::wrap(fmExperimentOpen(filepath));
    return rcpp_result_gen;
END_RCPP
}
// fmFormatAntID
std::string fmFormatAntID(fort::myrmidon::AntID antID);
RcppExport SEXP _FortMyrmidon_fmFormatAntID(SEXP antIDSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< fort::myrmidon::AntID >::type antID(antIDSEXP);
    rcpp_result_gen = Rcpp::wrap(fmFormatAntID(antID));
    return rcpp_result_gen;
END_RCPP
}
// fmFormatTagID
std::string fmFormatTagID(fort::myrmidon::TagID tagID);
RcppExport SEXP _FortMyrmidon_fmFormatTagID(SEXP tagIDSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< fort::myrmidon::TagID >::type tagID(tagIDSEXP);
    rcpp_result_gen = Rcpp::wrap(fmFormatTagID(tagID));
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherAny
fort::myrmidon::Matcher::Ptr fmMatcherAny();
RcppExport SEXP _FortMyrmidon_fmMatcherAny() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(fmMatcherAny());
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherAnd
fort::myrmidon::Matcher::Ptr fmMatcherAnd(const std::vector<fort::myrmidon::Matcher::Ptr>& matchers);
RcppExport SEXP _FortMyrmidon_fmMatcherAnd(SEXP matchersSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::vector<fort::myrmidon::Matcher::Ptr>& >::type matchers(matchersSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMatcherAnd(matchers));
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherOr
fort::myrmidon::Matcher::Ptr fmMatcherOr(const std::vector<fort::myrmidon::Matcher::Ptr>& matchers);
RcppExport SEXP _FortMyrmidon_fmMatcherOr(SEXP matchersSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::vector<fort::myrmidon::Matcher::Ptr>& >::type matchers(matchersSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMatcherOr(matchers));
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherAntID
fort::myrmidon::Matcher::Ptr fmMatcherAntID(fort::myrmidon::AntID antID);
RcppExport SEXP _FortMyrmidon_fmMatcherAntID(SEXP antIDSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< fort::myrmidon::AntID >::type antID(antIDSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMatcherAntID(antID));
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherAntMetaData
fort::myrmidon::Matcher::Ptr fmMatcherAntMetaData(const std::string& key, const fort::myrmidon::Value& value);
RcppExport SEXP _FortMyrmidon_fmMatcherAntMetaData(SEXP keySEXP, SEXP valueSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type key(keySEXP);
    Rcpp::traits::input_parameter< const fort::myrmidon::Value& >::type value(valueSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMatcherAntMetaData(key, value));
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherAntDistanceSmallerThan
fort::myrmidon::Matcher::Ptr fmMatcherAntDistanceSmallerThan(double distance);
RcppExport SEXP _FortMyrmidon_fmMatcherAntDistanceSmallerThan(SEXP distanceSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type distance(distanceSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMatcherAntDistanceSmallerThan(distance));
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherAntDistanceGreaterThan
fort::myrmidon::Matcher::Ptr fmMatcherAntDistanceGreaterThan(double distance);
RcppExport SEXP _FortMyrmidon_fmMatcherAntDistanceGreaterThan(SEXP distanceSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type distance(distanceSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMatcherAntDistanceGreaterThan(distance));
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherAntAngleSmallerThan
fort::myrmidon::Matcher::Ptr fmMatcherAntAngleSmallerThan(double angle);
RcppExport SEXP _FortMyrmidon_fmMatcherAntAngleSmallerThan(SEXP angleSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type angle(angleSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMatcherAntAngleSmallerThan(angle));
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherAntAngleGreaterThan
fort::myrmidon::Matcher::Ptr fmMatcherAntAngleGreaterThan(double angle);
RcppExport SEXP _FortMyrmidon_fmMatcherAntAngleGreaterThan(SEXP angleSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type angle(angleSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMatcherAntAngleGreaterThan(angle));
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherInteractionType
fort::myrmidon::Matcher::Ptr fmMatcherInteractionType(fort::myrmidon::AntShapeTypeID type1, fort::myrmidon::AntShapeTypeID type2);
RcppExport SEXP _FortMyrmidon_fmMatcherInteractionType(SEXP type1SEXP, SEXP type2SEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< fort::myrmidon::AntShapeTypeID >::type type1(type1SEXP);
    Rcpp::traits::input_parameter< fort::myrmidon::AntShapeTypeID >::type type2(type2SEXP);
    rcpp_result_gen = Rcpp::wrap(fmMatcherInteractionType(type1, type2));
    return rcpp_result_gen;
END_RCPP
}
// fmMatcherAntDisplacement
fort::myrmidon::Matcher::Ptr fmMatcherAntDisplacement(double under, fort::Duration minimumGap);
RcppExport SEXP _FortMyrmidon_fmMatcherAntDisplacement(SEXP underSEXP, SEXP minimumGapSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type under(underSEXP);
    Rcpp::traits::input_parameter< fort::Duration >::type minimumGap(minimumGapSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMatcherAntDisplacement(under, minimumGap));
    return rcpp_result_gen;
END_RCPP
}
// fmQueryComputeMeasurementFor
DataFrame fmQueryComputeMeasurementFor(const ExperimentPtr& experiment, fort::myrmidon::AntID antID, fort::myrmidon::MeasurementTypeID typeID);
RcppExport SEXP _FortMyrmidon_fmQueryComputeMeasurementFor(SEXP experimentSEXP, SEXP antIDSEXP, SEXP typeIDSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const ExperimentPtr& >::type experiment(experimentSEXP);
    Rcpp::traits::input_parameter< fort::myrmidon::AntID >::type antID(antIDSEXP);
    Rcpp::traits::input_parameter< fort::myrmidon::MeasurementTypeID >::type typeID(typeIDSEXP);
    rcpp_result_gen = Rcpp::wrap(fmQueryComputeMeasurementFor(experiment, antID, typeID));
    return rcpp_result_gen;
END_RCPP
}
// fmQueryComputeTagStatistics
fort::myrmidon::TagStatistics::ByTagID fmQueryComputeTagStatistics(const ExperimentPtr& experiment);
RcppExport SEXP _FortMyrmidon_fmQueryComputeTagStatistics(SEXP experimentSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const ExperimentPtr& >::type experiment(experimentSEXP);
    rcpp_result_gen = Rcpp::wrap(fmQueryComputeTagStatistics(experiment));
    return rcpp_result_gen;
END_RCPP
}
// pfmQueryIdentifyFrames
SEXP pfmQueryIdentifyFrames(const ExperimentPtr& experiment, const fort::Time& start, const fort::Time& end, bool computeZones, bool showProgress, bool singleThreaded);
RcppExport SEXP _FortMyrmidon_pfmQueryIdentifyFrames(SEXP experimentSEXP, SEXP startSEXP, SEXP endSEXP, SEXP computeZonesSEXP, SEXP showProgressSEXP, SEXP singleThreadedSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const ExperimentPtr& >::type experiment(experimentSEXP);
    Rcpp::traits::input_parameter< const fort::Time& >::type start(startSEXP);
    Rcpp::traits::input_parameter< const fort::Time& >::type end(endSEXP);
    Rcpp::traits::input_parameter< bool >::type computeZones(computeZonesSEXP);
    Rcpp::traits::input_parameter< bool >::type showProgress(showProgressSEXP);
    Rcpp::traits::input_parameter< bool >::type singleThreaded(singleThreadedSEXP);
    rcpp_result_gen = Rcpp::wrap(pfmQueryIdentifyFrames(experiment, start, end, computeZones, showProgress, singleThreaded));
    return rcpp_result_gen;
END_RCPP
}
// pfmQueryCollideFrames
SEXP pfmQueryCollideFrames(const ExperimentPtr& experiment, const fort::Time& start, const fort::Time& end, bool collisionsIgnoreZones, bool showProgress, bool singleThreaded);
RcppExport SEXP _FortMyrmidon_pfmQueryCollideFrames(SEXP experimentSEXP, SEXP startSEXP, SEXP endSEXP, SEXP collisionsIgnoreZonesSEXP, SEXP showProgressSEXP, SEXP singleThreadedSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const ExperimentPtr& >::type experiment(experimentSEXP);
    Rcpp::traits::input_parameter< const fort::Time& >::type start(startSEXP);
    Rcpp::traits::input_parameter< const fort::Time& >::type end(endSEXP);
    Rcpp::traits::input_parameter< bool >::type collisionsIgnoreZones(collisionsIgnoreZonesSEXP);
    Rcpp::traits::input_parameter< bool >::type showProgress(showProgressSEXP);
    Rcpp::traits::input_parameter< bool >::type singleThreaded(singleThreadedSEXP);
    rcpp_result_gen = Rcpp::wrap(pfmQueryCollideFrames(experiment, start, end, collisionsIgnoreZones, showProgress, singleThreaded));
    return rcpp_result_gen;
END_RCPP
}
// pfmQueryComputeAntTrajectories
SEXP pfmQueryComputeAntTrajectories(const ExperimentPtr& experiment, const fort::Time& start, const fort::Time& end, const fort::Duration& maximumGap, const fort::myrmidon::Matcher::Ptr& matcher, bool computeZones, bool showProgress, bool singleThreaded);
RcppExport SEXP _FortMyrmidon_pfmQueryComputeAntTrajectories(SEXP experimentSEXP, SEXP startSEXP, SEXP endSEXP, SEXP maximumGapSEXP, SEXP matcherSEXP, SEXP computeZonesSEXP, SEXP showProgressSEXP, SEXP singleThreadedSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const ExperimentPtr& >::type experiment(experimentSEXP);
    Rcpp::traits::input_parameter< const fort::Time& >::type start(startSEXP);
    Rcpp::traits::input_parameter< const fort::Time& >::type end(endSEXP);
    Rcpp::traits::input_parameter< const fort::Duration& >::type maximumGap(maximumGapSEXP);
    Rcpp::traits::input_parameter< const fort::myrmidon::Matcher::Ptr& >::type matcher(matcherSEXP);
    Rcpp::traits::input_parameter< bool >::type computeZones(computeZonesSEXP);
    Rcpp::traits::input_parameter< bool >::type showProgress(showProgressSEXP);
    Rcpp::traits::input_parameter< bool >::type singleThreaded(singleThreadedSEXP);
    rcpp_result_gen = Rcpp::wrap(pfmQueryComputeAntTrajectories(experiment, start, end, maximumGap, matcher, computeZones, showProgress, singleThreaded));
    return rcpp_result_gen;
END_RCPP
}
// pfmQueryComputeAntInteractions
SEXP pfmQueryComputeAntInteractions(const ExperimentPtr& experiment, const fort::Time& start, const fort::Time& end, const fort::Duration& maximumGap, const fort::myrmidon::Matcher::Ptr& matcher, bool collisionsIgnoreZones, bool reportFullTrajectories, bool showProgress, bool singleThreaded);
RcppExport SEXP _FortMyrmidon_pfmQueryComputeAntInteractions(SEXP experimentSEXP, SEXP startSEXP, SEXP endSEXP, SEXP maximumGapSEXP, SEXP matcherSEXP, SEXP collisionsIgnoreZonesSEXP, SEXP reportFullTrajectoriesSEXP, SEXP showProgressSEXP, SEXP singleThreadedSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const ExperimentPtr& >::type experiment(experimentSEXP);
    Rcpp::traits::input_parameter< const fort::Time& >::type start(startSEXP);
    Rcpp::traits::input_parameter< const fort::Time& >::type end(endSEXP);
    Rcpp::traits::input_parameter< const fort::Duration& >::type maximumGap(maximumGapSEXP);
    Rcpp::traits::input_parameter< const fort::myrmidon::Matcher::Ptr& >::type matcher(matcherSEXP);
    Rcpp::traits::input_parameter< bool >::type collisionsIgnoreZones(collisionsIgnoreZonesSEXP);
    Rcpp::traits::input_parameter< bool >::type reportFullTrajectories(reportFullTrajectoriesSEXP);
    Rcpp::traits::input_parameter< bool >::type showProgress(showProgressSEXP);
    Rcpp::traits::input_parameter< bool >::type singleThreaded(singleThreadedSEXP);
    rcpp_result_gen = Rcpp::wrap(pfmQueryComputeAntInteractions(experiment, start, end, maximumGap, matcher, collisionsIgnoreZones, reportFullTrajectories, showProgress, singleThreaded));
    return rcpp_result_gen;
END_RCPP
}
// fmQueryGetDataInformations
fort::myrmidon::ExperimentDataInfo fmQueryGetDataInformations(const ExperimentPtr& experiment);
RcppExport SEXP _FortMyrmidon_fmQueryGetDataInformations(SEXP experimentSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const ExperimentPtr& >::type experiment(experimentSEXP);
    rcpp_result_gen = Rcpp::wrap(fmQueryGetDataInformations(experiment));
    return rcpp_result_gen;
END_RCPP
}
// fmQueryGetMetaDataKeyRanges
Rcpp::DataFrame fmQueryGetMetaDataKeyRanges(const ExperimentPtr& experiment, const std::string& key, const fort::myrmidon::Value& value);
RcppExport SEXP _FortMyrmidon_fmQueryGetMetaDataKeyRanges(SEXP experimentSEXP, SEXP keySEXP, SEXP valueSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const ExperimentPtr& >::type experiment(experimentSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type key(keySEXP);
    Rcpp::traits::input_parameter< const fort::myrmidon::Value& >::type value(valueSEXP);
    rcpp_result_gen = Rcpp::wrap(fmQueryGetMetaDataKeyRanges(experiment, key, value));
    return rcpp_result_gen;
END_RCPP
}
// fmShapeListCreate
fort::myrmidon::Shape::List fmShapeListCreate(SEXP shapes);
RcppExport SEXP _FortMyrmidon_fmShapeListCreate(SEXP shapesSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type shapes(shapesSEXP);
    rcpp_result_gen = Rcpp::wrap(fmShapeListCreate(shapes));
    return rcpp_result_gen;
END_RCPP
}
// pfmIWantAShapeList
void pfmIWantAShapeList(const fort::myrmidon::Shape::List& l);
RcppExport SEXP _FortMyrmidon_pfmIWantAShapeList(SEXP lSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const fort::myrmidon::Shape::List& >::type l(lSEXP);
    pfmIWantAShapeList(l);
    return R_NilValue;
END_RCPP
}
// fmTimeCreate
fort::Time fmTimeCreate(double offset);
RcppExport SEXP _FortMyrmidon_fmTimeCreate(SEXP offsetSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type offset(offsetSEXP);
    rcpp_result_gen = Rcpp::wrap(fmTimeCreate(offset));
    return rcpp_result_gen;
END_RCPP
}
// fmTimeNow
fort::Time fmTimeNow();
RcppExport SEXP _FortMyrmidon_fmTimeNow() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(fmTimeNow());
    return rcpp_result_gen;
END_RCPP
}
// fmTimeForever
fort::Time fmTimeForever();
RcppExport SEXP _FortMyrmidon_fmTimeForever() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(fmTimeForever());
    return rcpp_result_gen;
END_RCPP
}
// fmTimeSinceEver
fort::Time fmTimeSinceEver();
RcppExport SEXP _FortMyrmidon_fmTimeSinceEver() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(fmTimeSinceEver());
    return rcpp_result_gen;
END_RCPP
}
// fmTimeParse
fort::Time fmTimeParse(const std::string& input);
RcppExport SEXP _FortMyrmidon_fmTimeParse(SEXP inputSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type input(inputSEXP);
    rcpp_result_gen = Rcpp::wrap(fmTimeParse(input));
    return rcpp_result_gen;
END_RCPP
}
// fmDurationParse
fort::Duration fmDurationParse(const std::string& input);
RcppExport SEXP _FortMyrmidon_fmDurationParse(SEXP inputSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type input(inputSEXP);
    rcpp_result_gen = Rcpp::wrap(fmDurationParse(input));
    return rcpp_result_gen;
END_RCPP
}
// fmHour
fort::Duration fmHour(double h);
RcppExport SEXP _FortMyrmidon_fmHour(SEXP hSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type h(hSEXP);
    rcpp_result_gen = Rcpp::wrap(fmHour(h));
    return rcpp_result_gen;
END_RCPP
}
// fmMinute
fort::Duration fmMinute(double m);
RcppExport SEXP _FortMyrmidon_fmMinute(SEXP mSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type m(mSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMinute(m));
    return rcpp_result_gen;
END_RCPP
}
// fmSecond
fort::Duration fmSecond(double s);
RcppExport SEXP _FortMyrmidon_fmSecond(SEXP sSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type s(sSEXP);
    rcpp_result_gen = Rcpp::wrap(fmSecond(s));
    return rcpp_result_gen;
END_RCPP
}
// fmMillisecond
fort::Duration fmMillisecond(double ms);
RcppExport SEXP _FortMyrmidon_fmMillisecond(SEXP msSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type ms(msSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMillisecond(ms));
    return rcpp_result_gen;
END_RCPP
}
// fmMicrosecond
fort::Duration fmMicrosecond(double us);
RcppExport SEXP _FortMyrmidon_fmMicrosecond(SEXP usSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< double >::type us(usSEXP);
    rcpp_result_gen = Rcpp::wrap(fmMicrosecond(us));
    return rcpp_result_gen;
END_RCPP
}
// fmNanosecond
fort::Duration fmNanosecond(int64_t ns);
RcppExport SEXP _FortMyrmidon_fmNanosecond(SEXP nsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int64_t >::type ns(nsSEXP);
    rcpp_result_gen = Rcpp::wrap(fmNanosecond(ns));
    return rcpp_result_gen;
END_RCPP
}

RcppExport SEXP _rcpp_module_boot_ant();
RcppExport SEXP _rcpp_module_boot_eigen();
RcppExport SEXP _rcpp_module_boot_experiment();
RcppExport SEXP _rcpp_module_boot_identification();
RcppExport SEXP _rcpp_module_boot_matchers();
RcppExport SEXP _rcpp_module_boot_shapes();
RcppExport SEXP _rcpp_module_boot_space();
RcppExport SEXP _rcpp_module_boot_time();
RcppExport SEXP _rcpp_module_boot_utestdata();
RcppExport SEXP _rcpp_module_boot_zone();

static const R_CallMethodDef CallEntries[] = {
    {"_FortMyrmidon_pfmValueType", (DL_FUNC) &_FortMyrmidon_pfmValueType, 1},
    {"_FortMyrmidon_pfmBool", (DL_FUNC) &_FortMyrmidon_pfmBool, 0},
    {"_FortMyrmidon_pfmInt", (DL_FUNC) &_FortMyrmidon_pfmInt, 0},
    {"_FortMyrmidon_pfmDouble", (DL_FUNC) &_FortMyrmidon_pfmDouble, 0},
    {"_FortMyrmidon_pfmString", (DL_FUNC) &_FortMyrmidon_pfmString, 0},
    {"_FortMyrmidon_pfmTime", (DL_FUNC) &_FortMyrmidon_pfmTime, 0},
    {"_FortMyrmidon_fmDefaultPalette", (DL_FUNC) &_FortMyrmidon_fmDefaultPalette, 0},
    {"_FortMyrmidon_fmDefaultPaletteColor", (DL_FUNC) &_FortMyrmidon_fmDefaultPaletteColor, 1},
    {"_FortMyrmidon_fmVector2dListCreate", (DL_FUNC) &_FortMyrmidon_fmVector2dListCreate, 1},
    {"_FortMyrmidon_pfmIWantAVector2dList", (DL_FUNC) &_FortMyrmidon_pfmIWantAVector2dList, 1},
    {"_FortMyrmidon_fmExperimentCreate", (DL_FUNC) &_FortMyrmidon_fmExperimentCreate, 1},
    {"_FortMyrmidon_fmExperimentOpen", (DL_FUNC) &_FortMyrmidon_fmExperimentOpen, 1},
    {"_FortMyrmidon_fmFormatAntID", (DL_FUNC) &_FortMyrmidon_fmFormatAntID, 1},
    {"_FortMyrmidon_fmFormatTagID", (DL_FUNC) &_FortMyrmidon_fmFormatTagID, 1},
    {"_FortMyrmidon_fmMatcherAny", (DL_FUNC) &_FortMyrmidon_fmMatcherAny, 0},
    {"_FortMyrmidon_fmMatcherAnd", (DL_FUNC) &_FortMyrmidon_fmMatcherAnd, 1},
    {"_FortMyrmidon_fmMatcherOr", (DL_FUNC) &_FortMyrmidon_fmMatcherOr, 1},
    {"_FortMyrmidon_fmMatcherAntID", (DL_FUNC) &_FortMyrmidon_fmMatcherAntID, 1},
    {"_FortMyrmidon_fmMatcherAntMetaData", (DL_FUNC) &_FortMyrmidon_fmMatcherAntMetaData, 2},
    {"_FortMyrmidon_fmMatcherAntDistanceSmallerThan", (DL_FUNC) &_FortMyrmidon_fmMatcherAntDistanceSmallerThan, 1},
    {"_FortMyrmidon_fmMatcherAntDistanceGreaterThan", (DL_FUNC) &_FortMyrmidon_fmMatcherAntDistanceGreaterThan, 1},
    {"_FortMyrmidon_fmMatcherAntAngleSmallerThan", (DL_FUNC) &_FortMyrmidon_fmMatcherAntAngleSmallerThan, 1},
    {"_FortMyrmidon_fmMatcherAntAngleGreaterThan", (DL_FUNC) &_FortMyrmidon_fmMatcherAntAngleGreaterThan, 1},
    {"_FortMyrmidon_fmMatcherInteractionType", (DL_FUNC) &_FortMyrmidon_fmMatcherInteractionType, 2},
    {"_FortMyrmidon_fmMatcherAntDisplacement", (DL_FUNC) &_FortMyrmidon_fmMatcherAntDisplacement, 2},
    {"_FortMyrmidon_fmQueryComputeMeasurementFor", (DL_FUNC) &_FortMyrmidon_fmQueryComputeMeasurementFor, 3},
    {"_FortMyrmidon_fmQueryComputeTagStatistics", (DL_FUNC) &_FortMyrmidon_fmQueryComputeTagStatistics, 1},
    {"_FortMyrmidon_pfmQueryIdentifyFrames", (DL_FUNC) &_FortMyrmidon_pfmQueryIdentifyFrames, 6},
    {"_FortMyrmidon_pfmQueryCollideFrames", (DL_FUNC) &_FortMyrmidon_pfmQueryCollideFrames, 6},
    {"_FortMyrmidon_pfmQueryComputeAntTrajectories", (DL_FUNC) &_FortMyrmidon_pfmQueryComputeAntTrajectories, 8},
    {"_FortMyrmidon_pfmQueryComputeAntInteractions", (DL_FUNC) &_FortMyrmidon_pfmQueryComputeAntInteractions, 9},
    {"_FortMyrmidon_fmQueryGetDataInformations", (DL_FUNC) &_FortMyrmidon_fmQueryGetDataInformations, 1},
    {"_FortMyrmidon_fmQueryGetMetaDataKeyRanges", (DL_FUNC) &_FortMyrmidon_fmQueryGetMetaDataKeyRanges, 3},
    {"_FortMyrmidon_fmShapeListCreate", (DL_FUNC) &_FortMyrmidon_fmShapeListCreate, 1},
    {"_FortMyrmidon_pfmIWantAShapeList", (DL_FUNC) &_FortMyrmidon_pfmIWantAShapeList, 1},
    {"_FortMyrmidon_fmTimeCreate", (DL_FUNC) &_FortMyrmidon_fmTimeCreate, 1},
    {"_FortMyrmidon_fmTimeNow", (DL_FUNC) &_FortMyrmidon_fmTimeNow, 0},
    {"_FortMyrmidon_fmTimeForever", (DL_FUNC) &_FortMyrmidon_fmTimeForever, 0},
    {"_FortMyrmidon_fmTimeSinceEver", (DL_FUNC) &_FortMyrmidon_fmTimeSinceEver, 0},
    {"_FortMyrmidon_fmTimeParse", (DL_FUNC) &_FortMyrmidon_fmTimeParse, 1},
    {"_FortMyrmidon_fmDurationParse", (DL_FUNC) &_FortMyrmidon_fmDurationParse, 1},
    {"_FortMyrmidon_fmHour", (DL_FUNC) &_FortMyrmidon_fmHour, 1},
    {"_FortMyrmidon_fmMinute", (DL_FUNC) &_FortMyrmidon_fmMinute, 1},
    {"_FortMyrmidon_fmSecond", (DL_FUNC) &_FortMyrmidon_fmSecond, 1},
    {"_FortMyrmidon_fmMillisecond", (DL_FUNC) &_FortMyrmidon_fmMillisecond, 1},
    {"_FortMyrmidon_fmMicrosecond", (DL_FUNC) &_FortMyrmidon_fmMicrosecond, 1},
    {"_FortMyrmidon_fmNanosecond", (DL_FUNC) &_FortMyrmidon_fmNanosecond, 1},
    {"_rcpp_module_boot_ant", (DL_FUNC) &_rcpp_module_boot_ant, 0},
    {"_rcpp_module_boot_eigen", (DL_FUNC) &_rcpp_module_boot_eigen, 0},
    {"_rcpp_module_boot_experiment", (DL_FUNC) &_rcpp_module_boot_experiment, 0},
    {"_rcpp_module_boot_identification", (DL_FUNC) &_rcpp_module_boot_identification, 0},
    {"_rcpp_module_boot_matchers", (DL_FUNC) &_rcpp_module_boot_matchers, 0},
    {"_rcpp_module_boot_shapes", (DL_FUNC) &_rcpp_module_boot_shapes, 0},
    {"_rcpp_module_boot_space", (DL_FUNC) &_rcpp_module_boot_space, 0},
    {"_rcpp_module_boot_time", (DL_FUNC) &_rcpp_module_boot_time, 0},
    {"_rcpp_module_boot_utestdata", (DL_FUNC) &_rcpp_module_boot_utestdata, 0},
    {"_rcpp_module_boot_zone", (DL_FUNC) &_rcpp_module_boot_zone, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_FortMyrmidon(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
