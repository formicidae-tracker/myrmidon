#' @name fmAnt
#' @title The main object of an Experiment
#' @description Ant are the main object of interest of an
#'     \code{\link{fmExperiment}}. They are identified from tags with
#'     \code{\link{fmIdentification}}, have a virtual shape to perform
#'     collision and interaction detection, and holds user defined
#'     metadata.
#' @description Ant can only be created from an
#'     \code{\link{fmExperiment}} with
#'     \code{\link{fmExperiment$createAnt}()}.
#' @description Ant are uniquely identified by their $ID. By
#'     convention we use decimal notation with up to two ``0`` prefix
#'     to display these AntID, as returned by
#'     \code{\link{fmFormatAntID}()}.
#' @description Instead of working directly with tag ID
#'     **fort-myrmidon** uses \code{\link{fmIdentification}} to relate
#'     a tag value to an Ant. An Ants could have different
#'     identifications, allowing us to use different tag ID to refer
#'     to the same individuals. One would use
#'     \code{\link{fmAnt$identifiedAt}()} to obtain the tag ID that
#'     identifies an Ant at a given \code{\link{fmTime}}.
#' @description Each Ant has an associated virtual shape that is used
#'     to compute instantaneous collision detection (
#'     \code{\link{fmQueryCollideFrame}()} ), or timed ant interactions
#'     ( \code{\link{fmQueryComputeAntInteraction}()} ). These shapes
#'     can be defined manually in **fort-studio** or programmatically
#'     accessed with $capsules and modified with
#'     \code{\link{fmAnt$addCapsule}()},
#'     \code{\link{fmAnt$deleteCapsule}()} and
#'     \code{\link{fmAnt$clearCapsules}()}.
#' @description Basic visualization of Experiment data can be done
#'     through **fort-studio**. Ants are displayed according to their
#'     $displayStatus and $displayColor. $displayStatus should be one
#'     of the values described in \code{\link{fmAnt.DisplayState}}
#' @description Ant can stores timed user defined metadata. These are
#'     modifiable using \code{\link{fmAnt$setValue}()} and
#'     \code{\link{fmAnt$deleteValue}()} and accesible through
#'     \code{\link{fmAnt$getValue}()}.
#' @field identifications a read-only list of
#'     \code{\link{fmIdentification}} for this Ant sorted in
#'     \code{\link{fmTime}}
#' @field capsule a read-only list of named tuple
#'     (type:integer,capsule:\code{link{fmCapsule}}) that describes
#'     the virtual shape of the ant.
#' @field ID the unique ID for this Ant
#' @field displayColor the \code{\link{fmColor}} used to display this
#'     ant in **fort-studio**.
#' @field displayStatus the display status of the Ant in
#'     **fort-studio**, should only be one of the value in
#'     \code{\link{fmAnt.DisplayState}}
#' @family fmAnt methods
NULL

#' Describes the posible values for \code{$displayStatus} field of
#' \code{\link{fmAnt}}
#' @family fmAnt methods
fmAnt.DisplayState <- list('VISIBLE' = 0, 'HIDDEN' = 1, 'SOLO' = 2)

#' @name fmAnt$identifiedAt
#' @title Gets the tag ID identifiying this ant at a given time
#' @description Gets the tag ID identifiying this ant at a given
#'     **time**. An error will occurs if there is no identification at
#'     **time** fort this ant.
#' @param time a \code{\link{fmTime}} we want an identification for
#' @return an integer which is the identification for this ant at
#'     **time**.
#' @family fmAnt methods
NULL

#' @name fmAnt$getValue
#' @title Gets user defined timed metadata.
#' @param key a character for the wanted key.
#' @param time a \code{\link{fmTime}} we want the value for.
#' @return either a boolean, integer, numerical, character or
#'     \code{\link{fmTime}}.
#' @family fmAnt methods
NULL


#' @name fmAnt$setValue
#' @title Sets user defined timed metadata.
#' @description Sets **key** to **value** starting at **time**. Errors
#'     could occurs if:
#'     \itemize{
#'     \item **key** is not defined in \code{\link{fmExperiment}}.
#'     \item **time** is  \code{\link{fmTimeForever}()}.
#'     \item **value** is not the right type for **key**
#'     }
#' @param key a character for the wanted key.
#' @param value either a boolean, integer, numerical, character or
#'     \code{link{fmTime}} we want to set **key** to.
#' @param time a \code{\link{fmTime}} were **key** will become
#'     **value**.
#' @family fmAnt methods
NULL

#' @name fmAnt$deleteValue
#' @title Deletes a user defined metadata
#' @description Removes the value sets for **key** at **time**. An
#'     error will occurs if no value were previously set for **key**
#'     at **time**.
#' @param key a character for the wanted key.
#' @param time the time of the timed value we want to remove.
#' @family fmAnt methods
NULL

#' @name fmAnt$addCapsule
#' @title Adds a virtual shape body to the ant
#' @description Adds **capsule** as a virtual shape of type
#'     **shapeTypeID**. An error will occur if **shapeTypeID** was not
#'     defined for the \code{\link{fmExperiment}}.
#' @param shapeTypeID the shape type as an integer
#' @param capsule a \code{\link{fmCapsule}} to set the shape to.
#' @family fmAnt methods
NULL

#' @name fmAnt$deleteCapsule
#' @title Removes a capsule from the list of virtual shape parts.
#' @param index the index to remove (starts at 1)
#' @family fmAnt methods
NULL

#' @name fmAnt$clearCapsules
#' @title Removes all capsules from the virtual shape definition
#' @family fmAnt methods
NULL
