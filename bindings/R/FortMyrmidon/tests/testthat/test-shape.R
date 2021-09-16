## local_data <- function() {
##     list("circle" = fmCircleCreate(),
##          "capsule" = fmCapsuleCreate(),
##          "polygon" = fmPolygonCreate(list(c(1,1),
##                                           c(-1,1),
##                                           c(-1,-1),
##                                           c(1,-1))))
## }


## test_that("types are correct",{
##     d <- local_data()
##     expect_equal(d$circle$shapeType,fmShapeTypes["CIRCLE"])
##     expect_equal(d$capsule$shapeType,fmShapeTypes["CAPSULE"])
##     expect_equal(d$polygon$shapeType,fmShapeTypes["POLYGON"])
## })

## test_that("fmCircle fields are manipulable",{
##     d <- local_data()
##     expect_equal(d$circle$center,c(0,0))
##     expect_equal(d$circle$radius,1.0)
##     d$circle$center = c(1,2)
##     d$circle$radius = 3.0
##     expect_equal(d$circle$center,c(1,2))
##     expect_equal(d$circle$radius,3.0)
## })

## test_that("fmCapsule fields are manipulable",{
##     d <- local_data()
##     expect_equal(d$capsule$c1,c(0,0))
##     expect_equal(d$capsule$r1,1.0)
##     expect_equal(d$capsule$c2,c(1,1))
##     expect_equal(d$capsule$r2,1.0)
##     d$capsule$c1 = c(-1,0)
##     d$capsule$r1 = 1.0
##     d$capsule$c2 = c(2,3)
##     d$capsule$r2 = 4.0
##     expect_equal(d$capsule$c1,c(-1,0))
##     expect_equal(d$capsule$r1,1.0)
##     expect_equal(d$capsule$c2,c(2,3))
##     expect_equal(d$capsule$r2,4.0)
## })

## test_that("fmPolygon fields are manipulable",{
##     d <- local_data()
##     expect_length(d$polygon$vertices,4)
##     expect_equal(d$polygon$vertices[[1]],c(1,1))
##     expect_equal(d$polygon$vertices[[2]],c(-1,1))
##     expect_equal(d$polygon$vertices[[3]],c(-1,-1))
##     expect_equal(d$polygon$vertices[[4]],c(1,-1))
##     d$polygon$vertices = list(c(0,0),c(1,1),c(-1,1))
## })
