import py_fort_myrmidon as m
import unittest
import datetime


class TimeTestCase(unittest.TestCase):
    def test_constructor(self):
        self.assertEqual(m.Time(), m.Time(0.0))
        self.assertEqual(m.Time(), 0.0)

    def test_has_infinite_support(self):
        self.assertEqual(m.Time.SinceEver().ToTimestamp(), float('-inf'))
        self.assertEqual(m.Time.Forever().ToTimestamp(), float('inf'))
        self.assertEqual(m.Time(float('-inf')), m.Time.SinceEver())
        self.assertEqual(m.Time(float('inf')), m.Time.Forever())

    def test_has_math_support(self):
        t = m.Time.Now().Round(m.Duration.Second)

        # we makes a deep copy of the time we use by passing it forth
        # and back to a float
        u = m.Time(t.ToDateTime())

        self.assertEqual(t.Add(1).Sub(t), 1)
        self.assertEqual(t.Add(1*m.Duration.Second).Sub(t), m.Duration.Second)

        # we can use the verbose comparators Equals/After/Before or
        # the overloaded operators
        self.assertFalse(t > t)
        self.assertFalse(t.After(t))
        self.assertFalse(t < t)
        self.assertFalse(t.Before(t))
        self.assertTrue(t.Add(1) > t)
        self.assertTrue(t.Add(1).After(t))
        self.assertFalse(t > t.Add(1))
        self.assertFalse(t.After(t.Add(1)))
        self.assertTrue(t == t)
        self.assertTrue(t.Equals(t))

        # all modification did not modify the original t
        self.assertEqual(t, u)

    def test_datetime_conversion(self):
        # create a datetime from UTC, and convert it to localtime
        d = datetime.datetime.fromisoformat(
            "2019-11-02T23:12:13.000014+00:00").astimezone(None)
        t = m.Time(d)
        self.assertEqual(t, m.Time.Parse("2019-11-02T23:12:13.000014Z"))
        self.assertEqual(d, t.ToDateTime().astimezone(None))
