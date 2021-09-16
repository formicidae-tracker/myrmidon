test_that("can be created from seconds", {
    expect_true(fmTimeCreate(0.0) == fmTimeCreate())
})

test_that("has infinite support",{
    expect_true(fmTimeForever() == fmTimeCreate(Inf))
    expect_true(fmTimeSinceEver() == fmTimeCreate(-Inf))

    expect_equal(fmTimeForever()$asPOSIXct(),Inf)
    expect_equal(fmTimeSinceEver()$asPOSIXct(),-Inf)

    expect_true(fmTimeForever()$isInfinite())
    expect_true(fmTimeForever()$isForever())
    expect_false(fmTimeForever()$isSinceEver())

    expect_true(fmTimeSinceEver()$isInfinite())
    expect_false(fmTimeSinceEver()$isForever())
    expect_true(fmTimeSinceEver()$isSinceEver())

    expect_true(fmTimeSinceEver() < fmTimeCreate())
    expect_true(fmTimeForever() > fmTimeCreate())

    expect_true(fmTimeSinceEver() < fmTimeForever())
    expect_false(fmTimeSinceEver() > fmTimeForever())
    expect_true(fmTimeSinceEver() == fmTimeSinceEver())
    expect_false(fmTimeSinceEver() == fmTimeForever())

    expect_false(fmTimeForever() < fmTimeForever())
    expect_false(fmTimeForever() > fmTimeForever())
    expect_false(fmTimeForever() == fmTimeSinceEver())
    expect_true(fmTimeForever() == fmTimeForever())
})

test_that("has math support",{
    t <- fmTimeNow()$round(fmSecond(1))
    expect_true(t$add(fmNanosecond(1))$sub(t) == fmNanosecond(1))
    expect_true(t$add(fmSecond(1))$sub(t) == fmSecond(1))
    expect_false(t > t)
    expect_false(t$after(t))
    expect_false(t < t)
    expect_false(t$before(t))
    expect_true(t$add(fmNanosecond(1)) > t)
    expect_true(t$add(fmNanosecond(1))$after(t))
    expect_false(t > t$add(fmNanosecond(1)))
    expect_false(t$after(t$add(fmNanosecond(1))))

    expect_true(t == t)
    expect_true(t$equals(t))
})

test_that("converts to and from POSIXct",{
    nowR <- Sys.time()
    nowFM <- fmTimeNow()
    expect_true(nowFM$sub(fmTimeCreate(nowR)) < fmMillisecond(10))
    expect_true((nowFM$asPOSIXct() - nowR) < 1.0e-3)
})


test_that("operations on infinite time overflows",{
    expect_error(fmTimeForever()$add(fmNanosecond(1)))
    expect_error(fmTimeForever()$add(fmNanosecond(-1)))
    expect_error(fmTimeSinceEver()$add(fmNanosecond(1)))
    expect_error(fmTimeSinceEver()$add(fmNanosecond(-1)))
    expect_error(fmTimeSinceEver()$sub(fmTimeCreate()))
    expect_error(fmTimeForever()$sub(fmTimeCreate()))
})

test_that("rounding on infinite is a noop",{
    expect_true(fmTimeForever()$round(fmSecond(1)) == fmTimeForever())
    expect_true(fmTimeSinceEver()$round(fmSecond(1)) == fmTimeSinceEver())
})

test_that("can be rounded", {
    t <- fmTimeParse("2020-03-20T15:34:08.865123567Z")
    expect_true(t$round(fmNanosecond(10)) == fmTimeParse("2020-03-20T15:34:08.86512357Z"))
    expect_true(t$round(fmNanosecond(100)) == fmTimeParse("2020-03-20T15:34:08.8651236Z"))
    expect_true(t$round(fmMicrosecond(1)) == fmTimeParse("2020-03-20T15:34:08.865124Z"))
    expect_true(t$round(fmMicrosecond(10)) == fmTimeParse("2020-03-20T15:34:08.86512Z"))
    expect_true(t$round(fmMicrosecond(100)) == fmTimeParse("2020-03-20T15:34:08.8651Z"))
    expect_true(t$round(fmMillisecond(1)) == fmTimeParse("2020-03-20T15:34:08.865Z"))
    expect_true(t$round(fmMillisecond(10)) == fmTimeParse("2020-03-20T15:34:08.87Z"))
    expect_true(t$round(fmMillisecond(100)) == fmTimeParse("2020-03-20T15:34:08.9Z"))
    expect_true(t$round(fmSecond(1)) == fmTimeParse("2020-03-20T15:34:09Z"))
    expect_true(t$round(fmSecond(2)) == fmTimeParse("2020-03-20T15:34:08Z"))
    expect_true(t$round(fmMinute(1)) == fmTimeParse("2020-03-20T15:34:00Z"))
    expect_true(t$round(fmHour(1)) == fmTimeParse("2020-03-20T16:00:00Z"))
    expect_true(t$round(fmHour(24)) == fmTimeParse("2020-03-21T00:00:00Z"))
})
