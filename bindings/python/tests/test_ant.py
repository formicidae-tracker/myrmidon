import py_fort_myrmidon as m
import unittest

class AntTestCase(unittest.TestCase):
    def setUp(self):
        self.experiment = m.Experiment("test-myrmidon")

    def tearDown(self):
        self.experiment = None;

    def test_have_unique_id(self):
        for i in range(10):
            a = self.experiment.CreateAnt()
            self.assertEqual(a.ID,i+1)

    def test_ant_have_sorted_identification(self):
        a = self.experiment.CreateAnt()
        t1 = m.Time.Now()
        t2 = t1.Add(1 * m.Duration.Second)

        i3 = self.experiment.AddIdentification(a.ID,
                                               2,
                                               t2,
                                               m.Time.Forever());
        i2 = self.experiment.AddIdentification(a.ID,
                                               1,
                                               t1,
                                               t2)
        i1 = self.experiment.AddIdentification(a.ID,
                                               0,
                                               m.Time.SinceEver(),
                                               t1);

        self.assertEqual(len(a.Identifications),3)
        self.assertEqual(a.Identifications[0],i1)
        self.assertEqual(a.Identifications[1],i2)
        self.assertEqual(a.Identifications[2],i3)

        self.experiment.DeleteIdentification(i1);
        self.assertEqual(len(a.Identifications),2)
        self.assertEqual(a.Identifications[0],i2)
        self.assertEqual(a.Identifications[1],i3)

        self.assertEqual(a.IdentifiedAt(t2),2);
        with self.assertRaises(RuntimeError):
            self.assertEqual(a.IdentifiedAt(t1.Add(-1)),2);
