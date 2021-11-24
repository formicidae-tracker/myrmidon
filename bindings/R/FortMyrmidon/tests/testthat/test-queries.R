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
