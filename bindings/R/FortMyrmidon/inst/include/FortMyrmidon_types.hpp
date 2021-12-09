#pragma once

#include <fort/time/Time.hpp>
#include <fort/myrmidon/Shapes.hpp>
#include <fort/myrmidon/Zone.hpp>
#include <fort/myrmidon/Space.hpp>
#include <fort/myrmidon/Experiment.hpp>
#include <fort/myrmidon/Matchers.hpp>
#include <fort/myrmidon/types/TagStatistics.hpp>
#include <fort/myrmidon/types/ExperimentDataInfo.hpp>

#include <RcppCommon.h>

RCPP_EXPOSED_CLASS_NODECL(fort::Duration);
RCPP_EXPOSED_CLASS_NODECL(fort::Time);


namespace Rcpp {
template <> Eigen::Vector2d as(SEXP);
template <> SEXP wrap(const Eigen::Vector2d & );

template <> fort::myrmidon::Color as(SEXP);
template <> SEXP wrap(const fort::myrmidon::Color & );

template <> fort::myrmidon::Vector2dList as(SEXP);

template <> fort::myrmidon::Shape::List as(SEXP);
template <> SEXP wrap(const fort::myrmidon::ZoneDefinitionList &);
template <> SEXP wrap(const fort::myrmidon::ZoneByID &);
template <> SEXP wrap(const fort::myrmidon::SpaceByID & );
template <> SEXP wrap(const fort::myrmidon::AntByID & );
template <> SEXP wrap(const std::map<fort::Time,fort::myrmidon::Value> & );

template <> SEXP wrap(const fort::myrmidon::IdentificationList &);

template <> fort::myrmidon::Value as(SEXP);
template <> SEXP wrap(const fort::myrmidon::Value & );


template <> SEXP wrap(const fort::myrmidon::TypedCapsuleList & );

template <> std::vector<fort::myrmidon::Matcher::Ptr> as(SEXP);

template <> SEXP wrap(const fort::myrmidon::TagStatistics::ByTagID &);

template <> SEXP wrap(const fort::myrmidon::ExperimentDataInfo &);
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

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Identification::Ptr);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Ant::Ptr);

typedef std::shared_ptr<fort::myrmidon::Experiment> ExperimentPtr;
RCPP_EXPOSED_CLASS_NODECL(ExperimentPtr);

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Matcher::Ptr);
