#' @name fmZoneDefinition
#' @title Defines the geometry of a Zone for a given Time range
#' @description Defines the geometry of a Zone for a given Time range.
#' @field shapes a \link{\code{fmShapeList}} defining the zone
#'     geometry
#' @field start the first valid \link{\code{fmTime}} for the zone
#' @field end the last valid \link{\code{fmTime}} for the zone
#' @description fmZoneDefinition can only be created from a
#'     \link{\code{fmZone}} with \link{\code{fmZone$addDefinition}}
#' @family fmZone classes and methods
NULL

#' @name fmZone
#' @title A geometric zone of a space
#' @description A fmZone represents a geometric zone of a
#'     \link{\code{fmSpace}} for tracking localization and
#'     interactions. It means that two Ant, which are lying in two
#'     separate Zone will never report a collision or Interaction.
#' @description fmZone can only be created from a
#'     \link{\code{fmSpace}} with \link{\code{fmSpace$createZone}}.
#' @details Zones are uniquely idenrified trough their ID in an
#'     \link{\code{fmExperiment}}, but they are managed and are
#'     related to a single \link{\code{fmSpace}}. They also have a
#'     user defined name, but it is never used internally.
#' @details By default an \link{\code{Ant}} lies in no fmZone, which
#'     is identified by the ZoneID 0.
#' @details Zone have time valid \link{\code{ZoneDefinition}} which
#'     represents their geometry. In most cases a Zone will have a
#'     single ZoneDefinition valid for
#'     ]\link{\code{fmTimeSinceEver}};\link{\code{fmTimeForever}}[. However
#'     it is possible to add as many different definition to a Zone,
#'     as long as they do not overlap in Time. The defintions are
#'     manipulated with \link{\code{fmZone$addDefinition}} and
#'     \link{\code{fmZone$deleteDefinition}}.
#' @field name the zone name
#' @field ID the zone ID, which is unique in the experiment
#' @field definitions (read-only) the zone definitions
#' @family fmZone classes and methods
NULL

#' @name fmZone$addDefinition
#' @title Adds a new fmZoneDefinition to the fmZone.
#' @description Adds a new \link{\code{fmZoneDefinition}} to the
#'     fmZone.
#' @param shapes the geometry
#' @param start the first valid \link{\code{fmTime}} for this definition.
#' @param end the first invalid \link{\code{fmTime}} for this definition.
#' @return a \link{\code{fmZoneDefinition}}
#' @family fmZone classes and methods
NULL

#' @name fmZone$deleteDefinition
#' @title Removes a fmZoneDefinition to the fmZone.
#' @description Removes a \link{\code{fmZoneDefinition}} to the
#'     fmZone.
#' @param index the index of the definition to remove
#' @family fmZone classes and methods
NULL
