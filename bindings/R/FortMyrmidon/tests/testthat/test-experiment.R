local_data <- function() {
    return(list("experiment" = fmExperimentCreate(file.path(ud$Basedir,"public-experiment-utest.myrmidon"))))
}

test_that("it can manipulate files",{
    dirs <- list(file.path(ud$Basedir,"test-manipulation"),
                 file.path(ud$Basedir,"test-manipulation-new"))
    for(d in dirs) {
        dir.create(d, recursive = TRUE, mode = '0755')
    }
    filepath = file.path(dirs[[1]],"test.myrmidon")
    goodNewPath = file.path(dirs[[1]],"test2.myrmidon")
    badNewPath = file.path(dirs[[2]],"test.myrmidon")
    e <- fmExperimentCreate(filepath)
    e$save(filepath)
    e$save(goodNewPath)
    expect_error({
        e$save(badNewPath)
    },'Changing experiment file directory is not yet supported')

})


test_that("it can manipulate spaces",{
    d <- local_data()
    spaces <- list(d$experiment$createSpace("nest"),
                   d$experiment$createSpace("foraging"))
    expect_length(d$experiment$spaces,2)
    for( s in spaces ) {
        expect_equal(d$experiment$spaces[[s$ID]]$get(),s$get())
    }
    expect_error({
        d$experiment$deleteSpace(42)
    },"Unknown SpaceID 42")
    d$experiment$deleteSpace(spaces[[1]]$ID)
    expect_length(d$experiment$spaces,2)
    expect_true(is.null(d$experiment$spaces[[1]]))
    expect_equal(d$experiment$spaces[[2]]$get(),spaces[[2]]$get())

    tddInfo <- ud$ForagingDataDirs[[1]]
    d$experiment$addTrackingDataDirectory(spaces[[2]]$ID,
                                          tddInfo$AbsoluteFilePath)
    expect_error({
        d$experiment$deleteSpace(spaces[[2]]$ID)
    }, "Space{ID:2, Name:'foraging'} is not empty (contains:{'foraging.0000'})",fixed = TRUE)
})

test_that("it can manipulate tracking data directory",{
    d <- local_data()
    foragingID <- d$experiment$createSpace("foraging")$ID
    nestID <- d$experiment$createSpace("nest")$ID
    foragingTDDPath <- ud$ForagingDataDirs[[1]]$AbsoluteFilePath
    nestTDDPath <- ud$NestDataDirs[[1]]$AbsoluteFilePath
    badTDDPath <- file.path(ud$Basedir,"does-not-exist.9999")
    expect_error({
        d$experiment$addTrackingDataDirectory(42,foragingTDDPath)
    },'Unknown SpaceID 42')
    expect_error({
        d$experiment$addTrackingDataDirectory(foragingID,badTDDPath)
    },'.* is not a directory')
    URI <- d$experiment$addTrackingDataDirectory(foragingID,foragingTDDPath)
    expect_equal(URI,basename(foragingTDDPath))
    expect_error({
        d$experiment$addTrackingDataDirectory(foragingID,nestTDDPath)
    },'TDD.* and TDD.* overlaps in time')
    URI <- d$experiment$addTrackingDataDirectory(nestID,nestTDDPath)
    expect_equal(URI,basename(nestTDDPath))
    expect_error({
        d$experiment$removeTrackingDataDirectory(basename(badTDDPath))
    },"Unknown TDD{URI:'does-not-exist.9999'}",fixed=TRUE)
    d$experiment$removeTrackingDataDirectory(URI)
})


test_that("it can manipulate ant",{
    d <- local_data()
    a <- d$experiment$createAnt()
    expect_false(is.null(d$experiment$ants[[a$ID]]))
    expect_equal(d$experiment$ants[[a$ID]]$get(),a$get())

    expect_error({
        d$experiment$deleteAnt(42)
    },'Unknown AntID 42')
    d$experiment$addIdentification(a$ID,1,fmTimeSinceEver(),fmTimeForever())
    expect_error({
        d$experiment$deleteAnt(a$ID)
    },'Cannot remove Ant{ID:001}: it has some identifications left',fixed=TRUE)
    d$experiment$deleteIdentification(a$identifications[[1]])
    d$experiment$deleteAnt(a$ID)
    expect_length(d$experiment$ants,0)
})

test_that("it can manipulate identification", {
    d <- local_data()
    ants <- list(d$experiment$createAnt(),
                 d$experiment$createAnt())
    expect_error({
        d$experiment$addIdentification(42,0,fmTimeSinceEver(),fmTimeForever())
    },'Unknown AntID 42')
    d$experiment$addIdentification(ants[[1]]$ID,0,fmTimeSinceEver(),fmTimeForever())
    expect_error({
        d$experiment$addIdentification(ants[[1]]$ID,1,fmTimeSinceEver(),fmTimeForever())
    },'Identification.* and Identification.* overlaps')

    expect_error({
        d$experiment$addIdentification(ants[[2]]$ID,0,fmTimeSinceEver(),fmTimeForever())
    },'Identification.* and Identification.* overlaps')
    d$experiment$addIdentification(ants[[2]]$ID,1,fmTimeSinceEver(),fmTimeForever())
    e <- fmExperimentCreate('foo.myrmidon')
    a <- e$createAnt()
    i <- e$addIdentification(a$ID,0,fmTimeSinceEver(),fmTimeForever())

    expect_error({
        d$experiment$deleteIdentification(i)
    },'Identification.* is not managed by this Experiment')

    i <- ants[[1]]$identifications[[1]]
    i$end <- fmTimeCreate()
    expect_error({
        d$experiment$freeIdentificationRangeAt(ants[[1]]$identifications[[1]]$tagValue,
                                               fmTimeCreate(-1e-09))
    },'0x000 identifies an Ant at 1969-12-31T23:59:59.999999999Z')

    r <- d$experiment$freeIdentificationRangeAt(ants[[1]]$identifications[[1]]$tagValue,
                                                fmTimeCreate(0))
    expect_true(r$start == fmTimeCreate())
    expect_true(r$end$isForever())
    r <- d$experiment$freeIdentificationRangeAt(42,fmTimeCreate(0))
    expect_true(r$start$isSinceEver())
    expect_true(r$end$isForever())
    identifications <- d$experiment$identificationsAt(fmTimeCreate(-1e-09))
    expect_length(identifications,2)
    expect_equal(identifications[[ants[[1]]$ID]],0)
    expect_equal(identifications[[ants[[2]]$ID]],1)
    identifications <- d$experiment$identificationsAt(fmTimeCreate())
    expect_length(identifications,2)
    expect_equal(identifications[[ants[[1]]$ID]],NULL)
    expect_equal(identifications[[ants[[2]]$ID]],1)
})

test_that("it can manipulate its fields",{
    d <- local_data()
    expect_equal(d$experiment$name,'')
    d$experiment$name = 'foo'
    expect_equal(d$experiment$name,'foo')

    expect_equal(d$experiment$author,'')
    d$experiment$author = 'bar'
    expect_equal(d$experiment$author,'bar')

    expect_equal(d$experiment$comment,'')
    d$experiment$comment = 'baz'
    expect_equal(d$experiment$comment,'baz')

    expect_equal(d$experiment$family,fmTagFamily[['Undefined']])
    spaceID <- d$experiment$createSpace('foraging')$ID
    tddInfo <- ud$ForagingDataDirs[[1]]

    d$experiment$addTrackingDataDirectory(spaceID,
                                          tddInfo$AbsoluteFilePath)

    expect_equal(d$experiment$family,tddInfo$Family)
    expect_equal(names(which(fmTagFamily == d$experiment$family)),
                 'Tag36h11')

    expect_equal(d$experiment$defaultTagSize,1.0)
    d$experiment$defaultTagSize = 2.5
    expect_equal(d$experiment$defaultTagSize,2.5)
})


test_that("it can manipulate measurement types",{
    d <- local_data()
    mtID <- d$experiment$createMeasurementType("antennas")
    expect_length(d$experiment$measurementTypeNames,2)
    expect_equal(d$experiment$measurementTypeNames[[1]],"head-tail")
    expect_equal(d$experiment$measurementTypeNames[[mtID]],"antennas")
    expect_error({
        d$experiment$setMeasurementTypeName(42,'foo')
    },"Unknown MeasurementTypeID 42")
    d$experiment$setMeasurementTypeName(1,'foo')
    expect_error({
        d$experiment$deleteMeasurementType(42)
    },"Unknown MeasurementTypeID 42")
    expect_error({
        d$experiment$deleteMeasurementType(1)
    },"Could not remove default measurement type 'head-tail'")

    d$experiment$deleteMeasurementType(mtID)
    expect_length(d$experiment$measurementTypeNames,1)
})

test_that("it can manipulate shape type",{
   d <- local_data()
   bodyID <- d$experiment$createAntShapeType("body")
   headID <- d$experiment$createAntShapeType("head")
   expect_false(is.null(d$experiment$antShapeTypeNames[[bodyID]]))
   expect_false(is.null(d$experiment$antShapeTypeNames[[headID]]))
   expect_equal(d$experiment$antShapeTypeNames[[bodyID]], "body")
   expect_equal(d$experiment$antShapeTypeNames[[headID]], "head")

   expect_error({
       d$experiment$setAntShapeTypeName(42,'foo')
   },'Unknown AntShapeTypeID 42')
   d$experiment$setAntShapeTypeName(bodyID,'foo')

   expect_error({
       d$experiment$deleteAntShapeType(42)
   },'Unknown AntShapeTypeID 42')

})
