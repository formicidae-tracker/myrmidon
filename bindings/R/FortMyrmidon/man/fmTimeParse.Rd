% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{fmTimeParse}
\alias{fmTimeParse}
\title{Parses a time from a RFC3339 string representation}
\usage{
fmTimeParse(input)
}
\arguments{
\item{input}{the string to parse}
}
\value{
a \code{\link{fmTime}} representing \code{input}.
}
\description{
Parses a time from a RFC3339 (
i.e. '1970-01-01T00:00:00.000Z' for UNIX epoch) string
representation to a fmTime
}
\examples{
fmTimeParse('1970-01-01T00:00:00.000Z')$equals(fmTimeCreate()) # will be TRUE on UNIX systems
}
\seealso{
Other fmTime methods: 
\code{\link{'<.Rcpp_fmTime'}},
\code{\link{'<=.Rcpp_fmTime'}},
\code{\link{'==.Rcpp_fmTime'}},
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
\code{\link{fmTimeSinceEver}()},
\code{\link{fmTime}}
}
\concept{fmTime methods}
