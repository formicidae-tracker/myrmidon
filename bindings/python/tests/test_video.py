import fort_myrmidon as m
import fort_myrmidon_utestdata as ud
import unittest

import cv2
import numpy.testing as npt

from . import assertions


class VideoTestCase(unittest.TestCase, assertions.CustomAssertion):
    def setUp(self):
        path = str(ud.UData().CurrentVersionFile.AbsoluteFilePath)
        self.experiment = m.Experiment.Open(path)

    def tearDown(self):
        self.experiment = None

    def test_end_to_end(self):
        expected = ud.UData().ExpectedResults[0]
        frames = ud.UData().ExpectedFrames
        segments = m.Query.FindVideoSegments(
            self.experiment, space=1, start=expected.Start, end=expected.End
        )
        m.VideoSegment.Match(segments, frames)
        m.VideoSegment.Match(segments, expected.Trajectories)
        m.VideoSegment.Match(segments, expected.Interactions)
        self.assertEqual(len(segments), len(expected.VideoSegments[1]))
        for s, e in zip(segments, expected.VideoSegments[1]):
            self.assertVideoSegmentEqual(s, e)

        expectedSegment = expected.VideoSegments[1][0]

        cap = cv2.VideoCapture(str(expectedSegment.AbsoluteFilePath))
        with m.VideoSequence(segments) as sequence:
            for i, (frame, data) in enumerate(sequence):
                ret, expectedFrame = cap.read()
                self.assertTrue(ret)
                self.assertVideoFrameDataEqual(data, expectedSegment.Data[i])

        self.assertEqual(cap.get(cv2.CAP_PROP_POS_FRAMES), expectedSegment.End - 1)

        segments = m.Query.FindVideoSegments(self.experiment, space=2)
        self.assertEqual(len(segments), 0)
        segments = m.Query.FindVideoSegments(self.experiment, space=3)
        self.assertEqual(len(segments), 0)

    def test_match_data_edge_cases(self):
        m.VideoSegment.Match([], [])
        with self.assertRaises(ValueError):
            m.VideoSegment.Match([m.VideoSegment(1), m.VideoSegment(2)], [])

    def test_video_edge_cases(self):
        expected = ud.UData().ExpectedResults[0]
        segments = expected.VideoSegments[1].deepcopy()
        segments[0].Data.pop(len(segments[0].Data) - 1)
        segments[0].Data.append(
            m.VideoFrameData(position=segments[0].End, time=m.Time.SinceEver())
        )
        segments[0].End += 2

        with m.VideoSequence(segments) as sequence:
            for _, data in sequence:
                fi = (d for d in segments[0].Data if d.Position == data.Position)
                try:
                    self.assertVideoFrameDataEqual(data, next(fi))
                except StopIteration:
                    e = m.VideoFrameData(
                        position=data.Position, time=m.Time.SinceEver()
                    )
                    self.assertVideoFrameDataEqual(data, e)
