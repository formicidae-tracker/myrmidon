import py_fort_myrmidon as m
import numpy.testing as npt


class CustomAssertion:
    def assertVector2dEqual(self,a,b):
        npt.assert_almost_equal(a,b)

    def assertTimeEqual(self,a,b):
        self.assertIsInstance(a,m.Time)
        self.assertIsInstance(b,m.Time)
        if a.Equals(b) == False:
            raise AssertionError("time a: " + str(a) + " and b: " + str(b) +  " are not equals")

    def assertCapsuleEqual(self,a,b):
        self.assertVector2dEqual(a.C1,b.C1)
        self.assertVector2dEqual(a.C2,b.C2)
        self.assertEqual(a.R1,b.R1)
        self.assertEqual(a.R2,b.R2)


    def assertSingleTagStatEqual(self,a,b):
        self.assertEqual(a.ID,b.ID)
        self.assertTimeEqual(a.FirstSeen,b.FirstSeen)
        self.assertTimeEqual(a.LastSeen,b.LastSeen)
        npt.assert_equal(a.Counts,b.Counts)


    def assertTagStatisticsEqual(self,a,b):
        self.assertEqual(len(a),len(b))
        for tagID,expectedTagStat in b.items():
            self.assertTrue(tagID in a)
            self.assertSingleTagStatEqual(a[tagID],expectedTagStat)
