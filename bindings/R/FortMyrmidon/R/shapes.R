#' Describes the possible values for fmShape$shapeType
#' @family fmShape methods and classes
fmShapeTypes <- list( CAPSULE=0L, CIRCLE=1L, POLYGON=2L)

#' @name fmShape
#' @title Represents a 2D shape
#' @description A fmShape describe a 2D geometric shape
#' @field shapeType the shape type, which would be one of the values in fmShapeTypes
#' @family fmShape methods and classes
NULL

#' @name fmCircle
#' @title Represents a circle
#' @description A \code{\link{fmShape}} that describe a circle
#' @field center the circle's center as a numeric vector
#' @field radius the circle's radius
#' @family fmShape methods and classes
NULL


#' Creates a circle
#' @param center the circle's center
#' @param radius the circle's radius
#' @return a \code{\link{fmCircle}}
#' @family fmShape methods and classes
fmCircleCreate <- function(center = c(0,0),radius = 1.0) {
    fmpCircleCreate(center,radius)
}

#' @name fmCapsule
#' @title Represents a capsule
#' @description A \code{\link{fmShape}} that describe a capsule,
#'     i.e. the region inside and between two circles
#' @field c1 the first circle's center as a numeric vector
#' @field c2 the second circle's center as a numeric vector
#' @field r1 the first circle's radiu
#' @field r2 the second circle's radius
#' @family fmShape methods and classes
NULL


#' Creates a capsule
#' @param c1 the first circle's center
#' @param c2 the second circle's center
#' @param r1 the first circle's radius
#' @param r2 the second circle's radius
#' @return a \code{\link{fmCapsule}}
#' @family fmShape methods and classes
fmCircleCreate <- function(c1 = c(0,0),c2 = c(1,1), r1 = 1.0,r2 = 1.0) {
    fmpCapsuleCreate(c1,c2,r1,r2)
}
