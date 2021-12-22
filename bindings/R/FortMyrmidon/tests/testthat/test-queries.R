local_data <- function () {
    experiment <- fmExperimentOpen(ud$CurrentVersionFile$AbsoluteFilePath)
    return(list("experiment"= experiment))
}

test_that("it can query tag statistics",{
    d <- local_data()
    expect_equal(fmQueryComputeTagStatistics(d$experiment),ud$ExpectedTagStatistics)
})


test_that("it can identify frames",{
    d <- local_data()
    expected <- ud$ExpectedFrames
    expected <- expected[names(expected) != "collisions"]
    identified <- fmQueryIdentifyFrames(d$experiment,showProgress = FALSE)
    expect_equal(identified,expected)
    t <- ud$NestDataDirs[[1]]$End
    expectedNumber <- sum(expected$frames$time <= t$asPOSIXct())
    identified <- fmQueryIdentifyFrames(d$experiment,
                                        end = t,
                                        showProgress = FALSE)
    expect_length(identified$positions,expectedNumber)
    truncated <- list("frames" = head(expected$frames,expectedNumber),
                      "positions" = head(expected$positions,expectedNumber))
    expect_equal(identified,truncated)
})

test_that("it can collide frames",{
    d <- local_data()
    expected <- ud$ExpectedFrames
    expect_equal(fmQueryCollideFrames(d$experiment,showProgress = FALSE),expected)
})

test_that("it can compute ant trajectories",{
    d <- local_data()
    for(expectedResult in ud$ExpectedResults) {
        result <- fmQueryComputeAntTrajectories(d$experiment,
                                                start = expectedResult$Start,
                                                end = expectedResult$End,
                                                maximumGap = expectedResult$MaximumGap,
                                                matcher = expectedResult$Matches,
                                                showProgress = FALSE)
        expect_equal(result$trajectories_summary,expectedResult$trajectories_summary)
        expect_equal(result$trajectories,expectedResult$trajectories)
    }

})

test_that("it can compute ant interactions",{
    d <- local_data()
    for(expectedResult in ud$ExpectedResults) {
        result <- fmQueryComputeAntInteractions(d$experiment,
                                                start = expectedResult$Start,
                                                end = expectedResult$End,
                                                maximumGap = expectedResult$MaximumGap,
                                                reportFullTrajectories = TRUE,
                                                matcher = expectedResult$Matches,
                                                showProgress = FALSE)
        expect_equal(result$trajectories_summary,expectedResult$interactions_summary)
        expect_equal(result$trajectories,expectedResult$interactions_trajectories)
        expect_equal(result$interactions,expectedResult$interactions)

        result <- fmQueryComputeAntInteractions(d$experiment,
                                                start = expectedResult$Start,
                                                end = expectedResult$End,
                                                maximumGap = expectedResult$MaximumGap,
                                                reportFullTrajectories = FALSE,
                                                matcher = expectedResult$Matches,
                                                showProgress = FALSE)
        expect_equal(result$trajectories_summary,NULL)
        expect_equal(result$trajectories,NULL)
        expect_equal(result,expectedResult$summarized_interactions)
    }
})

test_that("it can get metadata key ranges",{
    experiment <- fmExperimentCreate("foo.myrmidon")
    experiment$setMetaDataKey("alive",TRUE)
    experiment$createAnt()
    a2 <- experiment$createAnt()
    a2$setValue("alive",FALSE,fmTimeCreate())
    a3 <- experiment$createAnt()
    a3$setValue("alive",FALSE,fmTimeSinceEver())
    a3$setValue("alive",TRUE,fmTimeCreate())
    a3$setValue("alive",TRUE,fmTimeCreate(1e-09))
    a3$setValue("alive",FALSE,fmTimeCreate(2e-09))
    a3$setValue("alive",TRUE,fmTimeCreate(3e-09))

    expectedStartTime = c(NA,NA,0.0,3.0e-09)
    attr(expectedStartTime,"class") = "POSIXct"
    expectedEndTime = c(NA,0.0,2.0e-09,NA)
    attr(expectedEndTime,"class") = "POSIXct"

    expect_equal(fmQueryGetMetaDataKeyRanges(experiment,"alive",TRUE),
                 data.frame("antID" = c(1,2,3,3),
                            "start" = expectedStartTime,
                            "end" = expectedEndTime))

    expect_error({
        fmQueryGetMetaDataKeyRanges(experiment,"isDead",TRUE)
    }, "Invalid key 'isDead'")

    expect_error({
        fmQueryGetMetaDataKeyRanges(experiment,"alive","alive")
    }, 'Value is not of the right type')
})


test_that("it can get tag close-ups",{
    d <- local_data()
    res <- fmQueryGetTagCloseUps(d$experiment)
    expect_equal(names(res),c("path","ID","X","Y","Theta","c0_X","c0_Y","c1_X","c1_Y","c2_X","c2_Y","c3_X","c3_Y"))
})
