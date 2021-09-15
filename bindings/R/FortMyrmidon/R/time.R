#' @name fmTime
#' @title Represents a time
#' @description a fmTime represents a Time issued by the tracking
#'     system. These times are more precise than the natural ctPOSIX
#'     object used by R. They can also describe the infinite time
#'     \code{\link{fmTimeSinceEver}} and \code{\link{fmTimeForever}}
#' @family fmTime methods
NULL

#' @name fmTime$after
#' @title tests if the fmTime object is strictly after another fmTime
#' @param other the other fmTime to test for
#' @family fmTime methods
#' @return \code{TRUE} if this fmTime is after other
NULL

#' @name fmTime$before
#' @title tests if the fmTime object is strictly before another fmTime
#' @param other the other fmTime to test for
#' @family fmTime methods
#' @return \code{TRUE} if this fmTime is before other
NULL

#' @name fmTime$equals
#' @title tests if the fmTime object is euals to another fmTime
#' @param other the other fmTime to test for
#' @family fmTime methods
#' @return \code{TRUE} if this fmTime is equal to other
NULL

#' @name fmTime$add
#' @title add a \code{\link{fmDuration}} to this \code{\link{fmTime}}
#' @param duration the duration to add
#' @examples
#' \dontrun{
#' fmTimeCreate()$add(fmSecond(1)) == fmTimeCreate(1.0) # this is the same
#' }
#' @family fmTime methods
#' @return a new \code{\link{fmTime}} which is duration after this fmTime
NULL


#' @name fmTime$sub
#' @title computes the time difference between two times
#' @param other the other time to substract
#' @examples
#' start <- fmTimeNow()
#' # do some computation
#' ellapsed <- fmTimeNow()$sub(start)
#' @family fmTime methods
#' @return a \code{\link{fmDuration}} which the difference between this fmTime and other
NULL

#' @name fmTime$round
#' @title rounds a time to the nearest duration
#' @param duration the \code{\link{fmDuration}} to round to.
#' @examples
#' fmTimeNow()$round(fmHour(1)) # current time rounded to one hour
#' @family fmTime methods
#' @return a new \code{\link{fmTime}} which this time rounded to duration
NULL

#' @name fmTime$asPOSIXct
#' @title Converts to a R datetime object
#' @examples fmTimeNow()$asPOSIXct # equivalent to Sys.time()
#' @family fmTime methods
#' @return a datetime object representing this time this value can be
#'     Inf or -Inf if this isInfinite()
NULL

#' @name fmTime$isInfinite
#' @title Tests if it is an infinite time
#' @examples fmTimeNow()$isInfinite() # should be FALSE
#' @family fmTime methods
#' @return TRUE if this time is fmTimeForever() or fmTimeSinceEver()
NULL

#' @name fmTime$isForever
#' @title Tests if this fmTime is fmTimeForever()
#' @examples fmTimeForever()$isForever() # is always TRUE
#' @family fmTime methods
#' @return TRUE if this time is fmTimeForever()
NULL

#' @name fmTime$isSinceEver
#' @title Tests if this fmTime is fmTimeSinceEver()
#' @examples fmTimeForever()$isSinceEver() # is always TRUE
#' @family fmTime methods
#' @return TRUE if this time is fmTimeSinceEver()
NULL


#' @name '<.Rcpp_fmTime'
#' @title Comparison operator for fmTime
#' @param e1 the first fmTime to compare
#' @param e2 the second fmTime to compare
#' @return \code{TRUE} if \code{e1} represents a time before \code{e2}
#' @examples
#' \dontrun{
#' fmTimeParse("2020-01-29T08:00:00.000Z") < fmTimeParse("2020-01-29T08:00:01.000Z")
#' fmTimeParse("2020-01-29T08:00:00.000Z") < fmTimeParse("2020-01-29T08:00:00.000Z")
#' }
#' @family fmTime methods
NULL

#' @name '<=.Rcpp_fmTime'
#' @title Comparison operator for fmTime
#' @param e1 the first fmTime to compare
#' @param e2 the second fmTime to compare
#' @return \code{TRUE} if \code{e1} represents a time before or equal to \code{e2}
#' @examples
#' \dontrun{
#' fmTimeParse("2020-01-29T08:00:00.000Z") <= fmTimeParse("2020-01-29T08:00:01.000Z")
#' fmTimeParse("2020-01-29T08:00:00.000Z") <= fmTimeParse("2020-01-29T08:00:00.000Z")
#' }
#' @family fmTime methods
NULL

#' @name '>.Rcpp_fmTime'
#' @title Comparison operator for fmTime
#' @param e1 the first fmTime to compare
#' @param e2 the second fmTime to compare
#' @return \code{TRUE} if \code{e1} represents a time after \code{e2}
#' @examples
#' \dontrun{
#' fmTimeParse("2020-01-29T08:00:01.000Z") > fmTimeParse("2020-01-29T08:00:00.000Z")
#' fmTimeParse("2020-01-29T08:00:00.000Z") > fmTimeParse("2020-01-29T08:00:00.000Z")
#' }
#' @family fmTime methods
NULL

#' @name '>=.Rcpp_fmTime'
#' @title Comparison operator for fmTime
#' @param e1 the first fmTime to compare
#' @param e2 the second fmTime to compare
#' @return \code{TRUE} if \code{e1} represents a time after or equal to \code{e2}
#' @examples
#' \dontrun{
#' fmTimeParse("2020-01-29T08:01:00.000Z") >= fmTimeParse("2020-01-29T08:00:00.000Z")
#' fmTimeParse("2020-01-29T08:00:00.000Z") >= fmTimeParse("2020-01-29T08:00:00.000Z")
#' }
#' @family fmTime methods
NULL

#' @name '==.Rcpp_fmTime'
#' @title Equal operator for fmTime
#' @param e1 the first fmTime to compare
#' @param e2 the second fmTime to compare
#' @return \code{TRUE} if \code{e1} and  \code{e2} represents the same time at nanoseconds precision
#' @examples
#' \dontrun{
#' fmTimeParse("2020-01-29T08:00:00.000Z") == fmTimeParse("2020-01-29T08:00:00.000Z")
#' fmTimeParse("2020-01-29T08:00:00.000Z") == fmTimeParse("2020-01-29T08:00:00.000Z")$add(1)
#' }
#' @family fmTime methods
NULL


#' @name '+.Rcpp_fmDuration'
#' @title Addition operator for fmDuration
#' @param e1 the first fmDuration to add
#' @param e2 the second fmDuration to add
#' @return the sum of the two fmDuration
#' @examples
#' \dontrun{
#' fmSecond(2) + fmMicrosecond(300)
#' }
#' @family fmDuration methods
NULL

#' @name '-.Rcpp_fmDuration'
#' @title Substract operator for fmDuration
#' @param e1 the first fmDuration to substract
#' @param e2 the second fmDuration to substract
#' @return the difference of the two duration
#' @examples
#' \dontrun{
#' fmSecond(1) - fmMillisecond(1)
#' }
#' @family fmDuration methods
NULL

#' @name '<.Rcpp_fmDuration'
#' @title Comparison operator for fmDuration
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 < e2}
#' @examples
#' \dontrun{
#' fmSecond(1) < fmMinute(1)
#' fmSecond(60) < fmMinute(1)
#' }
#' @family fmDuration methods
NULL

#' @name '<=.Rcpp_fmDuration'
#' @title Comparison operator for fmDuration
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 <= e2}
#' @examples
#' \dontrun{
#' fmSecond(1) <= fmMinute(1)
#' fmSecond(60) <= fmMinute(1)
#' }
#' @family fmDuration methods
NULL

#' @name '>=.Rcpp_fmDuration'
#' @title Comparison operator for fmDuration
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 >= e2}
#' @examples
#' \dontrun{
#' fmMinute(1) >= fmSecond(1)
#' fmMinute(60) >= fmSecond(60)
#' }
#' @family fmDuration methods
NULL

#' @name '>.Rcpp_fmDuration'
#' @title Comparison operator for fmDuration
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 > e2}
#' @examples
#' \dontrun{
#' fmMinute(1) > fmSecond(1)
#' fmMinute(1) > fmSecond(60)
#' }
#' @family fmDuration methods
NULL

#' @name '==.Rcpp_fmDuration'
#' @title Equal operator for fmDuration
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 == e2}
#' @examples
#' \dontrun{
#' fmSecond(1) == fmMinute(1)
#' fmSecond(60) == fmMinute(1)
#' }
#' @family fmDuration methods
NULL


#' @name fmDuration
#' @title Represents a duration with nanosecond precision
#' @description a fmDuration represents a duration with nanosecond
#'     precision
#' @family fmDuration methods
NULL

#' @name Rcpp_fmDuration-class
#' @title Internal Object
#' @description representation for \code{\link{fmDuration}}
#' @family internals
NULL

#' @name Rcpp_fmTime-class
#' @title Internal Object
#' @description representation for \code{\link{fmTime}}
#' @family internals
NULL
