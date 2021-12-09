local_data <- function() {
    return(list("experiment" = fmExperimentCreate('utest-ant.myrmidon')))
}

test_that("it have unique ID",{
    d <- local_data()
    for( i in seq(1:10) ) {
        a <- d$experiment$createAnt()
        expect_equal(a$ID,!!i)
    }
})

test_that("it has sorted identification", {
    d <- local_data()
    a <- d$experiment$createAnt()
    t1 <- fmTimeNow()
    t2 <- t1$add(fmSecond(1))
    idents <- list(
        d$experiment$addIdentification(a$ID,
                                       2,
                                       t2,
                                       fmTimeForever()),
        d$experiment$addIdentification(a$ID,
                                       1,
                                       t1,
                                       t2),
        d$experiment$addIdentification(a$ID,
                                       0,
                                       fmTimeSinceEver(),
                                       t1)
    )

    expect_length(a$identifications,3)
    expect_equal(a$identifications[[1]]$get(),idents[[3]]$get())
    expect_equal(a$identifications[[2]]$get(),idents[[2]]$get())
    expect_equal(a$identifications[[3]]$get(),idents[[1]]$get())

    d$experiment$deleteIdentification(idents[[3]])
    expect_length(a$identifications,2)
    expect_equal(a$identifications[[1]]$get(),idents[[2]]$get())
    expect_equal(a$identifications[[2]]$get(),idents[[1]]$get())

    expect_equal(a$identifiedAt(t2),2)
    expect_error({
        id <- a$identifiedAt(t1$add(fmNanosecond(-1)))
    }, 'Ant 001 is not identified at .*')

})

test_that("it has a display status",{
    d <- local_data()
    a <- d$experiment$createAnt()
    expect_equal(a$displayColor,fmDefaultPaletteColor(1))
    a$displayColor = c(255,255,255)
    expect_equal(a$displayColor,c('R' = 255,'G' = 255,'B' = 255))

    expect_equal(a$displayStatus,fmAnt.DisplayState[['VISIBLE']])
    a$displayStatus <- fmAnt.DisplayState[['HIDDEN']]
    expect_equal(a$displayStatus,fmAnt.DisplayState[['HIDDEN']])
})

test_that("it has values",{
    d <- local_data()
    a <- d$experiment$createAnt()
    t <- fmTimeNow()
    expectedTimes <- numeric()
    expectedTimes[[1]] = NA
    attr(expectedTimes,'class') = 'POSIXct'
    
    d$experiment$setMetaDataKey("alive",TRUE)
    values <- a$getValues("alive")
    expect_equal(values,data.frame("times" = expectedTimes,"values" = c(TRUE)));

    expect_error({
        a$getValue("isDead",t)
    },'Invalid key')
    expect_error({
        a$setValue("isDead",TRUE, t)
    },"Unknown meta data key 'isDead'")
    expect_error({
        a$setValue("alive",42L, t)
    },'Value is not of the right type')
    expect_error({
        a$setValue("alive",FALSE, fmTimeForever())
    },'Time cannot be +∞', fixed=TRUE)

    a$setValue('alive',FALSE, t)
    expectedTimes[[2]] = t$asPOSIXct()
    expect_equal(a$getValues("alive"),
                 data.frame("times" = expectedTimes,
                            "values" = c(TRUE,FALSE)))
    
    expect_error({a$getValues("isDead")},"Invalid key 'isDead'")
    
    expect_equal(a$getValue('alive',fmTimeSinceEver()),TRUE)
    expect_equal(a$getValue('alive',t$add(fmNanosecond(-1))),TRUE)
    expect_equal(a$getValue('alive',t),FALSE)
    expect_equal(a$getValue('alive',fmTimeForever()),FALSE)

    expect_error({
        a$deleteValue("isDead",t)
    },"No stored values for 'isDead'")

    expect_error({
        a$deleteValue("alive",t$add(fmSecond(1)))
    },"No stored values for 'alive' at requested time '.*'")

    a$deleteValue("alive",t)
    expect_equal(a$getValue('alive',t),TRUE)
    expect_equal(a$getValue('alive',fmTimeForever()),TRUE)

    d$experiment$setMetaDataKey("alive",FALSE)
    expect_equal(a$getValue('alive',fmTimeSinceEver()),FALSE)
    expect_equal(a$getValue('alive',t$add(fmNanosecond(-1))),FALSE)
    expect_equal(a$getValue('alive',t),FALSE)
    expect_equal(a$getValue('alive',fmTimeForever()),FALSE)

})

test_that("it has a virtual shape",{
    d <- local_data()
    d$experiment$createAntShapeType("body")
    d$experiment$createAntShapeType("antenna")
    a <- d$experiment$createAnt()
    expect_length(a$capsules,0)
    c1 <- fmCapsuleCreate()
    c2 <- fmCapsuleCreate(c2 = c(-1,-1))
    expect_error({
        a$addCapsule(42,c1)
    },'Unknown AntShapeTypeID 42')
    a$addCapsule(1,c1)
    a$addCapsule(1,c2)
    a$addCapsule(2,c1)
    expect_length(a$capsules,3)
    expect_capsule_equal(a$capsules[[1]]$capsule,c1)
    expect_capsule_equal(a$capsules[[2]]$capsule,c2)
    expect_capsule_equal(a$capsules[[3]]$capsule,c1)

    expect_error({
        a$deleteCapsule(42)
    },'Index 41 is out of range [0;3[',fixed=TRUE)
    a$deleteCapsule(2)
    expect_length(a$capsules,2)
    expect_capsule_equal(a$capsules[[1]]$capsule,c1)
    expect_capsule_equal(a$capsules[[2]]$capsule,c1)

    a$clearCapsules()
    expect_length(a$capsules,0)

})
