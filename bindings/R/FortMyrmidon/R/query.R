#' @name fmQuery
#' @title Computations on tracking data
#' @description fmQuery are computation on an fmExperiment tracking data.
#'
#' @note For very small Experiment in number of Ant, most of the query
#'     operation are IO bounded, and the multithreading overhead will
#'     impact performance by 40-50% in computation time, as threads
#'     are waiting for data inpout to come. This is the case
#'     for\code{\link{fmQueryIdentifyFrames}()}, and
#'     \code{\link{fmQueryComputeAntTrajectories}()}. When collision
#'     detection is required, multi-threading can largely help,
#'     especially with a large number of individuals. Threading is
#'     controlled the singleThreaded args of multi-threaded queries.
#' @family fmQuery methods
NULL


#' Identifies fmAnt in tracking frame
#' @description Identifies fmAnt in tracking data frame. I.e. finds
#'     fmAnt based on fmIdentification and apply the tag <-> ant
#'     position corrections.
#' @param experiment the \code{\link{fmExperiment}} to query on
#' @param start the first tracking time to consider,
#' @param end the last tracking time to consider
#' @param computeZones computes the zone of the Ant, but do not
#'     perform collision detection. If FALSE, \code{0} will be
#'     reported as a zone
#' @param showProgress display query progress in the console
#' @param singleThreaded enforce the use of a single thread.
#' @return a list of two element \itemize{\item\code{$frames}: a
#'     \code{data.frame} summarizing the time space and dimension of
#'     each frame.\item\code{$positions}: a list of \code{data.frame}
#'     with the position of each ant for that frame. Each element of
#'     this list correspond to the same index in \code{frames}}
fmQueryIdentifyFrames <- function(experiment,
                                  start = fmTimeSinceEver(),
                                  end = fmTimeForever(),
                                  computeZones = FALSE,
                                  showProgress = TRUE,
                                  singleThreaded = FALSE) {
    pfmQueryIdentifyFrames(experiment,
                           start,
                           end,
                           computeZones,
                           showProgress,
                           singleThreaded)
}

#' Finds Ant collisions in tracking frames.
#' @description Finds collision, (punctual interactions) between fmAnt
#'     in tracking frame based on their virtual shape. Current zone
#'     for each ants will also be computed.
#' @param experiment the \code{\link{fmExperiment}} to query on
#' @param start the first tracking time to consider,
#' @param end the last tracking time to consider
#' @param showProgress display query progress in the console
#' @param singleThreaded enforce the use of a single thread.
#' @return a named list of two elements:\itemize{\item\code{$frame}:
#'     a \code{data.frame} summarizing the time, space and dimensions
#'     of each frame \item\code{$positions} a list of \code{data.frame}
#'     with the position of each ant for that frame. Each element of
#'     this list correspond to the same row in \code{$frames}
#'     \item{$collisions} a \code{data.frame} with all collision
#'     happening between **start** and **end**. The field
#'     \code{$frame_row_index} indicates in which row in \code{frames}
#'     this collision happens.}
fmQueryCollideFrames <- function(experiment,
                                 start = fmTimeSinceEver(),
                                 end = fmTimeForever(),
                                 showProgress = TRUE,
                                 singleThreaded = FALSE) {
    pfmQueryCollideFrames(experiment,
                          start,
                          end,
                          showProgress,
                          singleThreaded)
}

#' Computes trajectories for ants.
#' @description Computes ant trajectories for the fmExperiment. A
#'     trajectory is the consecutive position of an Ant in a Space,
#'     with detection gap smaller than **maximumGap**. These will be
#'     reported ordered by ending time.
#' @param experiment the \code{\link{fmExperiment}} to query on
#' @param start the first tracking time to consider,
#' @param end the last tracking time to consider
#' @param maximumGap the maximum gap in tracking before cutting the
#'     trajectory in two different object. Use an insanely large value
#'     ( such as \code{fmHour(24*365)} to disable cutting of
#'     trajectory). Trajectory will always be cut when an ant moves
#'     from a space to another.
#' @param matcher a \code{\link{fmMatcher}} to reduce the query to
#'     wanted criterion. You can use \code{\link{fmMatcherAny}()} to
#'     match anything.
#' @param computeZones computes the zone of the Ant. Otherwise
#'     \code{0} will always be reported.
#' @param showProgress display query progress in the console
#' @param singleThreaded enforce the use of a single thread.
#' @return a named list of two elements:
#'     \itemize{\code{$trajectories_summary}: a \code{data.frame}
#'     summarizing the ant, space and starting time of the
#'     trajectory.\item\code{trajectories} a list of \code{data.frame}
#'     with the position of the ant during the trajectory \code{$time}
#'     is the offset from the trajectory \code{$start} time in
#'     seconds.
fmQueryComputeAntTrajectories <- function(experiment,
                                          start = fmTimeSinceEver(),
                                          end = fmTimeForever(),
                                          maximumGap = fmMillisecond(500),
                                          matcher = fmMatcherAny(),
                                          computeZones = FALSE,
                                          showProgress = TRUE,
                                          singleThreaded = FALSE) {
    pfmQueryComputeAntInteractions(experiment,
                                   start,
                                   end,
                                   maximumGap,
                                   matcher,
                                   computeZones,
                                   showProgress,
                                   singleThreaded)
}


#' Computes ant interactions for ants.
#' @description Computes ant interactions, i.e. time intervals where two Ants
#' collides. These will be reported ordered by ending time.
#'
#' @description the parameter **reportFullTrajectories** controls if
#'     full trajectories should be reported or if only a summary
#'     should be reported. The former have an high impact on the
#'     amount of RAM required to perform the query efficiently or at
#'     all.
#' @param experiment the \code{\link{fmExperiment}} to query on
#' @param start the first tracking time to consider,
#' @param end the last tracking time to consider
#' @param maximumGap the maximum gap in tracking before cutting the
#'     trajectory or interactions in two different object. Use an
#'     insanely large value ( such as \code{fmHour(24*365)} to disable
#'     cutting of trajectory). Trajectory will always be cut when an
#'     ant moves from a space to another.
#' @param matcher a \code{\link{fmMatcher}} to reduce the query to
#'     wanted criterion. You can use \code{\link{fmMatcherAny}()} to
#'     match anything.
#' @param reportFullTrajectories computes the zone of the
#'     Ant. Otherwise \code{0} will always be reported.
#' @param showProgress display query progress in the console
#' @param singleThreaded enforce the use of a single thread.
#' @return Depending on **reportFullTrajectories**:\itemize{\item if
#'     \code{FALSE}: reports a single \code{data.frame} with the
#'     interactions, with their start and end date, types, and a
#'     summary of each ant positions and zones.\item \code{TRUE} A
#'     named list of 3 elements: \itemize{\item
#'     \code{$trajectories_summary} the start time and space of each
#'     trajectories.\item\code{$trajectories} timed position of the
#'     ant during the trajectory.\item\code{$interactions}: a
#'     \code{data.frame} which contains the interactions start and end
#'     date, and the indexes of \code{$trajectories} that correspond
#'     to each ant sub-trajectory. I.e. for the first ant
#'     \itemize{\item\code{$ant1.trajectory.row} the corresponding
#'     index in \code{$trajectory}. \item\code{$ant1.trajectory.start}
#'     the row index in the \code{data.frame}
#'     \code{$trajectory[[$ant1.trajectory.row]]} that correspond to
#'     the interaction start time. \item\code{$ant1.trajectory.end}:
#'     likewise, the row index that correspond to the interaction
#'     end}}}
fmQueryComputeAntInteractions <- function(experiment,
                                          start = fmTimeSinceEver(),
                                          end = fmTimeForever(),
                                          maximumGap = fmMillisecond(500),
                                          matcher = fmMatcherAny(),
                                          reportFullTrajectories = FALSE,
                                          showProgress = TRUE,
                                          singleThreaded = FALSE) {
    pfmQueryComputeAntInteractions(experiment,
                                   start,
                                   end,
                                   maximumGap,
                                   matcher,
                                   reportFullTrajectories,
                                   showProgress,
                                   singleThreaded)
}
