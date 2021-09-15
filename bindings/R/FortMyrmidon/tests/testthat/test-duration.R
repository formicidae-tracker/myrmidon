context("duration")

test_that("arithmetic is nanosecond exact" ,{
    expect_true(fmSecond(1) + fmNanosecond(1) == fmNanosecond(1e9 + 1))
    expect_true(fmHour(1) + fmSecond(1) == fmNanosecond(3601 * 1e9))

    expect_true(fmSecond(1) - fmNanosecond(1) == fmNanosecond(1e9 - 1))
    expect_true(fmNanosecond(1) - fmSecond(1) == fmNanosecond(1 - 1e9))
    expect_true(fmSecond(1) - fmHour(1) == fmNanosecond(-3599 * 1e9))
})


test_that("it could be formatted properly" ,{
    expect_output(str(fmNanosecond(0)),"0s")
    expect_output(str(fmHour(1)),"1h0m0s")
    expect_output(str(fmMinute(1)),"1m0s")
    expect_output(str(fmSecond(1)),"1s")
    expect_output(str(fmMillisecond(34)),"34ms")
    expect_output(str(fmSecond(1) + fmMicrosecond(100)),"1.0001s")



})
