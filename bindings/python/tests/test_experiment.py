import py_fort_myrmidon as m
import unittest
import py_fort_myrmidon_utestdata as ud

class ExperimentTestCase(unittest.TestCase):
    def setUp(self):
        self.experiment = m.Experiment(ud.UData().Basedir + '/foo.myrmidon')

    def tearDown(self):
        self.experiment = None

    def test_opening_dataless(self):
        self.assertTrue( self.experiment != None)
