import fort_myrmidon as m
import unittest
import datetime


class TimeTestCase(unittest.TestCase):
    def test_constructor(self):
        self.assertEqual(m.Time(), m.Time(0.0))
        self.assertEqual(m.Time(), 0.0)

    def test_has_infinite_support(self):
        self.assertEqual(m.Time.SinceEver().ToTimestamp(), float("-inf"))
        self.assertEqual(m.Time.Forever().ToTimestamp(), float("inf"))
        self.assertEqual(m.Time(float("-inf")), m.Time.SinceEver())
        self.assertEqual(m.Time(float("inf")), m.Time.Forever())
        self.assertTrue(m.Time.Forever().IsInfinite())
        self.assertTrue(m.Time.Forever().IsForever())
        self.assertFalse(m.Time.Forever().IsSinceEver())
        self.assertTrue(m.Time.SinceEver().IsInfinite())
        self.assertFalse(m.Time.SinceEver().IsForever())
        self.assertTrue(m.Time.SinceEver().IsSinceEver())

    def test_has_math_support(self):
        t = m.Time.Now().Round(m.Duration.Second)

        # we makes a deep copy of the time we use by passing it forth
        # and back to a float
        u = m.Time(t.ToDateTime())

        self.assertEqual(t.Add(1).Sub(t), 1)
        self.assertEqual(t.Add(1 * m.Duration.Second).Sub(t), m.Duration.Second)

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
            "2019-11-02T23:12:13.000014+00:00"
        ).astimezone(None)
        t = m.Time(d)
        self.assertEqual(t, m.Time.Parse("2019-11-02T23:12:13.000014Z"))
        self.assertEqual(d, t.ToDateTime().astimezone(None))

    def test_time_overflow(self):
        with self.assertRaises(RuntimeError):
            m.Time(2.0 ** 64)

    def test_infinite_time_computation_overflow(self):
        operations = [
            (lambda: m.Time.Forever().Add(0), False),
            (lambda: m.Time.Forever().Add(1), True),
            (lambda: m.Time.Forever().Add(-1), True),
            (lambda: m.Time.SinceEver().Add(0), False),
            (lambda: m.Time.SinceEver().Add(1), True),
            (lambda: m.Time.SinceEver().Add(-1), True),
            (lambda: m.Time.SinceEver().Sub(m.Time()), True),
            (lambda: m.Time.SinceEver().Sub(m.Time()), True),
        ]
        for op, hasError in operations:
            if hasError:
                with self.assertRaises(RuntimeError):
                    op()
            else:
                op()

    def test_time_infinite_rounding_is_noop(self):
        self.assertTrue(m.Time.Forever().Round(0).Equals(m.Time().Forever()))
        self.assertTrue(m.Time.SinceEver().Round(0).Equals(m.Time().SinceEver()))

    def test_time_rounding(self):
        data = [
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                m.Duration(10),
                m.Time.Parse("2020-03-20T15:34:08.86512357Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                m.Duration(100),
                m.Time.Parse("2020-03-20T15:34:08.8651236Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                1 * m.Duration.Microsecond,
                m.Time.Parse("2020-03-20T15:34:08.865124Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                10 * m.Duration.Microsecond,
                m.Time.Parse("2020-03-20T15:34:08.86512Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                100 * m.Duration.Microsecond,
                m.Time.Parse("2020-03-20T15:34:08.8651Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                1 * m.Duration.Millisecond,
                m.Time.Parse("2020-03-20T15:34:08.865Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                10 * m.Duration.Millisecond,
                m.Time.Parse("2020-03-20T15:34:08.87Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                100 * m.Duration.Millisecond,
                m.Time.Parse("2020-03-20T15:34:08.9Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                m.Duration.Second,
                m.Time.Parse("2020-03-20T15:34:09Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                2 * m.Duration.Second,
                m.Time.Parse("2020-03-20T15:34:08Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                m.Duration.Minute,
                m.Time.Parse("2020-03-20T15:34:00Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                m.Duration.Hour,
                m.Time.Parse("2020-03-20T16:00:00Z"),
            ),
            (
                m.Time.Parse("2020-03-20T15:34:08.865123567Z"),
                24 * m.Duration.Hour,
                m.Time.Parse("2020-03-21T00:00:00Z"),
            ),
        ]
        for v, d, expected in data:
            self.assertEqual(v.Round(d), expected)

    def test_infinite_time_comparisons(self):
        data = [
            m.Time(),
            m.Time().Add(2 ** 63 - 1),
            m.Time().Add(-(2 ** 63)),
        ]
        for t in data:
            self.assertTrue(t < m.Time.Forever())
            self.assertTrue(t > m.Time.SinceEver())

        self.assertFalse(m.Time.Forever() > m.Time.Forever())
        self.assertFalse(m.Time.Forever() < m.Time.Forever())
        self.assertTrue(m.Time.Forever() == m.Time.Forever())

        self.assertFalse(m.Time.SinceEver() > m.Time.SinceEver())
        self.assertFalse(m.Time.SinceEver() < m.Time.SinceEver())
        self.assertTrue(m.Time.SinceEver() == m.Time.SinceEver())

    def test_time_parsing(self):

        self.assertEqual(
            m.Time.Parse("1970-01-02T01:02:03.004Z"),
            m.Time().Add(
                25 * m.Duration.Hour
                + 2 * m.Duration.Minute
                + 3 * m.Duration.Second
                + 4 * m.Duration.Millisecond
            ),
        )

        with self.assertRaises(RuntimeError):
            m.Time.Parse("-∞")
        with self.assertRaises(RuntimeError):
            m.Time.Parse("+∞")
