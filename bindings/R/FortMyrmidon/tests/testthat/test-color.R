test_that("has a default palette",{
    palette <- fmDefaultPalette()
    l <- length(palette)
    for(i in seq(1:2*l)) {
        expect_equal(fmDefaultPaletteColor(!!i),
                     palette[[!!(((i-1)%%l)+1)]])
    }

    data <- list(
        list("c" = fmDefaultPaletteColor(1),"e" = c("R" = 230, "G" = 159, "B" =   0)),
        list("c" = fmDefaultPaletteColor(2),"e" = c("R" =  86, "G" = 180, "B" = 233)),
        list("c" = fmDefaultPaletteColor(3),"e" = c("R" =   0, "G" = 158, "B" = 115)),
        list("c" = fmDefaultPaletteColor(4),"e" = c("R" = 240, "G" = 228, "B" =  66)),
        list("c" = fmDefaultPaletteColor(5),"e" = c("R" =   0, "G" = 114, "B" = 178)),
        list("c" = fmDefaultPaletteColor(6),"e" = c("R" = 213, "G" =  94, "B" =   0)),
        list("c" = fmDefaultPaletteColor(7),"e" = c("R" = 204, "G" = 121, "B" = 167)),
        list("c" = fmDefaultPaletteColor(8),"e" = c("R" = 230, "G" = 159, "B" =   0))
        )

    for ( d in data ) {
        expect_equal(d$c,d$e)
    }

})
