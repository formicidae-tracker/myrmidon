local_data <- function() {
    experiment <- fmExperimentCreate('space-utest.myrmidon')
    space <- experiment$createSpace('foo')
    return(list('experiment' = experiment, 'space'=space))
}

test_that("it can manipulate its fields",{
    d <- local_data()
    expect_equal(d$space$ID, 1)
    expect_equal(d$space$name,'foo')
    d$space$name = 'bar'
    expect_equal(d$space$name,'bar')
})

test_that("it can manipulate zones",{
    d <- local_data()
    z <- d$space$createZone('food')
    expect_length(d$space$zones,1)
    given <- d$space$zones[[1]]
    expect_equal(z$ID,1)
    expect_equal(given$ID,1)
    expect_equal(z$get(),given$get())

    expect_error({
        d$space$deleteZone(42)
    },'42 is not managed')

    d$space$deleteZone(1)
    expect_length(d$space$zones,0)
})

test_that("it can locate movie frame",{
    e <- fmExperimentOpen(ud$CurrentVersionFile$AbsoluteFilePath)
    tddInfo <- ud$WithVideoDataDir
    res <- e$spaces[[1]]$locateMovieFrame(tddInfo$Start)
    expect_equal(res$filepath,file.path(tddInfo$AbsoluteFilePath,
                                        'stream.0000.mp4'))
    expect_equal(res$frameID,0)
    expect_error(e$spaces[[1]]$locateMovieFrame(tddInfo$End),
                 'Could not find time .* in space nest-area')
})
