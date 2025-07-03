import unittest
import fort_myrmidon as m
import fort_myrmidon_utestdata as ud


class SpaceTestCase(unittest.TestCase):
    def setUp(self):
        self.experiment = m.Experiment(str(ud.UData().Basedir / "space-utest.myrmidon"))
        self.space = self.experiment.CreateSpace("foo")

    def tearDown(self):
        self.space = None
        self.experiment = None

    def test_fields_manipulation(self):
        self.assertEqual(self.space.ID, 1)
        self.assertEqual(self.space.Name, "foo")
        self.space.Name = "bar"
        self.assertEqual(self.space.Name, "bar")

    def test_zone_manipulation(self):
        z = self.space.CreateZone("food")
        self.assertEqual(len(self.space.Zones), 1)
        self.assertEqual(self.space.Zones[z.ID], z)

        with self.assertRaises(IndexError):
            self.space.DeleteZone(42)

        self.space.DeleteZone(z.ID)

    def test_can_locate_movie_frame(self):
        self.experiment = m.Experiment.Open(
            str(ud.UData().CurrentVersionFile.AbsoluteFilePath)
        )
        tddInfo = ud.UData().WithVideoDataDir
        filepath, frame = self.experiment.Spaces[1].LocateMovieFrame(tddInfo.Start)
        self.assertEqual(filepath, str(tddInfo.AbsoluteFilePath / "stream.0000.mp4"))
        self.assertEqual(frame, 0)
        with self.assertRaises(IndexError):
            filepath, frame = self.experiment.Spaces[1].LocateMovieFrame(tddInfo.End)
