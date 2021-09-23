#include <fort/time/Time.hpp>
#include <fort/myrmidon/Shapes.hpp>
#include <fort/myrmidon/Zone.hpp>
#include <fort/myrmidon/Space.hpp>
#include <fort/myrmidon/Experiment.hpp>
#include <RcppCommon.h>

RCPP_EXPOSED_CLASS_NODECL(fort::Duration);
RCPP_EXPOSED_CLASS_NODECL(fort::Time);


namespace Rcpp {
template <> Eigen::Vector2d as(SEXP);
template <> SEXP wrap(const Eigen::Vector2d & );
template <> fort::myrmidon::Vector2dList as(SEXP);
template <> fort::myrmidon::Shape::List as(SEXP);
template <> SEXP wrap(const fort::myrmidon::ZoneDefinitionList &);
template <> SEXP wrap(const fort::myrmidon::ZoneByID &);
}

RCPP_EXPOSED_ENUM_NODECL(fort::myrmidon::Shape::Type)
RCPP_EXPOSED_WRAP(fort::myrmidon::Vector2dList);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Shape);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Circle);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Capsule);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Polygon);
RCPP_EXPOSED_WRAP(fort::myrmidon::Shape::List);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::ZoneDefinition::Ptr);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Zone::Ptr);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Space::Ptr);


typedef std::shared_ptr<fort::myrmidon::Experiment> ExperimentPtr;
RCPP_EXPOSED_CLASS_NODECL(ExperimentPtr);
