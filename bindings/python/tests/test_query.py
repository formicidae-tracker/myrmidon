import unittest
import py_fort_myrmidon as m
import py_fort_myrmidon_utestdata as ud
import assertions

class QueryTestCase(unittest.TestCase,assertions.CustomAssertion):
    def setUp(self):
        self.experiment = m.Experiment.Open(str(ud.UData().CurrentVersionFile.AbsoluteFilePath))

    def tearDown(self):
        self.experiment = None

    def test_tag_statistics(self):
        tagStats = m.Query.ComputeTagStatistics(self.experiment)
        self.assertTagStatisticsEqual(tagStats,ud.UData().ExpectedTagStatistics)
