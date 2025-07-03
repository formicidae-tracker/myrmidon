import fort_myrmidon as m
import fort_myrmidon_utestdata as ud
import unittest


class TrackingSolverTestCase(unittest.TestCase):
    def setUp(self):
        self.experiment = m.Experiment.Open(
            str(ud.UData().CurrentVersionFile.AbsoluteFilePath)
        )
        self.solver = self.experiment.CompileTrackingSolver()

    def test_can_identify_ants(self):
        self.assertEqual(self.solver.IdentifyAnt(0, m.Time()), 1)
        self.assertEqual(self.solver.IdentifyAnt(1, m.Time()), 2)
        self.assertEqual(self.solver.IdentifyAnt(2, m.Time()), 3)
        self.assertEqual(self.solver.IdentifyAnt(123, m.Time()), 0)

    def test_can_identify_and_collide_frames(self):
        pass
