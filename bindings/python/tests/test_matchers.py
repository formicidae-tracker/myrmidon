import unittest
import fort_myrmidon as m


class MatchersTestCase(unittest.TestCase):
    def test_right_matcher(self):
        testdata = [
            (
                m.Matcher.And(m.Matcher.AntID(1), m.Matcher.AntID(2)),
                "( Ant.ID == 001 && Ant.ID == 002 )",
            ),
            (
                m.Matcher.Or(m.Matcher.AntID(1), m.Matcher.AntID(2)),
                "( Ant.ID == 001 || Ant.ID == 002 )",
            ),
            (m.Matcher.AntID(1), "Ant.ID == 001"),
            (m.Matcher.AntMetaData(key="group", value="nurse"), "Ant.'group' == nurse"),
            (m.Matcher.AntDistanceSmallerThan(10.0), "Distance(Ant1, Ant2) < 10"),
            (m.Matcher.AntDistanceGreaterThan(10.0), "Distance(Ant1, Ant2) > 10"),
            (m.Matcher.AntAngleSmallerThan(1.0), "Angle(Ant1, Ant2) < 1"),
            (m.Matcher.AntAngleGreaterThan(1.0), "Angle(Ant1, Ant2) > 1"),
            (m.Matcher.InteractionType(1, 1), "InteractionType(1 - 1)"),
            (m.Matcher.InteractionType(2, 1), "InteractionType(1 - 2)"),
            (
                m.Matcher.AntDisplacement(10.0, 2),
                "AntDisplacement(under: 10, minimumGap: 2ns)",
            ),
        ]
        for matcher, e in testdata:
            self.assertEqual(str(matcher), e)
