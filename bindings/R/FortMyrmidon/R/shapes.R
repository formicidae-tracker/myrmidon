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
    new(fmCircle, center, radius)
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
fmCapsuleCreate <- function(c1 = c(0,0),c2 = c(1,1), r1 = 1.0, r2 = 1.0) {
    new(fmCapsule,c1,c2,r1,r2)
}

#' @name fmPolygon
#' @title Represents a close polygon
#' @description Represents a closed polygon. The polygon is always
#'     considered closed. The order does matters as
#'     `fmPolygonCreate(list(c(1,1),c(-1,1),c(-1,-1),c(1,-1)))` is a
#'     square and
#'     `fmPolygonCreate(list(c(1,1),c(-1,-1),c(1,-1),c(-1,1)))` is an
#'     hourglass.
#' @field vertices a \code{\link{fmVector2dList}} of the polygon vertices
#' @family fmShape methods and classes
NULL

#' Creates a polygon
#' @param vertices the polygon vertices
#' @return a \code{\link{fmPolygon}}
#' @family fmShape methods and classes
fmPolygonCreate <- function(vertices = NULL) {
    if (is.null(vertices)) {
        vertices = list(c(1,1),c(-1,1),c(-1,-1),c(1,-1))
    }
    new(fmPolygon,fmVector2dListCreate(vertices))
}


#' @name fmShapeList
#' @title A list of heterogenous 2D shapes
#' @description an object that is similar to a list, but could only
#'     accept fmShape. It is implicitly convertible from a native R
#'     list. It can also be copied with $as.list
#' @family fmShape methods and classes
NULL

#' @name fmShapeList$size
#' @title The size of the fmShapeList
#' @description the \code{length()} function can also be used.
#' @examples
#' l <- fmShapeListCreate()
#' l$size()
#' \dontrun{length(l)}
#' @return the size of the \code{\link{fmShapeList}}
#' @family fmShape methods and classes
NULL

#' @name fmShapeList$[[
#' @title Access elements from the list
#' @param i index of the element to access
#' @examples
#' l <- fmShapeListCreate(list(fmCircleCreate()))
#' l[[1]]$center = c(2,2) #
#' @return the element at the i positions
#' @family fmShape methods and classes
NULL

#' @name fmShapeList$[[<-
#' @title Access elements from the list
#' @param i index of the element to access
#' @param v the new shape to set at i
#' @examples
#' l <- fmShapeListCreate(list(fmCircleCreate()))
#' l[[1]] = fmCapsuleCreate()# I wanted a capsule there
#' @family fmShape methods and classes
NULL

#' @name fmShapeList$as.list
#' @title Copies the fmShapeList as a native R list
#' @return a R list of copies of all fmShape in the list
#' @examples
#' l <- fmShapeListCreate(list(fmCircleCreate()))
#' ll = l$as.list()
#' @family fmShape methods and classes
NULL
