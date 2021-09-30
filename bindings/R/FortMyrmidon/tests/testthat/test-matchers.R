test_that("right matchers are mapped" ,{
    data <- list(
        list("matcher" = fmMatcherAnd(list(fmMatcherAntID(1),fmMatcherAntID(2))),
             "expected" = "( Ant.ID == 001 && Ant.ID == 002 )" ),
        list("matcher" = fmMatcherOr(list(fmMatcherAntID(1),fmMatcherAntID(2))),
             "expected"= "( Ant.ID == 001 || Ant.ID == 002 )"),
        list("matcher" = fmMatcherAntID(1),"expected" = "Ant.ID == 001" ),
        list("matcher" = fmMatcherAntMetaData("group","nurse"),"expected" = "Ant.'group' == nurse" ),
        list("matcher" = fmMatcherAntDistanceSmallerThan(10.0),"expected" = "Distance(Ant1, Ant2) < 10" ),
        list("matcher" = fmMatcherAntDistanceGreaterThan(10.0),"expected" = "Distance(Ant1, Ant2) > 10" ),
        list("matcher" = fmMatcherAntAngleSmallerThan(1.0),"expected" = "Angle(Ant1, Ant2) < 1" ),
        list("matcher" = fmMatcherAntAngleGreaterThan(1.0),"expected" = "Angle(Ant1, Ant2) > 1" ),
        list("matcher" = fmMatcherInteractionType(1,1),"expected" = "InteractionType(1 - 1)" ),
        list("matcher" = fmMatcherInteractionType(2,1),"expected" = "InteractionType(1 - 2)" ),
        list("matcher" = fmMatcherAntDisplacement(10.0,fmNanosecond(2)),"expected" = "AntDisplacement(under: 10, minimumGap: 2ns)" )
    )
    for ( d in data ) {
        expect_equal(d$matcher$format(),d$expected)
    }
})
