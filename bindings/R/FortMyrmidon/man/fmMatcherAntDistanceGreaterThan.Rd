% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{fmMatcherAntDistanceGreaterThan}
\alias{fmMatcherAntDistanceGreaterThan}
\title{A fmMatcher that matches ants distances}
\usage{
fmMatcherAntDistanceGreaterThan(distance)
}
\arguments{
\item{distance}{the distance in pixel to be greater}
}
\value{
a \code{\link{fmMatcher}} that matches when two Ant lies
apart of the given distance.
}
\description{
A fmMatcher that matches ants distances
}
\details{
In the case of trajectories, it matches anything.
}
\seealso{
Other fmMatcher methods: 
\code{\link{fmMatcherAnd}()},
\code{\link{fmMatcherAntAngleGreaterThan}()},
\code{\link{fmMatcherAntAngleSmallerThan}()},
\code{\link{fmMatcherAntDisplacement}()},
\code{\link{fmMatcherAntDistanceSmallerThan}()},
\code{\link{fmMatcherAntID}()},
\code{\link{fmMatcherAntMetaData}()},
\code{\link{fmMatcherAny}()},
\code{\link{fmMatcherInteractionType}()},
\code{\link{fmMatcherOr}()},
\code{\link{fmMatcher}}
}
\concept{fmMatcher methods}
