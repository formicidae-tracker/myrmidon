test_that("it is casted to the right type", {
    expect_equal(pfmValueType(FALSE),0)
    expect_equal(pfmValueType(0L),1)
    expect_equal(pfmValueType(0.0),2)
    expect_equal(pfmValueType('foo'),3)
    expect_equal(pfmValueType(fmTimeCreate()),4)

    expect_equal(pfmBool(),FALSE)
    expect_equal(pfmInt(),0L)
    expect_equal(pfmDouble(),0.0)
    expect_equal(pfmString(),'')
    expect_true(pfmTime() == fmTimeCreate())
})
