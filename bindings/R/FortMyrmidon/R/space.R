#' @name fmSpace
#' @title An homogenous coordinate system for tracking data
#' @description A fmSpace represent the physical space tracked by one
#'     single camera. Coordinates in one fmSpace cannot be compared
#'     with coordinate from another fmSpace.
#' @description fmSpace are uniquely identified with their ID.
#' @description fmSpace can only be created from an
#'     \code{\link{fmExperiment}} with
#'     \code{\link{fmExperiment$createSpace}()}
#' @description Within a single fmSpace, it could be relevant to
#'     define \code{\link{fmZone}} where interaction between
#'     \code{\link{fmAnt}} could be measured. I.e. Ants in different
#'     fmZones won't report interactions.
#' @description \code{\link{fmZone}} are manipulated with
#'     \code{\link{fmSpace$createZone}()} and
#'     \code{\link{fmSpace$deleteZone}()}.
#' @field name the fmSpace name as a character
#' @field ID the fmSpace unique identifier in the fmExperiment.
#' @family fmSpace methods
NULL

#' @name fmSpace$createZone
#' @title Creates a fmZone for this fmSpace
#' @description Creates a new \code{\link{fmZone}} for this fmSpace.
#' @param name the wanted name for the \code{\link{fmZone}}
#' @return the new \code{\link{fmZone}}
#' @family fmSpace methods
NULL

#' @name fmSpace$deleteZone
#' @title Removes a fmZone for this fmSpace
#' @description Removes a \code{\link{fmZone}} from this fmSpace.
#' @param zoneID the ID of the \code{\link{fmZone}} to remove
#' @family fmSpace methods
NULL

#' @name fmSpace$locateMovieFrame
#' @title Locates a movie file and frame number from a fmTime
#' @description Locates a movie file and frame number from a
#'     \code{\link{fmTime}}.
#' @param time the time we want to query
#' @return a named list of the absolute file path to the file, and a
#'     frame number of the frame that was acquired at or just after
#'     time.
#' @family fmSpace methods
NULL
