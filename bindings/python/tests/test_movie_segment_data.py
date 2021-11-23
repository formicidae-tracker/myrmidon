import py_fort_myrmidon as m
import py_fort_myrmidon_utestdata as ud
import unittest
import assertions


class MovieSegmentDataTestCase(unittest.TestCase, assertions.CustomAssertion):
    def setUp(self):
        path = str(ud.UData().CurrentVersionFile.AbsoluteFilePath)
        self.experiment = m.Experiment.Open(path)

    def tearDown(self):
        self.experiment = None

    def test_end_to_end(self):
        expected = ud.UData().ExpectedResults[0]
        frames = ud.UData().ExpectedFrames
        segments = m.Query.FindVideoSegments(self.experiment,
                                             space=1,
                                             start=expected.Start,
                                             end=expected.End)
        m.VideoSegment.Match(segments, frames)
        m.VideoSegment.Match(segments, expected.Trajectories)
        m.VideoSegment.Match(segments, expected.Interactions)
        self.assertEqual(len(segments), len(expected.VideoSegments[1]))
        for s, e in zip(segments, expected.VideoSegments[1]):
            self.assertVideoSegmentEqual(s, e)

        expectedData = expected.VideoSegments[1][0].Data
        # with m.MovieSegment.Open(segments) as video:
        #     for i, (frame, data) in enumerate(video):
        #         self.assertMovieFrameDataEqual(data, expectedData[i])

        segments = m.Query.FindVideoSegments(self.experiment, space=2)
        self.assertEqual(len(segments), 0)
        segments = m.Query.FindVideoSegments(self.experiment, space=3)
        self.assertEqual(len(segments), 0)

    def test_match_data_edge_cases(self):
        m.VideoSegment.Match([], [])
        with self.assertRaises(ValueError):
            m.VideoSegment.Match([m.VideoSegment(1),
                                  m.VideoSegment(2)], [])
