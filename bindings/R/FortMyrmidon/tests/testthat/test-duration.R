context("duration")

test_that("arithmetic is nanosecond exact" ,{
    expect_true(fmSecond(1) + fmNanosecond(1) == fmNanosecond(1e9 + 1))
    expect_true(fmHour(1) + fmSecond(1) == fmNanosecond(3601 * 1e9))

    expect_true(fmSecond(1) - fmNanosecond(1) == fmNanosecond(1e9 - 1))
    expect_true(fmNanosecond(1) - fmSecond(1) == fmNanosecond(1 - 1e9))
    expect_true(fmSecond(1) - fmHour(1) == fmNanosecond(-3599 * 1e9))
})


test_that("it could be formatted properly" ,{
    expect_output((fmNanosecond(0))$show(),"0s")
    expect_output((fmHour(1))$show(),"1h0m0s")
    expect_output((fmMinute(1))$show(),"1m0s")
    expect_output((fmSecond(1))$show(),"1s")
    expect_output((fmMillisecond(34))$show(),"34ms")
    expect_output((fmNanosecond(30))$show(),"30ns")
    expect_output((fmSecond(1) + fmMicrosecond(100))$show(),"1.0001s")
})

test_that("creator functions provides the righ values",{
    expect_equal(fmHour(1)$nanoseconds(),3600 * 1e9)
    expect_equal(fmMinute(1)$nanoseconds(),60 * 1e9)
    expect_equal(fmSecond(1)$nanoseconds(),1e9)
    expect_equal(fmMillisecond(1)$nanoseconds(),1e6)
    expect_equal(fmMicrosecond(1)$nanoseconds(),1e3)
    expect_equal(fmNanosecond(1)$nanoseconds(),1)
})
