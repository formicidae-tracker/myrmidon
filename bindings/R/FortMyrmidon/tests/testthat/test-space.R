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
    expect_length(d$space$zones$objects,1)
    given <- d$space$zones$objects[[1]]
    expect_equal(z$get(),given$get())

    expect_error({
        d$space$deleteZone(42)
    },'42 is not managed')

    d$space$deleteZone(1)
    expect_length(d$space$zones$objects,0)
})

test_that("it can locate movie frame",{
})
