import unittest
import py_fort_myrmidon as m
import py_fort_myrmidon_utestdata as ud
import assertions
import functools
import time

class QueryTestCase(unittest.TestCase,assertions.CustomAssertion):
    def setUp(self):
        self.experiment = m.Experiment.Open(str(ud.UData().CurrentVersionFile.AbsoluteFilePath))

    def tearDown(self):
        self.experiment = None


    def test_tag_statistics(self):
        tagStats = m.Query.ComputeTagStatistics(self.experiment)
        self.assertTagStatisticsEqual(tagStats,ud.UData().ExpectedTagStatistics)

    def test_identify_frames(self):
        identifieds = m.Query.IdentifyFrames(self.experiment)
        expected = ud.UData().ExpectedFrames
        self.assertEqual(len(identifieds),len(expected))
        for i,e in enumerate(expected):
            self.assertIdentifiedFrameEqual(identifieds[i],e[0])
        t = ud.UData().NestDataDirs[0].End
        expectedNumber = 0
        for identified,collision in expected:
            if identified.FrameTime > t:
                break
            else:
                expectedNumber +=1
        identifieds = m.Query.IdentifyFrames(self.experiment, end = t)
        self.assertEqual(len(identifieds),expectedNumber)
        for i,e in enumerate(expected[:expectedNumber]):
            self.assertIdentifiedFrameEqual(identifieds[i],e[0])


    def test_collide_frames(self):
        expected = ud.UData().ExpectedFrames
        results = m.Query.CollideFrames(self.experiment)
        self.assertEqual(len(results),len(expected))

        for idx,e in enumerate(expected):
            self.assertIdentifiedFrameEqual(results[idx][0],e[0])
            self.assertCollisionFrameEqual(results[idx][1],e[1])

    def compare_trajectories(a,b) -> bool:
        if a.End() == b.End():
            return a.Space < b.Space
        return a.End() < b.End()


    def test_compute_ant_trajectories(self):
        for expectedResult in ud.UData().ExpectedResults:
            trajectories = m.Query.ComputeAntTrajectories(self.experiment,
                                                          start = expectedResult.Start,
                                                          end = expectedResult.End,
                                                          maximumGap = expectedResult.MaximumGap,
                                                          matcher = expectedResult.Matches)
            self.assertEqual(len(trajectories),len(expectedResult.Trajectories))

            trajectories = sorted(trajectories,
                                 key = functools.cmp_to_key(QueryTestCase.compare_trajectories))
            for i,expected in enumerate(expectedResult.Trajectories):
                self.assertAntTrajectoryEqual(trajectories[i],expected)

    def test_compute_ant_interactions(self):
        for expectedResult in ud.UData().ExpectedResults:
            trajectories,interactions = m.Query.ComputeAntInteractions(self.experiment,
                                                                       start = expectedResult.Start,
                                                                       end = expectedResult.End,
                                                                       maximumGap = expectedResult.MaximumGap,
                                                                       matcher = expectedResult.Matches)
            self.assertEqual(len(trajectories),len(expectedResult.Trajectories))
            self.assertEqual(len(interactions),len(expectedResult.Interactions))

            trajectories = sorted(trajectories,
                                  key = functools.cmp_to_key(QueryTestCase.compare_trajectories))
            for i,expected in enumerate(expectedResult.Trajectories):
                self.assertAntTrajectoryEqual(trajectories[i],expected)

            for i,expected in enumerate(expectedResult.Interactions):
                self.assertAntInteractionEqual(interactions[i],expected)


    def test_frame_selection(self):
        firstDate = min(ud.UData().NestDataDirs[0].Start,
                        ud.UData().ForagingDataDirs[0].Start)

        data = m.Query.IdentifyFrames(self.experiment,start = firstDate)
        self.assertEqual(len(data),len(ud.UData().ExpectedFrames))

        data = m.Query.IdentifyFrames(self.experiment,
                                      start = firstDate,
                                      end = firstDate.Add(1));
        self.assertTrue(len(data) > 0)
        self.assertTrue(len(data) <= 2)
        self.assertTimeEqual(data[0].FrameTime,firstDate)
        if len(data) == 2:
            self.assertTimeEqual(data[1].FrameTime,firstDate)

        data = m.Query.IdentifyFrames(self.experiment,
                                      start = firstDate,
                                      end = firstDate);
        self.assertEqual(len(data),0)
