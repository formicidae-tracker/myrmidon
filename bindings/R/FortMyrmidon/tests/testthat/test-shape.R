local_data <- function() {
    list("circle" = fmCircleCreate(),
         "capsule" = fmCapsuleCreate(),
         "polygon" = fmPolygonCreate(list(c(1,1),
                                          c(-1,1),
                                          c(-1,-1),
                                          c(1,-1))))
}


test_that("types are correct",{
    d <- local_data()
    expect_equal(d$circle$shapeType,fmShapeTypes[["CIRCLE"]])
    expect_equal(d$capsule$shapeType,fmShapeTypes[["CAPSULE"]])
    expect_equal(d$polygon$shapeType,fmShapeTypes[["POLYGON"]])
})

test_that("fmCircle can be formatted", {
    expect_output(print(fmCircleCreate()),"fmCircle($center = (0, 0), $radius = 1)",fixed=TRUE)
})

test_that("fmCircle fields are manipulable",{
    d <- local_data()
    expect_equal(d$circle$center,c(0,0))
    expect_equal(d$circle$radius,1.0)
    d$circle$center = c(1,2)
    d$circle$radius = 3.0
    expect_equal(d$circle$center,c(1,2))
    expect_equal(d$circle$radius,3.0)
})

test_that("fmCapsule fields are manipulable",{
    d <- local_data()
    expect_equal(d$capsule$c1,c(0,0))
    expect_equal(d$capsule$r1,1.0)
    expect_equal(d$capsule$c2,c(1,1))
    expect_equal(d$capsule$r2,1.0)
    d$capsule$c1 = c(-1,0)
    d$capsule$r1 = 1.0
    d$capsule$c2 = c(2,3)
    d$capsule$r2 = 4.0
    expect_equal(d$capsule$c1,c(-1,0))
    expect_equal(d$capsule$r1,1.0)
    expect_equal(d$capsule$c2,c(2,3))
    expect_equal(d$capsule$r2,4.0)
})

test_that("fmCapsule can be formatted", {
    expect_output(print(fmCapsuleCreate()),"fmCapsule($c1 = (0, 0), $c2 = (1, 1), $r1 = 1, $r2 = 1)",fixed=TRUE)
})


test_that("fmPolygon fields are manipulable",{
    d <- local_data()
    expect_length(d$polygon$vertices,4)
    expect_equal(d$polygon$vertices[[1]],c(1,1))
    expect_equal(d$polygon$vertices[[2]],c(-1,1))
    expect_equal(d$polygon$vertices[[3]],c(-1,-1))
    expect_equal(d$polygon$vertices[[4]],c(1,-1))
    d$polygon$vertices[[3]] = c(6,6)
    expect_equal(d$polygon$vertices[[3]],c(6,6))
})

test_that("fmPolygon can be formatted", {
    expect_output(print(fmPolygonCreate()),"fmPolygon((1, 1), (-1,  1), (-1, -1), ( 1, -1))",fixed=TRUE)
})


test_that("fmShapeList can be initialized",{
    l <- fmShapeListCreate(list(fmCircleCreate(),fmCapsuleCreate(),fmPolygonCreate()))
    expect_length(l,3)
})

test_that("fmShapeList can be formatted", {
    l <- fmShapeListCreate(list(fmCircleCreate(),fmCapsuleCreate(),fmPolygonCreate()))
    expect_output(print(l),"[[1]]
[1] fmCircle($center = (0, 0), $radius = 1)
[[2]]
[1] fmCapsule($c1 = (0, 0), $c2 = (1, 1), $r1 = 1, $r2 = 1)
[[3]]
[1] fmPolygon((1, 1), (-1,  1), (-1, -1), ( 1, -1))",fixed=TRUE)
})

test_that("fmShapeList can be implicitly converted",{
    expect_silent(pfmIWantAShapeList(list(fmCircleCreate())))

    a <- as.list(fmShapeListCreate(list(fmCircleCreate())))
    expect_equal(a[[1]]$center,c(0,0))
})

test_that("fmShapeList wraps mutable elements", {
    l <- fmShapeListCreate(list(fmCircleCreate(),fmCapsuleCreate(),fmPolygonCreate()))
    l[[1]]$center = c(1,2)
    l[[2]]$c2 = c(3,4)
    l[[3]]$vertices[[4]] = c(5,6)
    expect_output(print(l),"[[1]]
[1] fmCircle($center = (1, 2), $radius = 1)
[[2]]
[1] fmCapsule($c1 = (0, 0), $c2 = (3, 4), $r1 = 1, $r2 = 1)
[[3]]
[1] fmPolygon((1, 1), (-1,  1), (-1, -1), (5, 6))",fixed=TRUE)
})
