#' @name fmZoneDefinition
#' @title Defines the geometry of a Zone for a given Time range
#' @description Defines the geometry of a Zone for a given Time range.
#' @field shapes a \code{\link{fmShapeList}} defining the zone
#'     geometry
#' @field start the first valid \code{\link{fmTime}} for the zone
#' @field end the last valid \code{\link{fmTime}} for the zone
#' @description fmZoneDefinition can only be created from a
#'     \code{\link{fmZone}} with \code{\link{fmZone$addDefinition}()}
#' @family fmZone classes and methods
NULL

#' @name fmZone
#' @title A geometric zone of a space
#' @description A fmZone represents a geometric zone of a
#'     \code{\link{fmSpace}} for tracking localization and
#'     interactions. It means that two Ant, which are lying in two
#'     separate Zone will never report a collision or Interaction.
#' @description fmZone can only be created from a
#'     \code{\link{fmSpace}} with \code{\link{fmSpace$createZone}()}.
#' @details Zones are uniquely idenrified trough their ID in an
#'     \code{\link{fmExperiment}}, but they are managed and are
#'     related to a single \code{\link{fmSpace}}. They also have a
#'     user defined name, but it is never used internally.
#' @details By default an \code{\link{fmAnt}} lies in no fmZone, which
#'     is identified by the ZoneID 0.
#' @details Zone have time valid \code{\link{fmZoneDefinition}} which
#'     represents their geometry. In most cases a Zone will have a
#'     single ZoneDefinition valid for
#'     ]\code{\link{fmTimeSinceEver}()};\code{\link{fmTimeForever}()}[. However
#'     it is possible to add as many different definition to a Zone,
#'     as long as they do not overlap in Time. The defintions are
#'     manipulated with \code{\link{fmZone$addDefinition}()} and
#'     \code{\link{fmZone$deleteDefinition}()}.
#' @field name the zone name
#' @field ID the zone ID, which is unique in the experiment
#' @field definitions (read-only) the zone definitions
#' @family fmZone classes and methods
NULL

#' @name fmZone$addDefinition
#' @title Adds a new fmZoneDefinition to the fmZone.
#' @description Adds a new \code{\link{fmZoneDefinition}} to the
#'     fmZone.
#' @param shapes the geometry
#' @param start the first valid \code{\link{fmTime}} for this definition.
#' @param end the first invalid \code{\link{fmTime}} for this definition.
#' @return a \code{\link{fmZoneDefinition}}
#' @family fmZone classes and methods
NULL

#' @name fmZone$deleteDefinition
#' @title Removes a fmZoneDefinition to the fmZone.
#' @description Removes a \code{\link{fmZoneDefinition}} to the
#'     fmZone.
#' @param index the index of the definition to remove
#' @family fmZone classes and methods
NULL
