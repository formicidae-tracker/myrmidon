import unittest
import fort_myrmidon as m
import fort_myrmidon_utestdata as ud

from . import assertions


class IdentificationTestCase(unittest.TestCase, assertions.CustomAssertion):
    def setUp(self):
        self.experiment = m.Experiment(
            str(ud.UData().Basedir / "identification-utest.myrmidon")
        )
        self.ant = self.experiment.CreateAnt()
        self.i = self.experiment.AddIdentification(self.ant.ID, 123)

    def tearDown(self):
        self.i = None
        self.ant = None
        self.experiment = None

    def test_has_target_values(self):
        self.assertEqual(self.i.TagValue, 123)
        self.assertEqual(self.i.TargetAntID, self.ant.ID)

    def test_time_manipulation(self):
        self.experiment.DeleteIdentification(self.i)
        self.i = None
        idents = [
            self.experiment.AddIdentification(self.ant.ID, 123, end=m.Time()),
            self.experiment.AddIdentification(self.ant.ID, 124, start=m.Time()),
        ]
        self.assertTimeEqual(idents[0].Start, m.Time.SinceEver())
        self.assertTimeEqual(idents[0].End, m.Time())
        self.assertTimeEqual(idents[1].Start, m.Time())
        self.assertTimeEqual(idents[1].End, m.Time.Forever())

        with self.assertRaises(m.OverlappingIdentification):
            idents[0].End = m.Time().Add(1)

        with self.assertRaises(m.OverlappingIdentification):
            idents[1].Start = m.Time().Add(-1)

        idents[0].End = m.Time().Add(-1)
        idents[1].Start = m.Time().Add(1)

        self.assertTimeEqual(idents[0].End, m.Time().Add(-1))
        self.assertTimeEqual(idents[1].Start, m.Time().Add(1))

    def test_tag_size_manipulation(self):
        self.assertTrue(self.i.HasDefaultTagSize())
        self.assertEqual(self.i.TagSize, m.Identification.DEFAULT_TAG_SIZE)
        self.i.TagSize = 2.4
        self.assertEqual(self.i.TagSize, 2.4)
        self.assertFalse(self.i.HasDefaultTagSize())

    def test_ant_pose_manipulation(self):
        self.assertFalse(self.i.HasUserDefinedAntPose())
        self.assertVector2dEqual(self.i.AntPosition, [0, 0])
        self.assertEqual(self.i.AntAngle, 0.0)

        position = [1, 2]
        angle = 3.0
        self.i.SetUserDefinedAntPose(antPosition=position, antAngle=angle)
        self.assertVector2dEqual(self.i.AntPosition, position)
        self.assertEqual(self.i.AntAngle, angle)
        self.assertTrue(self.i.HasUserDefinedAntPose())

        self.i.ClearUserDefinedAntPose()
        self.assertFalse(self.i.HasUserDefinedAntPose())
        self.assertVector2dEqual(self.i.AntPosition, [0, 0])
        self.assertEqual(self.i.AntAngle, 0.0)

    def test_formatting(self):
        self.assertEqual(str(self.i), "Identification{ID:0x07b ↦ 1, From:-∞, To:+∞}")

    def test_scope_validity(self):
        self.experiment = None
        self.ant = None

        self.i.Start = m.Time()
