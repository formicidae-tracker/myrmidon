test_that("it is casted to the right type", {
    expect_equal(pfmAntStaticValueType(FALSE),0)
    expect_equal(pfmAntStaticValueType(0L),1)
    expect_equal(pfmAntStaticValueType(0.0),2)
    expect_equal(pfmAntStaticValueType('foo'),3)
    expect_equal(pfmAntStaticValueType(fmTimeCreate()),4)

    expect_equal(pfmAntStaticBool(),FALSE)
    expect_equal(pfmAntStaticInt(),0L)
    expect_equal(pfmAntStaticDouble(),0.0)
    expect_equal(pfmAntStaticString(),'')
    expect_true(pfmAntStaticTime() == fmTimeCreate())
})
