% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/shapes.R
\name{fmPolygon}
\alias{fmPolygon}
\title{Represents a close polygon}
\description{
Represents a closed polygon. The polygon is always
considered closed. The order does matters as
\code{fmPolygonCreate(list(c(1,1),c(-1,1),c(-1,-1),c(1,-1)))} is a
square and
\code{fmPolygonCreate(list(c(1,1),c(-1,-1),c(1,-1),c(-1,1)))} is an
hourglass.
}
\section{Fields}{

\describe{
\item{\code{vertices}}{a \code{\link{fmVector2dList}} of the polygon vertices}
}}

\seealso{
Other fmShape methods and classes: 
\code{\link{fmCapsuleCreate}()},
\code{\link{fmCapsule}},
\code{\link{fmCircleCreate}()},
\code{\link{fmCircle}},
\code{\link{fmPolygonCreate}()},
\code{\link{fmShapeList$[[<-}},
\code{\link{fmShapeList$[[}},
\code{\link{fmShapeList$as.list}},
\code{\link{fmShapeList$size}},
\code{\link{fmShapeListCreate}()},
\code{\link{fmShapeList}},
\code{\link{fmShapeTypes}},
\code{\link{fmShape}}
}
\concept{fmShape methods and classes}
