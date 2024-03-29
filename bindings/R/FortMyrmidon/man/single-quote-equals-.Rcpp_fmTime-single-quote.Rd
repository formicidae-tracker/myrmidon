% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/time.R
\name{'==.Rcpp_fmTime'}
\alias{'==.Rcpp_fmTime'}
\title{Equal operator for fmTime}
\arguments{
\item{e1}{the first fmTime to compare}

\item{e2}{the second fmTime to compare}
}
\value{
\code{TRUE} if \code{e1} and  \code{e2} represents the same time at nanoseconds precision
}
\description{
Equal operator for fmTime
}
\examples{
\dontrun{
fmTimeParse("2020-01-29T08:00:00.000Z") == fmTimeParse("2020-01-29T08:00:00.000Z")
fmTimeParse("2020-01-29T08:00:00.000Z") == fmTimeParse("2020-01-29T08:00:00.000Z")$add(1)
}
}
\seealso{
Other fmTime methods: 
\code{\link{'<.Rcpp_fmTime'}},
\code{\link{'<=.Rcpp_fmTime'}},
\code{\link{'>.Rcpp_fmTime'}},
\code{\link{'>=.Rcpp_fmTime'}},
\code{\link{fmTime$add}},
\code{\link{fmTime$after}},
\code{\link{fmTime$asPOSIXct}},
\code{\link{fmTime$before}},
\code{\link{fmTime$equals}},
\code{\link{fmTime$isForever}},
\code{\link{fmTime$isInfinite}},
\code{\link{fmTime$isSinceEver}},
\code{\link{fmTime$round}},
\code{\link{fmTime$sub}},
\code{\link{fmTimeCreate}()},
\code{\link{fmTimeForever}()},
\code{\link{fmTimeNow}()},
\code{\link{fmTimeParse}()},
\code{\link{fmTimeSinceEver}()},
\code{\link{fmTime}}
}
\concept{fmTime methods}
