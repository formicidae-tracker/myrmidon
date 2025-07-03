import fort_myrmidon as m
import unittest


class AntTestCase(unittest.TestCase):
    def setUp(self):
        self.experiment = m.Experiment("test-myrmidon")

    def tearDown(self):
        self.experiment = None

    def test_have_unique_id(self):
        for i in range(10):
            a = self.experiment.CreateAnt()
            self.assertEqual(a.ID, i + 1)

    def test_ant_have_sorted_identification(self):
        a = self.experiment.CreateAnt()
        t1 = m.Time.Now()
        t2 = t1.Add(1 * m.Duration.Second)

        i3 = self.experiment.AddIdentification(a.ID, 2, t2, m.Time.Forever())
        i2 = self.experiment.AddIdentification(a.ID, 1, t1, t2)
        i1 = self.experiment.AddIdentification(a.ID, 0, m.Time.SinceEver(), t1)

        self.assertEqual(len(a.Identifications), 3)
        self.assertEqual(a.Identifications[0], i1)
        self.assertEqual(a.Identifications[1], i2)
        self.assertEqual(a.Identifications[2], i3)

        self.experiment.DeleteIdentification(i1)
        self.assertEqual(len(a.Identifications), 2)
        self.assertEqual(a.Identifications[0], i2)
        self.assertEqual(a.Identifications[1], i3)

        self.assertEqual(a.IdentifiedAt(t2), 2)
        with self.assertRaises(RuntimeError):
            self.assertEqual(a.IdentifiedAt(t1.Add(-1)), 2)

    def test_have_display_status(self):
        a = self.experiment.CreateAnt()
        self.assertEqual(a.DisplayColor, m.DefaultPaletteColor(0))
        a.DisplayColor = m.DefaultPaletteColor(42)
        self.assertEqual(a.DisplayColor, m.DefaultPaletteColor(42))

        self.assertEqual(a.DisplayStatus, m.Ant.DisplayState.VISIBLE)
        a.DisplayStatus = m.Ant.DisplayState.HIDDEN
        self.assertEqual(a.DisplayStatus, m.Ant.DisplayState.HIDDEN)

    def test_have_static_value(self):
        a = self.experiment.CreateAnt()
        t = m.Time.Now()

        self.experiment.SetMetaDataKey("alive", True)
        values = a.GetValues("alive")
        self.assertEqual(len(values), 1)
        self.assertEqual(values[0], (m.Time.SinceEver(), True))

        with self.assertRaises(IndexError):
            a.GetValue(key="isDead", time=t)
        with self.assertRaises(IndexError):
            a.GetValues(key="isDead")
        with self.assertRaises(IndexError):
            a.SetValue(key="isDead", value=True, time=t)
        with self.assertRaises(RuntimeError):
            a.SetValue(key="alive", value=42, time=t)
        with self.assertRaises(ValueError):
            a.SetValue(key="alive", value=False, time=m.Time.Forever())

        a.SetValue(key="alive", value=False, time=t)

        values = a.GetValues("alive")
        self.assertEqual(len(values), 2)
        self.assertEqual(values[0], (m.Time.SinceEver(), True))
        self.assertEqual(values[1], (t, False))

        self.assertEqual(a.GetValue(key="alive", time=m.Time.SinceEver()), True)
        self.assertEqual(a.GetValue(key="alive", time=t.Add(-1)), True)
        self.assertEqual(a.GetValue(key="alive", time=t), False)
        self.assertEqual(a.GetValue(key="alive", time=m.Time.Forever()), False)

        with self.assertRaises(IndexError):
            a.DeleteValue(key="isDead", time=t)

        with self.assertRaises(IndexError):
            a.DeleteValue(key="alive", time=t.Add(1))

        a.DeleteValue(key="alive", time=t)
        self.assertEqual(a.GetValue(key="alive", time=t), True)
        self.assertEqual(a.GetValue(key="alive", time=m.Time.Forever()), True)

        self.experiment.SetMetaDataKey("alive", False)
        self.assertEqual(a.GetValue(key="alive", time=m.Time.SinceEver()), False)
        self.assertEqual(a.GetValue(key="alive", time=t.Add(-1)), False)
        self.assertEqual(a.GetValue(key="alive", time=t), False)
        self.assertEqual(a.GetValue(key="alive", time=m.Time.Forever()), False)

    def test_have_virtual_shape(self):
        self.experiment.CreateAntShapeType("body")
        self.experiment.CreateAntShapeType("antenna")
        a = self.experiment.CreateAnt()
        self.assertEqual(len(a.Capsules), 0)
        c1 = m.Capsule((0, 0), (1, 1), 1, 1)
        c2 = m.Capsule((0, 0), (-1, -1), 1, 1)
        with self.assertRaises(ValueError):
            a.AddCapsule(42, c1)
        a.AddCapsule(1, c1)
        a.AddCapsule(1, c2)
        a.AddCapsule(2, c1)

        self.assertEqual(len(a.Capsules), 3)
        self.assertEqual(a.Capsules[0][1], c1)
        self.assertEqual(a.Capsules[1][1], c2)
        self.assertEqual(a.Capsules[2][1], c1)

        with self.assertRaises(IndexError):
            a.DeleteCapsule(42)

        a.DeleteCapsule(1)
        self.assertEqual(len(a.Capsules), 2)
        self.assertEqual(a.Capsules[0][1], c1)
        self.assertEqual(a.Capsules[1][1], c1)

        a.ClearCapsules()
        self.assertEqual(len(a.Capsules), 0)

    def test_scope_vailidity(self):
        a = self.experiment.CreateAnt()
        self.experiment.SetMetaDataKey("alive", True)
        self.experiment = None
        a.SetValue("alive", False, m.Time())
