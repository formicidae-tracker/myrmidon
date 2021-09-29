local_data <- function() {
    experiment = fmExperimentCreate("identification-utest.myrmidon")
    ant = experiment$createAnt()
    i = experiment$addIdentification(ant$ID,123,fmTimeSinceEver(),fmTimeForever())
    return(list("experiment" = experiment,
                "ant" = ant,
                "i" = i))
}

test_that("it has target values",{
    d <- local_data()
    expect_equal(d$i$tagValue,123)
    expect_equal(d$i$targetAntID,d$ant$ID)
})

test_that("it can manipulate boundaries time", {
    experiment <- fmExperimentCreate("identification-utest.myrmidon")
    ant <- experiment$createAnt()
    idents = list(
        experiment$addIdentification(ant$ID,
                                     123,
                                     fmTimeSinceEver(),
                                     fmTimeCreate()),
        experiment$addIdentification(ant$ID,
                                     123,
                                     fmTimeCreate(),
                                     fmTimeForever())
    )
    expect_length(ant$identifications,2)
    expect_true(idents[[1]]$start == fmTimeSinceEver())
    expect_true(idents[[1]]$end == fmTimeCreate())
    expect_true(idents[[2]]$start == fmTimeCreate())
    expect_true(idents[[2]]$end == fmTimeForever())
    expect_error({
        idents[[1]]$end <- fmTimeCreate(1e-09)
    },'Identification.* and Identification.* overlaps')
    expect_error({
        idents[[2]]$start <- fmTimeCreate(-1e-09)
    },'Identification.* and Identification.* overlaps')

    idents[[1]]$end <- fmTimeCreate(-1e-09)
    idents[[2]]$start <- fmTimeCreate(1e-09)

    expect_true(idents[[1]]$end == fmTimeCreate(-1e-09))
    expect_true(idents[[2]]$start == fmTimeCreate(1e-09))

})

test_that("it can manipulate tag size", {
    d <- local_data()
    expect_equal(d$i$hasDefaultTagSize(), TRUE)
    expect_equal(d$i$tagSize,0)
    d$i$tagSize = 2.4
    expect_equal(d$i$hasDefaultTagSize(), FALSE)
    expect_equal(d$i$tagSize,2.4)
})

test_that("it can manipulate Ant Poses", {
    d <- local_data()
    expect_equal(d$i$hasUserDefinedAntPose(), FALSE)
    expect_equal(d$i$antPosition,c(0,0))
    expect_equal(d$i$antAngle,0)
    position <- c(1,2)
    angle <- 3
    d$i$setUserDefinedAntPose(position,angle)
    expect_equal(d$i$hasUserDefinedAntPose(), TRUE)
    expect_equal(d$i$antPosition,position)
    expect_equal(d$i$antAngle,angle)

    d$i$clearUserDefinedAntPose()
    expect_equal(d$i$hasUserDefinedAntPose(), FALSE)
    expect_equal(d$i$antPosition,c(0,0))
    expect_equal(d$i$antAngle,0)
})

test_that("it can format itself", {
    d <- local_data()
    expect_output(print(d$i),'fmIdentification( $tagValue = 0x07b , $targetAntID = 001 , $start = -∞ , $end = +∞ )',fixed = TRUE)
})
