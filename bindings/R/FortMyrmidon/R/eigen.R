#' @name fmVector2dList
#' @title Represents a list of 2D point
#' @description an object that is somewhat similar to a
#'     list. fmVector2dList can take a list of numerical vector or a
#'     dataframe with 'x', and 'y' column as an argument to create a
#'     fmVector2dList. Function and methods expecting a fmVector2dList
#'     will accept such list or data.frame as input.
#' @examples
#'     l <-fmVector2dListCreate(data.frame("x" = c(1,2,3), "y"= c(1,2,3)))
#'     \dontrun{as.list(l) # will give a list of numerical vector
#'     as.data.frame(l) # will give a dataframe}
#' @family fmVector2dList methods
NULL

#' @name fmVector2dList$size
#' @title returns the size of the Vector2dList
#' @description returns the size of the Vector2dList. Works also with
#'     the \code{length} function
#' @examples
#'     l <-fmVector2dListCreate(data.frame("x" = c(1,2,3), "y"= c(1,2,3)))
#'     l$size()
#'     \dontrun{length(l)}
#' @return the number of points in the list
#' @family fmVector2dList methods
NULL

#' @name fmVector2dList$[[
#' @title Access a point in the list
#' @param i the index to access
#' @examples
#'     l <-fmVector2dListCreate(data.frame("x" = c(1,2,3), "y"= c(1,2,3)))
#'     l[[1]]
#' @return the 2D point at i in the list
#' @family fmVector2dList methods
NULL

#' @name fmVector2dList$[[<-
#' @title Sets a point in the list
#' @param i the index to set
#' @param v the value to set to
#' @examples
#'     l <-fmVector2dListCreate(data.frame("x" = c(1,2,3), "y"= c(1,2,3)))
#'     l[[1]] <- c(3,4)
#' @family fmVector2dList methods
NULL

#' @name fmVector2dList$as.data.frame
#' @title Returns a copy of the list as a data.frame
#' @title Returns a copy of the list as a data.frame. Also works with
#'     the \code{as.data.frame} function
#' @examples
#'     l <-fmVector2dListCreate(data.frame("x" = c(1,2,3), "y"= c(1,2,3)))
#'     \dontrun{as.data.frame(l)}
#'     l$as.data.frame()
#' @return a copy of the list data organized in a data.frame
#' @family fmVector2dList methods
NULL

#' @name fmVector2dList$as.list
#' @title Returns a copy of the list as a plain R list
#' @title Returns a copy of the list as a plain R list. Also works with
#'     the \code{as.list} function
#' @examples
#'     l <-fmVector2dListCreate(data.frame("x" = c(1,2,3), "y"= c(1,2,3)))
#'     \dontrun{as.list(l)}
#'     l$as.list()
#' @return a copy of the list data organized in a list
#' @family fmVector2dList methods
NULL
