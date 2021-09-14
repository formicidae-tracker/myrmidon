#' @name FortMyrmidon
#' @title FortMyrmidon
#' @docType package
#' @author alexandre.tuleu.2005@polytechnique.org
#' @description foo
#' @import methods
#' @useDynLib FortMyrmidon, .registration = TRUE
#' @exportPattern '^[[:alpha:]]+'
#' @import Rcpp
NULL


Rcpp::loadModule("time",TRUE)

evalqOnLoad({
    assign('+.Rcpp_fmDuration', function(e1, e2) {
        e1$add(e2)
    }, .GlobalEnv)
    assign('-.Rcpp_fmDuration', function(e1,e2) {
        e1$sub(e2)
    }, .GlobalEnv)
    assign('!=.Rcpp_fmDuration', function(e1, e2) {
        !e1$equals(e2)
    }, .GlobalEnv)
    assign('==.Rcpp_fmDuration', function(e1, e2) {
        e1$equals(e2)
    }, .GlobalEnv)
    assign('>.Rcpp_fmDuration', function(e1, e2) {
        !e1$lowerEqual(e2)
    }, .GlobalEnv)
    assign('>=.Rcpp_fmDuration', function(e1, e2) {
        !e1$lower(e2)
    }, .GlobalEnv)
    assign('<=.Rcpp_fmDuration', function(e1, e2) {
        e1$lowerEqual(e2)
    }, .GlobalEnv)
    assign('<.Rcpp_fmDuration', function(e1, e2) {
        e1$lower(e2)
    }, .GlobalEnv)
    assign('!=.Rcpp_fmTime', function(e1, e2) {
        !e1$equals(e2)
    }, .GlobalEnv)
    assign('==.Rcpp_fmTime', function(e1, e2) {
        e1$equals(e2)
    }, .GlobalEnv)
    assign('>=.Rcpp_fmTime', function(e1, e2) {
        !e1$before(e2)
    }, .GlobalEnv)
    assign('>.Rcpp_fmTime', function(e1, e2) {
        e1$after(e2)
    }, .GlobalEnv)
    assign('<=.Rcpp_fmTime', function(e1, e2) {
        !e1$after(e2)
    }, .GlobalEnv)
    assign('<.Rcpp_fmTime', function(e1, e2) {
        e1$before(e2)
    }, .GlobalEnv)
})
