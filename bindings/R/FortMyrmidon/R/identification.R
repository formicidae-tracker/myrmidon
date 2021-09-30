#' @name fmIdentification
#' @title Relates tag to fmAnt
#' @description Identifications relates tag to \code{\link{fmAnt}}
#'     with time and geometric data.
#' @description Identification can only be created from an
#'     \code{\link{fmExperiment}} with
#'     \code{\link{fmExperiment$addIdentification}()}.
#'
#' @description  Identifications are bounded in \code{\link{fmTime}} in the
#'     range [ $start , $end [. These attributes can respectively be
#'     set to \code{\link{fmTimeSinceEver}()} and
#'     \code{\link{fmTimeForever}()}. Internally **fort-myrmidon**
#'     ensure the validity of all Identifications. It means that:
#'     \itemize{
#'     \item Two Identifications using the same $tagValue cannot overlap in Time.
#'     \item Two Identifications using targeting the same \code{\link{fmAnt}} cannot overlap in Time.
#'     }
#'     If any modifications through
#'     \code{\link{fmExperiment$addIdentification}()}, $start or $end
#'     would violate one of these conditions, an error will be raised.
#'
#' @description Identifications also contains geometric informations
#'     on how the detected tag is related to the observed
#'     \code{\link{fmAnt}}. These are the translation and rotation of
#'     the Ant, expressed in the tag coordinate reference. Usually,
#'     this information is automatically generated from the manual
#'     measurement of type
#'     \code{\link{fmExperiment.HEAD_TAIL_MEASUREMENT_TYPE_ID}} made in
#'     **fort-studio**. Alternatively, users can override this
#'     behavior by setting themselves this pose using
#'     \code{\link{fmIdentification$setUserDefinedAntPose}()}. \code{\link{fmIdentification$clearUserDefinedAntPose}()}
#'     can be used to revert to the internally computed pose.
#'
#' @note Any angle is measured in radians, with a standard
#'     mathematical convention. Since in image processing the y-axis
#'     is pointing from the top of the image to the bottom, positive
#'     angles appears clockwise.
#'
#' @description Identifications also contains the information of the
#'     physical tag size used to identify the individual. It can be
#'     accessed and set with $tagSize and. The value \code{0.0}
#'     indicates that the \code{$defaultTagSize} field of
#'     \code{\link{fmExperiment}} should be used. Therefore, for most
#'     \code{\link{fmAnt}}, this field should be kept to \code{0.0},
#'     excepted for a few individuals, for examples, Queens.
#' @field tagValue the tag ID used for this identication
#' @field targetAntID the ant ID targetted by this identification
#' @field start the first valid \code{\link{fmTime}} for this
#'     identification
#' @field end the first invalid \code{\link{fmTime}} for this
#'     identification
#' @field antPosition the relative position of the ant in the tag
#'     reference
#' @field antAngle the relative angle of the ant to the tag
#' @field tagSize the physical tag size, if equal to \code{0.0} the
#'     \code{$defaultTagSize} field of \code{\link{fmExperiment}} will
#'     be used
#' @family fmIdentification methods
NULL

#' @name fmIdentification$hasDefaultTagSize
#' @title Indicates if the Experiment's default tag size should be used
#' @return \code{TRUE} when \code{$tagSize == 0.0}
#' @family fmIdentification methods
NULL

#' @name fmIdentification$hasUserDefinedAntPose
#' @title Indicates if the user overided the computed Ant Pose
#' @return \code{TRUE} when an ant pose was defined with
#'     \code{\link{fmIdentification$setUserDefinedAntPose}()}
#' @family fmIdentification methods
NULL

#' @name fmIdentification$clearUserDefinedAntPose
#' @title Removes the user defined ant pose.
#' @description The Ant pose will be reverted to the
#'     automatically computed one from **fort-studio**
#' @family fmIdentification methods
NULL


#' @name fmIdentification$setUserDefinedAntPose
#' @title Overrides the computed Ant Pose with a user defined one.
#' @description Sets an user defined ant position relatively to the
#'     tag, overiding the one computed from manual measurements.
#'
#' @description To revert to the automatically computed ones, use
#'     \code{\link{fmIdentification$clearUserDefinedAntPose}()}
#' @param antPosition the position of the ant relatively to the tag
#'     center, in the tag reference frame. It should be a numerical
#'     vector of size 2.
#' @param antAngle the ant angle relatively to the tag angle in radians.
#' @family fmIdentification methods
NULL
