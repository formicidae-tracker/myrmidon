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
        self.assertRegex(repr(data),"IdentifiedFrame{Time:.*, len\\(Positions\\):[0-9]+}")
        self.assertEqual(repr(data),str(data))

    def test_ant_trajectory_methods(self):
        traj = ud.UData().ExpectedResults[0].Trajectories[0]
        self.assertEqual(
            traj.End(),
            traj.Start.Add(traj.Positions[-1, 0] * m.Duration.Second.Nanoseconds()),
        )
        self.assertRegex(repr(traj),"AntTrajectory{Ant:[0-9]+, Space:[0-9]+, Start:.*, len\\(Positions\\):[0-9]+}")
        self.assertEqual(repr(traj),str(traj))

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
        self.assertEqual(repr(seg),str(seg))
        self.assertRegex(repr(seg),"AntTrajectorySegment{Ant:[0-9], Start:.*, Range:\\[[0-9]+-[0-9]+\\[}")

    def test_ant_interaction_methods(self):
        i = ud.UData().ExpectedResults[0].Interactions[0]
        self.assertEqual(repr(i),str(i))
        self.assertRegex(repr(i),"AntInteraction{Ants:[0-9]+-[0-9]+, Start:.*, End:.*, Types:\\[([0-9]+-[0-9]+(, )?)+\\]}")

    def test_tag_statistics(self):
        s = ud.UData().ExpectedTagStatistics[1]
        self.assertEqual(str(s),repr(s))
        self.assertRegex(repr(s),"TagStatistics{ID:0x[0-9a-f]+, Total:[0-9]+, FirstSeen:.*, LastSeen:.*}")

    def test_computed_measurements(self):
        a = m.ComputedMeasurement(m.Time.Now(),12.0,220.0)
        self.assertRegex(repr(a),"ComputedMeasurement{Length_mm:12, Length_px:220, Time:.*}")
        self.assertEqual(repr(a),str(a))

    def test_collision_frame(self):
        cf = None
        for [_,cf_] in ud.UData().ExpectedFrames:
            if len(cf_.Collisions)>0:
                cf = cf_
                break

        self.assertIsNotNone(cf)
        c = cf.Collisions[0]
        self.assertEqual(type(c),m.Collision)
        self.assertEqual(str(c),repr(c))
        self.assertRegex(repr(c),"Collision{Ants:[0-9]+-[0-9]+, Zone:[0-9]+, Types:\\[([0-9]+-[0-9]+(, )?)*\\]}")
        self.assertEqual(str(cf),repr(cf))
        self.assertRegex(repr(cf),"CollisionFrame{Space:[0-9]+, Time:.*, len\\(Collisions\\):[0-9]+}")

    def test_ant_trajectory_summary(self):
        ts = ud.UData().ExpectedResults[0].Summarized()[0].Trajectories[0]

        self.assertEqual(type(ts),m.AntTrajectorySummary)
        self.assertEqual(repr(ts),str(ts))
        self.assertRegex(repr(ts),"AntTrajectorySummary{Ant:[0-9]+, Mean:\\[([0-9.](, )?)+\\], Zones:\\[([0-9]+(, )?)+\\]}")

    def test_experiment_data_info(self):
        ei = ud.UData().CurrentExperimentDataInfo
        self.assertEqual(type(ei),m.ExperimentDataInfo)
        si = ei.Spaces[1]
        self.assertEqual(type(si),m.SpaceDataInfo)
        tddi = si.TrackingDataDirectories[0]
        self.assertEqual(type(tddi),m.TrackingDataDirectoryInfo)

        self.assertEqual(str(tddi),repr(tddi))
        self.assertRegex(repr(tddi),"TrackingDataDirectoryInfo{URI:.*, Frames:[0-9]+, Start:.*, End:.*}")

        self.assertEqual(str(si),repr(si))
        self.assertRegex(repr(si),"SpaceDataInfo{Name:.*, Frames:[0-9]+, Start:.*, End:.*, len\\(TDD\\):[0-9]+}")

        self.assertEqual(str(ei),repr(ei))
        self.assertRegex(repr(ei),"ExperimentDataInfo{Frames:[0-9]+, Start:.*, End:.*, Spaces:\\[([0-9]+(, )?)+\\]}")
