test_that("arithmetic is nanosecond exact" ,{
    expect_true(fmSecond(1) + fmNanosecond(1) == fmNanosecond(1e9 + 1))
    expect_true(fmHour(1) + fmSecond(1) == fmNanosecond(3601 * 1e9))

    expect_true(fmSecond(1) - fmNanosecond(1) == fmNanosecond(1e9 - 1))
    expect_true(fmNanosecond(1) - fmSecond(1) == fmNanosecond(1 - 1e9))
    expect_true(fmSecond(1) - fmHour(1) == fmNanosecond(-3599 * 1e9))
})


test_that("it could be formatted properly" ,{
    expect_output((fmNanosecond(0))$show(),"0s")
    expect_output((fmNanosecond(1))$show(),"1ns")
    expect_output((fmMicrosecond(1.1))$show(),"1.1µs")
    expect_output((fmMillisecond(2.2))$show(),"2.2ms")
    expect_output((fmSecond(3.3))$show(),"3.3s")
    expect_output((fmSecond(-4.4))$show(),"-4.4s")
    expect_output((fmMinute(4) + fmSecond(5))$show(),"4m5s")
    expect_output((fmMinute(4) + fmMillisecond(5001))$show(),"4m5.001s")
    expect_output((fmHour(5) + fmMinute(6) + fmMillisecond(7001))$show(),"5h6m7.001s")
})

test_that("creator functions provides the righ values",{
    expect_equal(fmHour(1)$nanoseconds(),3600 * 1e9)
    expect_equal(fmMinute(1)$nanoseconds(),60 * 1e9)
    expect_equal(fmSecond(1)$nanoseconds(),1e9)
    expect_equal(fmMillisecond(1)$nanoseconds(),1e6)
    expect_equal(fmMicrosecond(1)$nanoseconds(),1e3)
    expect_equal(fmNanosecond(1)$nanoseconds(),1)
})

test_that("it can be casted to the right units",{
    d <- fmHour(1)
    expect_equal(d$hours(),1.0)
    expect_equal(d$minutes(),60.0)
    expect_equal(d$seconds(),3600.0)
    expect_equal(d$milliseconds(),3.6e6)
    expect_equal(d$microseconds(),3.6e9)
    expect_equal(d$nanoseconds(),36e11)
})

test_that("it can be parsed",{
    data <-list()
    data["0"] <-fmNanosecond(0)$nanoseconds()
    data["5s"] <- fmSecond(5)$nanoseconds()
    data["30s"] <- fmSecond(30)$nanoseconds()
    data["1478s"] <- fmSecond(1478)$nanoseconds()
    data["-5s"] <- fmSecond(-5)$nanoseconds()
    data["+5s"] <- fmSecond(5)$nanoseconds()
    data["-0"] <- fmNanosecond(0)$nanoseconds()
    data["+0"] <- fmNanosecond(0)$nanoseconds()

    data["5.0s"] <- fmSecond(5)$nanoseconds()
    data["5.6s"] <- fmSecond(5.6)$nanoseconds()
    data["5.s"] <- fmSecond(5)$nanoseconds()
    data[".5s"] <- fmMillisecond(500)$nanoseconds()
    data["1.0s"] <- fmSecond(1)$nanoseconds()
    data["1.00s"] <- fmSecond(1)$nanoseconds()
    data["1.004s"] <- fmMillisecond(1004)$nanoseconds()
    data["1.0040s"] <- fmMillisecond(1004)$nanoseconds()
    data["100.00100s"] <- (fmSecond(100) + fmMillisecond(1))$nanoseconds()
    data["10ns"] <- fmNanosecond(10)$nanoseconds()
    data["11us"] <- fmMicrosecond(11)$nanoseconds()
    data["12µs"] <- fmMicrosecond(12)$nanoseconds()
    data["12μs"] <- fmMicrosecond(12)$nanoseconds()
    data["13ms"] <- fmMillisecond(13)$nanoseconds()
    data["14s"] <- fmSecond(14)$nanoseconds()
    data["15m"] <- fmMinute(15)$nanoseconds()
    data["16h"] <- fmHour(16)$nanoseconds()
    data["3h30m"] <- (fmHour(3) + fmMinute(30))$nanoseconds()
    data["10.5s4m"] <- (fmMinute(4) + fmSecond(10) + fmMillisecond(500))$nanoseconds()
    data["-2m3.4s"] <- (fmMinute(-2) + fmSecond(-3) + fmMillisecond(-400))$nanoseconds()
    data["1h2m3s4ms5us6ns"] <- (fmHour(1) + fmMinute(2) + fmSecond(3) + fmMillisecond(4) + fmMicrosecond(5) + fmNanosecond(6))$nanoseconds()
    data["39h9m14.425s"] <- (fmHour(39) + fmMinute(9) + fmSecond(14) + fmMillisecond(425))$nanoseconds()
    data["52763797000ns"] <- fmNanosecond(52763797000)$nanoseconds()
    data["0.3333333333333333333h"] <- fmMinute(20)$nanoseconds()


    for( i in 1:length(data)) {
        v <- names(data)[[i]]
        expected <- fmNanosecond(data[[i]])
        expect_true(fmDurationParse(!!v) == expected)
    }

    errors <- list("",
                   "3",
                   "-",
                   "s",
                   ".",
                   "-.",
                   ".s",
                   "+.s",
                   "3000000h",
                   "9223372036854775808ns",
                   "9223372036854775.808us",
                   "9223372036854ms775us808ns",
                   "-9223372036854775808ns")
    for ( e in errors) {
        expect_error(fmDurationParse(!!e),"Could not parse '.*':.*")
    }
})

test_that("binary operation works as expected", {
    a <- fmNanosecond(1)
    b <- fmNanosecond(2)
    expect_true(a == a)
    expect_false(a == b)

    expect_false(a != a)
    expect_true(a != b)

    expect_false(a < a)
    expect_true(a < b)

    expect_true(a <= a)
    expect_true(a <= b)

    expect_false(a > a)
    expect_false(a > b)

    expect_true(a >= a)
    expect_false(a >= b)

})
