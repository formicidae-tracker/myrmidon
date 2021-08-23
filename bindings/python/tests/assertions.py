import py_fort_myrmidon as m
import numpy.testing as npt


class CustomAssertion:
    def assertVector2dEqual(self,a,b):
        npt.assert_almost_equal(a,b)

    def assertTimeEqual(self,a,b):
        self.assertIsInstance(a,m.Time)
        self.assertIsInstance(b,m.Time)
        if a.Equals(b) == False:
            raise AssertionError("time are not equals")

    def assertCapsuleEqual(self,a,b):
        self.assertVector2dEqual(a.C1,b.C1)
        self.assertVector2dEqual(a.C2,b.C2)
        self.assertEqual(a.R1,b.R1)
        self.assertEqual(a.R2,b.R2)
