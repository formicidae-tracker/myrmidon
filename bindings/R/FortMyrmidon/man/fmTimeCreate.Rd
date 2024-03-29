% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{fmTimeCreate}
\alias{fmTimeCreate}
\title{Creates a fmTime from an offset in second from the system's epoch}
\usage{
fmTimeCreate(offset = 0)
}
\arguments{
\item{offset}{the offset in second, Inf and -Inf are valid and
respectively represents fmTimeForever() and fmTimeSinceEver().}
}
\value{
the \code{\link{fmTime}} offseted by offset seconds from
the system's epoch
}
\description{
Creates a fmTime from an offset in second from the system's epoch
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
\code{\link{fmTimeForever}()},
\code{\link{fmTimeNow}()},
\code{\link{fmTimeParse}()},
\code{\link{fmTimeSinceEver}()},
\code{\link{fmTime}}
}
\concept{fmTime methods}
