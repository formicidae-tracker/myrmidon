#' @name FortMyrmidon
#' @title FortMyrmidon
#' @docType package
#' @author alexandre.tuleu.2005@polytechnique.org
#' @description R bindings to **fort-myrmidon**. Its is an object
#'     oriented API, please look up the following classes bellow:
#'
#' \itemize{
#' \item API entry point: \code{\link{fmExperiment}}
#' \item Ants and Identification: \itemize{ \item \code{\link{fmAnt}} \item \code{\link{fmIdentification}} }
#' \item Spaces and Zones: \itemize{ \item \code{\link{fmSpace}} \item \code{\link{fmZone}} \item \code{\link{fmZoneDefinition}} }
#' \item Queries and Marchers: \itemize{ \item \code{\link{fmQuery}} \item \code{\link{fmMatcher}} }
#' \item Time representation: \itemize{ \item \code{\link{fmTime}} \item \code{\link{fmDuration}} }
#' \item 2D Shapes : \itemize{ \item \code{\link{fmCircle}} \item \code{\link{fmCapsule}} \item \code{\link{fmPolygon}} }
#' }
#'
#' @import methods
#' @useDynLib FortMyrmidon, .registration = TRUE
#' @exportPattern '^fm[[:alpha:]]+'
#' @import Rcpp
NULL


Rcpp::loadModule("time",TRUE)
Rcpp::loadModule("eigen",TRUE)
Rcpp::loadModule("shapes",TRUE)

Rcpp::loadModule("zone",TRUE)
Rcpp::loadModule("space",TRUE)

Rcpp::loadModule("identification",TRUE)
Rcpp::loadModule("ant",TRUE)

Rcpp::loadModule("experiment",TRUE)

Rcpp::loadModule("matchers",TRUE)

Rcpp::loadModule("utestdata",TRUE)

evalqOnLoad({
    setMethod('+',
              signature(e1 = "Rcpp_fmDuration",
                  e2 = "Rcpp_fmDuration"),
              function(e1,e2) {
                  e1$add(e2)
              })
    setMethod('-',
              signature(e1 = "Rcpp_fmDuration",
                  e2 = "Rcpp_fmDuration"),
              function(e1,e2) {
                  e1$sub(e2)
              })

    setMethod('!=',
              signature(e1 = "Rcpp_fmDuration",
                  e2 = "Rcpp_fmDuration"),
              function(e1,e2) {
                  !e1$equals(e2)
              })
    setMethod('==',
              signature(e1 = "Rcpp_fmDuration",
                  e2 = "Rcpp_fmDuration"),
              function(e1,e2) {
                  e1$equals(e2)
              })
    setMethod('>=',
              signature(e1 = "Rcpp_fmDuration",
                  e2 = "Rcpp_fmDuration"),
              function(e1,e2) {
                  !e1$lower(e2)
              })
    setMethod('>',
              signature(e1 = "Rcpp_fmDuration",
                  e2 = "Rcpp_fmDuration"),
              function(e1,e2) {
                  !e1$lowerEqual(e2)
              })
    setMethod('<',
              signature(e1 = "Rcpp_fmDuration",
                  e2 = "Rcpp_fmDuration"),
              function(e1,e2) {
                  e1$lower(e2)
              })
    setMethod('<=',
              signature(e1 = "Rcpp_fmDuration",
                  e2 = "Rcpp_fmDuration"),
              function(e1,e2) {
                  e1$lowerEqual(e2)
              })
    setMethod('!=',
              signature(e1 = "Rcpp_fmTime",
                  e2 = "Rcpp_fmTime"),
              function(e1,e2) {
                  !e1$equals(e2)
              })
    setMethod('==',
              signature(e1 = "Rcpp_fmTime",
                  e2 = "Rcpp_fmTime"),
              function(e1,e2) {
                  e1$equals(e2)
              })
    setMethod('<',
              signature(e1 = "Rcpp_fmTime",
                  e2 = "Rcpp_fmTime"),
              function(e1,e2) {
                  e1$before(e2)
              })
    setMethod('<=',
              signature(e1 = "Rcpp_fmTime",
                  e2 = "Rcpp_fmTime"),
              function(e1,e2) {
                  !e1$after(e2)
              })
    setMethod('>',
              signature(e1 = "Rcpp_fmTime",
                  e2 = "Rcpp_fmTime"),
              function(e1,e2) {
                  e1$after(e2)
              })
    setMethod('>=',
              signature(e1 = "Rcpp_fmTime",
                  e2 = "Rcpp_fmTime"),
              function(e1,e2) {
                  !e1$before(e2)
              })
    setMethod("length",
              signature='Rcpp_fmVector2dList',
              function(x) {
                  x$size()
              })
    setMethod("as.data.frame",
              signature='Rcpp_fmVector2dList',
              function(x) {
                  x$as.data.frame()
              })
    setMethod("as.list",
              signature='Rcpp_fmVector2dList',
              function(x) {
                  x$as.list()
              })
    setMethod("length",
              signature='Rcpp_fmShapeList',
              function(x) {
                  x$size()
              })
    setMethod("as.list",
              signature='Rcpp_fmShapeList',
              function(x) {
                  x$as.list()
              })
})
