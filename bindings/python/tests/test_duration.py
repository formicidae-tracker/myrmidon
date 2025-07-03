import fort_myrmidon as m
import unittest


class DurationTestCase(unittest.TestCase):
    def test_constructors(self):
        self.assertEqual(m.Duration(), 0)
        self.assertEqual(m.Duration(421), 421)

    def test_arithmetic(self):
        self.assertEqual(m.Duration.Second + 1, int(1e9) + 1)
        self.assertEqual(1 + m.Duration.Second, int(1e9) + 1)
        self.assertEqual(m.Duration.Hour + m.Duration.Second, int(3601 * 1e9))

        self.assertEqual(m.Duration.Second - 1, int(1e9) - 1)
        self.assertEqual(1 - m.Duration.Second, 1 - int(1e9))
        self.assertEqual(m.Duration.Hour - m.Duration.Second, int(3599 * 1e9))

        self.assertEqual(m.Duration() * m.Duration.Hour, 0)
        self.assertEqual(1 * m.Duration.Hour, int(3600 * 1e9))
        self.assertEqual(m.Duration.Hour * 2, int(2 * 3600 * 1e9))

        self.assertTrue(m.Duration() == 0)
        self.assertTrue(m.Duration(1) > 0)
        self.assertFalse(m.Duration(1) < 0)

        self.assertTrue(m.Duration(1) != 0)

    def test_formatting(self):
        data = [
            ("0s", m.Duration(0)),
            ("1ns", m.Duration(1)),
            ("1.1µs", m.Duration(1100)),
            ("2.2ms", 2200 * m.Duration.Microsecond),
            ("3.3s", 3300 * m.Duration.Millisecond),
            ("-4.4s", -4400 * m.Duration.Millisecond),
            ("4m5s", 4 * m.Duration.Minute + 5 * m.Duration.Second),
            ("4m5.001s", 4 * m.Duration.Minute + 5001 * m.Duration.Millisecond),
            (
                "5h6m7.001s",
                5 * m.Duration.Hour
                + 6 * m.Duration.Minute
                + 7001 * m.Duration.Millisecond,
            ),
            ("8m1e-09s", 8 * m.Duration.Minute + 1),
            ("2562047h47m16.854775807s", m.Duration(2 ** 63 - 1)),
            ("-2562047h47m16.854775808s", m.Duration(-(2 ** 63))),
        ]
        for expected, d in data:
            self.assertEqual(str(d), expected)

    def test_constant(self):
        with self.assertRaises(Exception):
            self.assertEqual(m.Duration.Hour, int(3600 * 1e9))
            m.Duration.Hour = 0
        with self.assertRaises(Exception):
            self.assertEqual(m.Duration.Minute, int(60 * 1e9))
            m.Duration.Minute = 0
        with self.assertRaises(Exception):
            self.assertEqual(m.Duration.Second, int(1e9))
            m.Duration.Second = 0
        with self.assertRaises(Exception):
            self.assertEqual(m.Duration.Millisecond, int(1e6))
            m.Duration.Millisecond = 0
        with self.assertRaises(Exception):
            self.assertEqual(m.Duration.Microsecond, int(1e3))
            m.Duration.Microsecond = 0

    def test_duration_cast(self):
        d = m.Duration.Hour
        self.assertEqual(d.Hours(), 1.0)
        self.assertEqual(d.Minutes(), 60.0)
        self.assertEqual(d.Seconds(), 3600.0)
        self.assertEqual(d.Milliseconds(), 3.6e6)
        self.assertEqual(d.Microseconds(), 3.6e9)
        self.assertEqual(d.Nanoseconds(), 36e11)

    def test_duration_parsing(self):
        data = [
            ("0", True, m.Duration(0)),
            ("5s", True, 5 * m.Duration.Second),
            ("30s", True, 30 * m.Duration.Second),
            ("1478s", True, 1478 * m.Duration.Second),
            # sign
            ("-5s", True, -5 * m.Duration.Second),
            ("+5s", True, 5 * m.Duration.Second),
            ("-0", True, m.Duration(0)),
            ("+0", True, m.Duration(0)),
            # decimal
            ("5.0s", True, 5 * m.Duration.Second),
            ("5.6s", True, 5 * m.Duration.Second + 600 * m.Duration.Millisecond),
            ("5.s", True, 5 * m.Duration.Second),
            (".5s", True, 500 * m.Duration.Millisecond),
            ("1.0s", True, 1 * m.Duration.Second),
            ("1.00s", True, 1 * m.Duration.Second),
            ("1.004s", True, 1 * m.Duration.Second + 4 * m.Duration.Millisecond),
            ("1.0040s", True, 1 * m.Duration.Second + 4 * m.Duration.Millisecond),
            ("100.00100s", True, 100 * m.Duration.Second + 1 * m.Duration.Millisecond),
            # different units
            ("10ns", True, m.Duration(10)),
            ("11us", True, 11 * m.Duration.Microsecond),
            ("12µs", True, 12 * m.Duration.Microsecond),  # U+00B5
            ("12μs", True, 12 * m.Duration.Microsecond),  # U+03BC
            ("13ms", True, 13 * m.Duration.Millisecond),
            ("14s", True, 14 * m.Duration.Second),
            ("15m", True, 15 * m.Duration.Minute),
            ("16h", True, 16 * m.Duration.Hour),
            # composite durations
            ("3h30m", True, 3 * m.Duration.Hour + 30 * m.Duration.Minute),
            (
                "10.5s4m",
                True,
                4 * m.Duration.Minute
                + 10 * m.Duration.Second
                + 500 * m.Duration.Millisecond,
            ),
            (
                "-2m3.4s",
                True,
                -2 * m.Duration.Minute
                - 3 * m.Duration.Second
                - 400 * m.Duration.Millisecond,
            ),
            (
                "1h2m3s4ms5us6ns",
                True,
                1 * m.Duration.Hour
                + 2 * m.Duration.Minute
                + 3 * m.Duration.Second
                + 4 * m.Duration.Millisecond
                + 5 * m.Duration.Microsecond
                + 6,
            ),
            (
                "39h9m14.425s",
                True,
                39 * m.Duration.Hour
                + 9 * m.Duration.Minute
                + 14 * m.Duration.Second
                + 425 * m.Duration.Millisecond,
            ),
            # large value
            ("52763797000ns", True, m.Duration(52763797000)),
            # more than 9 digits after decimal point, see https://golang.org/issue/6617
            ("0.3333333333333333333h", True, 20 * m.Duration.Minute),
            # 9007199254740993 = 1<<53+1 cannot be stored precisely in a float64
            ("9007199254740993ns", True, m.Duration((1 << 53) + 1)),
            # largest duration that can be represented by int64 in nanoseconds
            ("9223372036854775807ns", True, m.Duration(2 ** 63 - 1)),
            ("9223372036854775.807us", True, m.Duration(2 ** 63 - 1)),
            ("9223372036s854ms775us807ns", True, m.Duration(2 ** 63 - 1)),
            # large negative value
            ("-9223372036854775807ns", True, m.Duration(-1 << 63) + 1),
            # huge string; issue 15011.
            ("0.100000000000000000000h", True, 6 * m.Duration.Minute),
            # This value tests the first overflow check in leadingFraction.
            (
                "0.830103483285477580700h",
                True,
                49 * m.Duration.Minute + 48 * m.Duration.Second + 372539827,
            ),
            # errors
            ("", False, m.Duration(0)),
            ("3", False, m.Duration(0)),
            ("-", False, m.Duration(0)),
            ("s", False, m.Duration(0)),
            (".", False, m.Duration(0)),
            ("-.", False, m.Duration(0)),
            (".s", False, m.Duration(0)),
            ("+.s", False, m.Duration(0)),
            # overflows errors
            ("3000000h", False, m.Duration(0)),
            ("9223372036854775808ns", False, m.Duration(0)),
            ("9223372036854775.808us", False, m.Duration(0)),
            ("9223372036854ms775us808ns", False, m.Duration(0)),
            # largest negative value of type int64 in nanoseconds should fail
            # see https://go-review.googlesource.com/#/c/2461/
            ("-9223372036854775808ns", False, m.Duration(0)),
        ]

        for v, hasNoError, expected in data:
            if hasNoError:
                res = m.Duration.Parse(v)
                self.assertEqual(
                    res,
                    expected,
                    msg="'%s' should parse to %d (is %s) got %d"
                    % (v, expected.Nanoseconds(), expected, res.Nanoseconds()),
                )
            else:
                with self.assertRaises(
                    RuntimeError, msg="'%s' should throw an error" % v
                ):
                    m.Duration.Parse(v)
