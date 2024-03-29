% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/experiment.R
\name{fmExperiment$deleteMeasurementType}
\alias{fmExperiment$deleteMeasurementType}
\title{Removes a measurement type for this fmExperiment.}
\arguments{
\item{typeID}{the measurement typeID to remove.}
}
\description{
Removes a measurement type for this fmExperiment. An
error occurs if \strong{typeID} is not valid for this fmExperiment
or if the fmExperiment contains measurement for this type or if
\code{\link{fmExperiment.HEAD_TAIL_MEASUREMENT_TYPE_ID}} is
removed.
}
\seealso{
Other fmExperiment methods: 
\code{\link{fmExperiment$addIdentification}},
\code{\link{fmExperiment$addTrackingDataDirectory}},
\code{\link{fmExperiment$createAntShapeType}},
\code{\link{fmExperiment$createAnt}},
\code{\link{fmExperiment$createMeasurementType}},
\code{\link{fmExperiment$createSpace}},
\code{\link{fmExperiment$deleteAntShapeType}},
\code{\link{fmExperiment$deleteAnt}},
\code{\link{fmExperiment$deleteIdentification}},
\code{\link{fmExperiment$deleteMetaDataKey}},
\code{\link{fmExperiment$deleteSpace}},
\code{\link{fmExperiment$ensureAllDataIsLoaded}},
\code{\link{fmExperiment$freeIdentificationRangeAt}},
\code{\link{fmExperiment$identificationsAt}},
\code{\link{fmExperiment$removeTrackingDataDirectory}},
\code{\link{fmExperiment$renameMetaDataKey}},
\code{\link{fmExperiment$save}},
\code{\link{fmExperiment$setAntShapeTypeName}},
\code{\link{fmExperiment$setMeasurementTypeName}},
\code{\link{fmExperiment$setMetaDataKey}},
\code{\link{fmExperiment.HEAD_TAIL_MEASUREMENT_TYPE_ID}},
\code{\link{fmExperimentCreate}()},
\code{\link{fmExperimentOpen}()},
\code{\link{fmExperiment}}
}
\concept{fmExperiment methods}
