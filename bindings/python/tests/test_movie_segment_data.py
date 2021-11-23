import py_fort_myrmidon as m
import py_fort_myrmidon_utestdata as ud
import unittest
import assertions


class MovieSegmentDataTestCase(unittest.TestCase,assertions.CustomAssertion):
    def setUp(self):
        self.experiment = m.Experiment.Open(str(ud.UData().CurrentVersionFile.AbsoluteFilePath))

    def tearDown(self):
        self.experiment = None
        
    def test_end_to_end(self):
        expected = ud.UData().ExpectedResults[0]
        frames = ud.UData().ExpectedFrames
        segments = m.Query.FindMovieSegment(self.experiment,
                                            space = 1,
                                            start = expected.Start,
                                            end = expected.End)
        m.MovieSegmentData.MatchData(segments,frames)
        m.MovieSegmentData.MatchData(segments,expected.Trajectories)
        m.MovieSegmentData.MatchData(segments,expected.Interactions)
        self.assertEqual(len(segments),len(expected.MovieSegments[1]))
        for s,e in zip(segments,expected.MovieSegments[1]):
            self.assertMovieSegmentEqual(s,e)

        segments = m.Query.FindMovieSegment(self.experiment,
                                            space = 2)
        self.assertEqual(len(segments),0)
        segments = m.Query.FindMovieSegment(self.experiment,
                                            space = 3)
        self.assertEqual(len(segments),0)
        
    def test_match_data_edge_cases(self):
        m.MovieSegmentData.MatchData([],[])
        with self.assertRaises(ValueError):
            m.MovieSegmentData.MatchData([m.MovieSegmentData(1),m.MovieSegmentData(2)],[])
