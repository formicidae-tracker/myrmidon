test_that("can be initialized",{
    l <- fmVector2dListCreate()
    expect_length(l,0)
    l <- fmVector2dListCreate(list(c(1,1)))
    expect_length(l,1)

    expect_error(fmVector2dListCreate(list(1,2,3)), 'Converting to Eigen::Vector2d needs a N=numeric vector f size 2')
})


test_that("it can be implicitly converted",{
    expect_silent(fmpIWantAVector2dList(list(c(1,2))))

    a <- fmVector2dListCreate(list(c(1,2),c(3,4)))
    expected <- data.frame("x"=c(1,3),"y"=c(2,4))
    expect_equal(as.data.frame(a),expected)
    l <- fmVector2dListCreate(as.data.frame(a))
    expect_length(l,2)
    expect_equal(l[[1]],c(1,2))
    expect_equal(l[[2]],c(3,4))

})

test_that("it is subscribable",{
    l <- fmVector2dListCreate(list(c(1,2),c(3,4)))
    expect_length(l,2)
    expect_equal(l[[1]],c(1,2))
    expect_equal(l[[2]],c(3,4))
    l[[1]] <- c(0,0)
    expect_equal(l[[1]],c(0,0))
})
