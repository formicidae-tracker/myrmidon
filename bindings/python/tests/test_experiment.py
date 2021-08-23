import py_fort_myrmidon as m
import unittest

class ExperimentTestCase(unittest.TestCase):
    def setUp(self):
        self.experiment = m.Experiment

    def tearDown(self):
        self.experiment = None
