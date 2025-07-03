import fort_myrmidon as m
import fort_myrmidon_utestdata as ud
import unittest


class TypesTestCase(unittest.TestCase):
    def test_identified_frame_methods(self):
        data = next(x for (x, y) in ud.UData().ExpectedFrames if x.Space == 1)
        self.assertTrue(data.Contains(1))
        self.assertTrue(data.Contains(2))
        self.assertTrue(data.Contains(3))
        self.assertFalse(data.Contains(4))

        with self.assertRaises(IndexError):
            data.At(42)

        antID, position, zoneID = data.At(0)
        self.assertEqual(antID, 1)
        self.assertEqual(zoneID, 0)

    def test_ant_trajectory_methods(self):
        traj = ud.UData().ExpectedResults[0].Trajectories[0]
        self.assertEqual(
            traj.End(),
            traj.Start.Add(traj.Positions[-1, 0] * m.Duration.Second.Nanoseconds()),
        )

    def test_ant_trajectory_segment_methods(self):
        seg = ud.UData().ExpectedResults[0].Interactions[0].Trajectories[0]
        self.assertEqual(
            seg.StartTime(),
            seg.Trajectory.Start.Add(
                seg.Trajectory.Positions[seg.Begin, 0] * m.Duration.Second.Nanoseconds()
            ),
        )
        self.assertEqual(
            seg.EndTime(),
            seg.Trajectory.Start.Add(
                seg.Trajectory.Positions[seg.End - 1, 0]
                * m.Duration.Second.Nanoseconds()
            ),
        )
