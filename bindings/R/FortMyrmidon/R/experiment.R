#' @name fmExperiment
#' @title API entry point
#' @description An fmExperiment olds a collection of
#'     \code{\link{fmAnt}}, \code{\link{fmIdentification}},
#'     \code{\link{fmSpace}} and \code{\link{fmZone}} and give access
#'     to the identified tracking data instantaneous collision and
#'     interaction detection through \code{\link{fmQuery}}.
#' @description fmExperiment are saved to the filesystem in
#'     `.myrmidon` files. One can use \code{\link{fmExperimentOpen}()}
#'     and \code{\link{fmExperiment$save}()} to interact with these
#'     files.
#' @description One can use
#'     \code{\link{fmExperiment$addTrackingDataDirectory}()} to link
#'     an fmExperiment with some tracking data, organized by
#'     **fort-leto** in a tracking data directory. This data must be
#'     assigned to a \code{\link{fmSpace}} (previously created with
#'     \code{\link{fmExperiment$createSpace}()}).  fmExperiment saves
#'     relative links to these tracking data directory. These paths
#'     are relative, so one can rename a `.myrmidon` file on the
#'     filesystem with \code{\link{fmExperiment$save}()}, but it must
#'     remains in the same directory.
#'@description In **fort-myrmidon**, tags are not used
#'     directly. Instead user are required to make \code{\link{fmAnt}}
#'     object (through \code{\link{fmExperiment$createAnt}()}) and use
#'     \code{\link{fmIdentification}} (through
#'     \code{\link{fmExperiment$addIdentification}()}) to relate a tag
#'     value to an \code{\link{fmAnt}}. To perform collision and
#'     interaction detection, users can create for each Ant a virtual
#'     shape, made of a collection of \code{\link{fmCapsule}}. Each
#'     Capsule is assigned an AntShapeTypeID (an integer starting from
#'     1) which must be previously defined using
#'     \code{\link{fmExperiment$createAntShapeType}()}. There is no
#'     predefined AntShapeTypeID.
#' @description **fort-studio** allows to make measurement on close-up
#'     of each \code{\link{fmAnt}}. These measurement must be assigned
#'     to a type, created with
#'     \code{\link{fmExperiment$createMeasurementType}()}. There is a
#'     predefined, undeletable MeasurementTypeID:
#'     \code{\link{fmExperiment.HEAD_TAIL_MEASUREMENT_TYPE_ID}}. It is
#'     used to automatically determine \code{$antPosition} and
#'     \code{$antAngle} fields of \code{\link{fmIdentification}} from
#'     **fort-studio** measurements.
#' @description Each \code{\link{fmAnt}} can also holds a dictionnary
#'     of key/value pairs. The key name, type and initial value for
#'     each Ant must be defined with
#'     \code{\link{fmExperiment$setMetaDataKey}()}. Through
#'     \code{\link{fmAnt$setValue}()}, individual, timed value can be
#'     assigned to each Ant. There are no predefined keys.
#' @field name a character that holds the fmExperiment name.
#' @field author a character that holds the fmExperiment author.
#' @field comment a character that holds the fmExperiment comment.
#' @field spaces a read-only, list of fmSpace of this fmExperiment.
#' @field ants a read-only, list of fmAnt of this fmExperiment.
#' @field absoluteFilePath a read-only, character holding the
#'     associated filepath of this fmExperiment.
#' @field defaultTagSize the default tag size in mm for
#'     fmIdentification in this fmExperiment.
#' @field mesurementTypeNames a read-only list of the fmExperiment
#'     defined measurement types
#' @field antShapeTypeNames a read-only list of the fmExperiment
#'     defined ant shape types
#' @field metaDataKeys a read-only list of the fmExperiment user
#'     defined metadata keys.
#' @family fmExperiment methods
NULL

#' @name fmExperiment$save
#' @title Saves the fmExperiment to the filesystem
#' @description Saves the fmExperiment to the filesystem. Errors
#'     occurs if this method is used to change the directory of the
#'     current fmExperiment.
#' @param filepath the filepath to save the fmExperiment to
#' @family fmExperiment methods
NULL

#' @name fmExperiment$createSpace
#' @title Creates a new fmSpace for this fmExperiment
#' @description Create a new \code{\link{fmSpace}} for this fmExperiment
#' @param name the name for the new fmSpace
#' @return the newly created \code{\link{fmSpace}}
#' @family fmExperiment methods
NULL

#' @name fmExperiment$deleteSpace
#' @title Deletes a fmSpace from this fmExperiment
#' @description Deletes a \code{\link{fmSpace}} from this
#'     fmExperiment. An error occurs if **spaceID** is not valid for
#'     this fmExperiment.
#' @param spaceID the $ID of the fmSpace to remove
#' @family fmExperiment methods
NULL

#' @name fmExperiment$addTrackingDataDirectory
#' @title Associates a tracking data directory with this fmExperiment.
#' @description Associates a tracking data directory to a
#'     \code{\link{fmSpace}} of this fmExperiment. An error could
#'     occur if either: \itemize{ \item **spaceID** is not valid for
#'     this fmExperiment.  \item **filepath** is not a valid tracking
#'     data directory \item **filepath** data would overlap in Time
#'     with data from another directory in **spaceID** \item
#'     **filepath** is associated with another fmSpace \item
#'     **filepath** is using a tag family that is different from the
#'     other directory associated in this experiment.  }
#' @param spaceID the $ID of the fmSpace to associate data with
#' @param filepath the filepath to the tracking data directory
#' @return the URI used to designate this directory in this fmExperiment
#' @family fmExperiment methods
NULL

#' @name fmExperiment$removeTrackingDataDirectory
#' @title Removes a tracking data directory from this fmExperiment.
#' @description Removes a tracking data directory from this
#'     fmExperiment. An error occurs if **URI** does not designate a
#'     tracking data directory from this fmExperiment.
#' @param URI the URI of the tracking data directory to remove
#' @family fmExperiment methods
NULL


#' @name fmExperiment$createAnt
#' @title Creates a new fmAnt for this fmExperiment
#' @description Creates a new \code{\link{fmAnt}} for this
#'     fmExperiment
#' @return the newly create \code{\link{fmAnt}}
#' @family fmExperiment methods
NULL


#' @name fmExperiment$deleteAnt
#' @title Removes a fmAnt from this fmExperiment
#' @description Removes a \code{\link{fmAnt}} from this
#'     fmExperiment. An error occurs if **antID** is not valid for
#'     this fmExperiment.
#' @param antID the $ID of the \code{\link{fmAnt}} to remove.
#' @family fmExperiment methods
NULL

#' @name fmExperiment$addIdentification
#' @title Adds an fmIdentication to this fmExperiment
#' @description Adds a \code{\link{fmIdentification}} to this
#'     fmExperiment. An error occurs if any time overlap with other
#'     fmIdentification would occurs for **antID** or for **tagID**.
#' @param antID the $ID of the \code{\link{fmAnt}} to identify.
#' @param tagID the tag value to associate with **antID**.
#' @param start the first valid time for this fmIdentification. Could
#'     be \code{\link{fmTimeSinceEver}()}.
#' @param end the first invalid time for this fmIdentification. Could
#'     be \code{\link{fmTimeForever}()}.
#' @return the newly created \code{\link{fmIdentification}}
#' @family fmExperiment methods
NULL

#' @name fmExperiment$deleteIdentification
#' @title Removes an fmIdentication from this fmExperiment
#' @description Removes a \code{\link{fmIdentification}} from this
#'     fmExperiment. An error occurs if the passed object is not valid
#'     for this fmExperiment.
#' @param identification the fmIdentification to remove
#' @family fmExperiment methods
NULL

#' @name fmExperiment$freeIdentificationRangeAt
#' @title Returns an available time range for a tag.
#' @description Returns an available time range for a tag. An error
#'     occurs if **tagID** is used in an fmIdentification at **time**.
#' @param tagID the tag value to query for
#' @param time the \code{\link{fmTime}} to find a range for.
#' @return a named list ($start,$end) containing the available time
#'     range containing **time**.
#' @family fmExperiment methods
NULL

#' The default available measurement type of any fmExperiment
#' @family fmExperiment methods
fmExperiment.HEAD_TAIL_MEASUREMENT_TYPE_ID <- 1L

#' @name fmExperiment$createMeasurementType
#' @title Create a new measurement type for this fmExperiment.
#' @description Create a new measurement type for this fmExperiment.
#' @param name the name for the new measurement type
#' @return the new measurement type ID that will identifies this
#'     measurement type.
#' @family fmExperiment methods
NULL

#' @name fmExperiment$setMeasurementTypeName
#' @title Renames a measurement type for this fmExperiment.
#' @description Renames a measurement type for this fmExperiment. An
#'     error occurs if **typeID** is not valid for this fmExperiment.
#' @param typeID the measurement typeID to rename.
#' @param name the new name for the measurement type.
#' @family fmExperiment methods
NULL

#' @name fmExperiment$deleteMeasurementType
#' @title Removes a measurement type for this fmExperiment.
#' @description Removes a measurement type for this fmExperiment. An
#'     error occurs if **typeID** is not valid for this fmExperiment
#'     or if the fmExperiment contains measurement for this type or if
#'     \code{\link{fmExperiment.HEAD_TAIL_MEASUREMENT_TYPE_ID}} is
#'     removed.
#' @param typeID the measurement typeID to remove.
#' @family fmExperiment methods
NULL


#' @name fmExperiment$createAntShapeType
#' @title Create a new ant shape type for this fmExperiment.
#' @description Create a new ant shape type for this fmExperiment.
#' @param name the name for the new ant shape type
#' @return the new ant shape type ID that will identifies this
#'     ant shape type.
#' @family fmExperiment methods
NULL

#' @name fmExperiment$setAntShapeTypeName
#' @title Renames an ant shape type for this fmExperiment.
#' @description Renames an ant shape type for this fmExperiment. An
#'     error occurs if **shapeTypeID** is not valid for this fmExperiment.
#' @param shapeTypeID the ant shape typeID to rename.
#' @param name the new name for the ant shape type.
#' @family fmExperiment methods
NULL

#' @name fmExperiment$deleteAntShapeType
#' @title Removes an ant shape type for this fmExperiment.
#' @description Removes a measurement type for this fmExperiment. An
#'     error occurs if **shapeTypeID** is not valid for this fmExperiment
#'     or if at least one fmAnt contains a fmCapsule of this type.
#' @param typeID the ant shape type ID to remove.
#' @family fmExperiment methods
NULL

#' @name fmExperiment$setMetaDataKey
#' @title Sets or creates the name, type and default value for an user
#'     defined metadata key.
#' @description Sets or creates the name, type and default value for
#'     an user defined metadata key. the type is inferred from
#'     **defaultValue** which should either be a logical, integer,
#'     numerical, character or \code{\link{fmTime}} value. An error
#'     could occurs if **key** is already defined, **defaultValue**
#'     would change the type for **key** and at least one fmAnt has a
#'     defined timed data for **key**.
#' @param key the key to define
#' @param defaultValue the default value for **key**
#' @family fmExperiment methods
NULL

#' @name fmExperiment$renameMetaDataKey
#' @title Renames a metadata key.
#' @description Renames a metadata key. An error occurs if **oldKey**
#'     is unknown or if **newKey** is already used.
#' @param oldKey the key to rename
#' @param newKey the new name for the key
#' @family fmExperiment methods
NULL

#' @name fmExperiment$deleteMetaDataKey
#' @title Deletes a metadata key.
#' @description Deletes a metadata key. An error occurs if **key** is
#'     unknown or if at least one Ant contains timed data for **key**.
#' @param key the key to remove
#' @family fmExperiment methods
NULL

#' @name fmExperiment$identificationsAt
#' @title Obtains the AntID <-> TagID correspondances for a time
#' @description Obtains the AntID <-> TagID correspondances for a
#'     given **time** as a list. Index of the list are the AntID, and
#'     the value the corresponding TagID. If an AntID does not exist
#'     or is not identified by any TagID, the corresponding value in
#'     the list will be NULL.
#' @return a list of TagID indexed by AntID.
#' @param time the time to query identification for.
#' @family fmExperiment methods
NULL

#' Enumerates the possible tag family used in the FORT system.
fmTagFamily <- c("Tag36h11" = 0,
                 "Tag36h10" = 1,
                 "Tag36ARTag" = 2,
                 "Tag16h5" = 3,
                 "Tag25h9" = 4,
                 "Circle21h7" = 5,
                 "Circle49h12" = 6,
                 "Custom48h12" = 7,
                 "Standard41h12" = 8,
                 "Standard52h13" = 9,
                 "Undefined" = 10)
