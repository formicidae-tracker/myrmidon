local_zone_test_case <- function() {
    experiment <- fmExperimentCreate("zone-utest.myrmidon")
    space <- experiment$createSpace("nest")
    return(list("experiment" = experiment,"space" = space))
}

test_that("can manipulate its field", {
    d <- local_zone_test_case()
    zone <- d$space$createZone("exit")
    expect_equal(zone$name,"exit")
    zone$name = "food"
    expect_equal(zone$name,"food")
})

test_that("can manipulate its fmZoneDefinition", {
    d <- local_zone_test_case()
    zone <- d$space$createZone("exit")
    definitions <- list(zone$addDefinition(list(),fmTimeCreate(),fmTimeForever()),
                        zone$addDefinition(list(),fmTimeSinceEver(),fmTimeCreate()))
    expect_error(zone$addDefinition(list(),
                                    fmTimeCreate(-1e-9),
                                    fmTimeCreate(1e-9)),
                 "Zone definition would overlap")

    expect_length(zone$definitions,2)
    expect_equal(zone$definitions[[1]]$get(),definitions[[2]]$get())
    expect_equal(zone$definitions[[2]]$get(),definitions[[1]]$get())

    expect_error(zone$deleteDefinition(42))
    zone$deleteDefinition(1)
    expect_equal(zone$definitions[[1]]$get(),definitions[[1]]$get())
})

test_that("fmZone$ID is experiment unique", {
    d <- local_zone_test_case()
    space <- d$experiment$createSpace("foraging")
    zones = list(
        d$space$createZone("zone"),
        space$createZone("zone"),
        d$space$createZone("zone")
    )
    i <- 1
    for ( z in zones) {
        expect_equal(z$ID,i)
        i <- i + 1
    }
})


test_that("fmZoneDefinition have shapes", {
    d <- local_zone_test_case()
    zone = d$space$createZone("food")
    d <- zone$addDefinition(list(),fmTimeSinceEver(),fmTimeForever())
    expect_length(d$shapes,0)
    d$shapes = list(fmCircleCreate(),fmCapsuleCreate())
    expect_length(d$shapes,2)
    expect_equal(d$shapes[[1]]$shapeType,fmShapeTypes[['CIRCLE']])
    expect_equal(d$shapes[[2]]$shapeType,fmShapeTypes[['CAPSULE']])

})



test_that("fmZoneDefinition have time validity", {
    d <- local_zone_test_case()
    zone = d$space$createZone("food")
    expect_error(zone$addDefinition(list(),fmTimeForever(),fmTimeSinceEver()),
                 "Invalid time range [+∞, -∞[",fixed=TRUE)
    definitions <- list (
        zone$addDefinition(list(),fmTimeSinceEver(),fmTimeCreate()),
        zone$addDefinition(list(),fmTimeCreate(),fmTimeForever())
        )
    expect_error({
        definitions[[1]]$end = fmTimeCreate(1e-09)
    },"Zone definition would overlaps")
    expect_error({
        definitions[[2]]$start = fmTimeCreate(-1e-09)
    },"Zone definition would overlaps")

    definitions[[1]]$end <- fmTimeCreate(-1e-09)
    definitions[[2]]$start <- fmTimeCreate(1e-09)
    zone$addDefinition(list(),
                       fmTimeCreate(-1e-09),
                       fmTimeCreate(1e-09))

})
