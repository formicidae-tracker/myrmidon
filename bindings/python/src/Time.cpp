#include "BindMethods.hpp"

#include <fort/time/Time.hpp>


void BindDuration(py::module_ & m) {
	py::class_<fort::Duration>(m,
	                           "Duration",
	                           R"pydoc(
Represents an amount of nanoseconds as a signed 64-bit integer

Note:
    The notion of 64-bit integer does not exist natively in python,
    but since the mapped c++ object is, it will works like a 64-bit
    integer.

Attributes:
    Hour (Duration): the value for an hour
    Minute (Duration): the value for a minute
    Second (Duration): the value for a second
    Millisecond (Duration): the value for a millisecond
    Microsecond (Duration): the value for a microsecond
)pydoc")
		.def(py::init<int64_t>(),
		     py::arg("ns"),
		     R"pydoc(
Initialize a Duration from an amount of nanoseconds

Args:
    ns (int): the number of nanosecond to represent
)pydoc")
		.def(py::init([](double ns){
			              return fort::Duration(int64_t(ns));
		              }),
			py::arg("ns"),
			R"pydoc(
Initialize a Duration from an amount of nanoseconds

Args:
    ns (float): the number of nanosecond to represent
)pydoc")
		.def(py::init<>(),
		     "Initialize a zero second Duration.")
		.def("__str__",[](const fort::Duration & d) -> std::string {
			               std::ostringstream oss;
			               oss << d;
			               return oss.str();
		               })
		.def_readonly_static("Hour",
		                     &fort::Duration::Hour,
		                     "Duration : the value for one hour.")
		.def_readonly_static("Minute",
		                     &fort::Duration::Minute,
		                     "Duration: A minute as a Duration")
		.def_readonly_static("Second",
		                     &fort::Duration::Second,
		                     "Duration: A second as a Duration")
		.def_readonly_static("Millisecond",
		                     &fort::Duration::Millisecond,
		                     "Duration: A millisecond as a Duration")
		.def_readonly_static("Microsecond",
		                     &fort::Duration::Microsecond,
		                     "Duration: A microsecond as a Duration")
		.def_static("Parse",
		            &fort::Duration::Parse,
		            py::arg("d"),
		            R"pydoc(
Parses a string to a Duration.

Args:
    d (str): a string in the format `[amount][unit]` as a
        duration. `[amout]` is a value that may contain a decimal
        point, and `[units]` could be any of
        'h','m','s','ms','us','ns'. The pattern can be repeated
        (i.e. '4m32s' is valid).

Returns:
    Duration: the parsed value

Raises:
    RuntimeError: when the parsed amount will not hold in a 64-bit
        signed integer
)pydoc")
		.def("Hours",
		     &fort::Duration::Hours,
		     R"pydoc(
    this Duration in hours.

    Returns:
        float: the duration as an amount of hours
)pydoc")
		.def("Minutes",
		     &fort::Duration::Minutes,
		     R"pydoc(
This Duration in minutes.

Returns:
    float: the duration as an amount of minutes
)pydoc")
		.def("Seconds",
		     &fort::Duration::Seconds,
		     R"pydoc(
This Duration in seconds.

Returns:
    float: the duration as an amount of seconds
)pydoc")
		.def("Milliseconds",
		     &fort::Duration::Milliseconds,
		     R"pydoc(
This Duration in milliseconds.

Returns:
    float: the duration as an amount of milliseconds
)pydoc")
		.def("Microseconds",
		     &fort::Duration::Microseconds,
		     R"pydoc(
This Duration in microseconds.

Returns:
    float: the duration as an amount of microseconds
)pydoc")
		.def("Nanoseconds",
		     &fort::Duration::Nanoseconds,
		     R"pydoc(
This Duration in nanoseconds.

Returns:
    int: the duration as an amount of nanoseconds
)pydoc")
		.def(py::self + py::self)
		.def(py::self + int())
		.def("__radd__",[](const fort::Duration & d, int64_t v) -> fort::Duration {
			                return fort::Duration(v) + d;
		                },
			py::is_operator())
		.def(py::self - py::self)
		.def(py::self - int())
		.def("__rsub__",[](const fort::Duration & d, int64_t v) -> fort::Duration {
			                return fort::Duration(v) - d;
		                },
			py::is_operator())

		.def(py::self * py::self)
		.def(py::self * int())
		.def("__rmul__",[](const fort::Duration & d, int64_t v) -> fort::Duration {
			                return v * d;
		                },
			py::is_operator())
		.def(py::self < py::self)
		.def(py::self <= py::self)
		.def(py::self > py::self)
		.def(py::self >= py::self)
		.def(py::self == py::self)
		;

	py::implicitly_convertible<int64_t,fort::Duration>();
	py::implicitly_convertible<double,fort::Duration>();
}

fort::Time timeFromPythonTimestamp(const double & t) {
	if ( std::isinf(t) ) {
		return t > 0 ? fort::Time::Forever() : fort::Time::SinceEver();
	}
	int64_t s = std::floor(t);
	int32_t ns = 1e9 * (t - s);
	return fort::Time::FromUnix(s,ns);
}

fort::Time timeFromPythonDatetime(const std::chrono::system_clock::time_point & t) {
	using namespace std::chrono;

	auto ns = duration_cast<nanoseconds>(t.time_since_epoch() % seconds(1));
	if ( ns.count() < 0 ) {
		ns += seconds(1);
	}

	std::time_t ltime = system_clock::to_time_t(time_point_cast<system_clock::duration>(t - ns));

	return fort::Time::FromUnix(ltime,ns.count());
}

void BindTime(py::module_ &m) {
	BindDuration(m);

	py::class_<fort::Time>(
	    m,
	    "Time",
	    R"pydoc(
Represents a point in time.

This object represents a point in time, potentially +/-∞. It
features operation to compare or measure a Duration between two
Time.

The operation manipulating Time objects never modifies the original
objects, and always return new allocated object, so Time object
can be considered immuable.

It also provides methods to convert to and from
:class:`datetime.datetime` object. In that case these objects
are considered naïve: expressed in localtime, and ignoring any
associated timezone information.


It provides methods to convert to and from :func:`time.time` and
:meth:`datetime.datetime.timestamp` float values. However for time around
2020, these only ensure a 10us precision, but Time objects are
precise to the nanosecond.

)pydoc"
	)
	    .def(py::init<>(), R"pydoc(
Initialize a Time as the epoch.
)pydoc")
	    .def(
	        py::init(&timeFromPythonTimestamp),
	        py::arg("timestamp"),
	        R"pydoc(
Initializes a Time from a float as returned by :func:`time.time` or
:meth:`datetime.datetime.timestamp`.

Args:
    timestamp (float): an amount of second since the epoch. Could be
        ``float('inf')`` or ``float('-inf')``.

Note:
    timestamp are only guaranted to be precise to 10us for Time around
    year 2020.
)pydoc"
	    )
	    .def(
	        py::init(&timeFromPythonDatetime),
	        py::arg("dt"),
	        R"pydoc(
Initialize from a :class:`datetime.datetime` object.

Creates a Time from a :class:`datetime.datetime`. The object will be
treated as a local naive datetime, i.e. expressed in localtime and
ignoring any time information.

Args:
    dt (datetime.datetime): a naïve :class:`datetime.datetime`.

Warning:
    The dt parameter will be treated as a naïve object, disregarding any
    associated timezone information. Therefore one must call
    :meth:`datetime.datetime.astimezone` with ``tz =None`` to convert the
    object in localtime so it would be correctly converted to UTC
    internally, which is the default representation in
    **fort-myrmidon** and **fort-studio**.
)pydoc"
	    )
	    .def_static(
	        "SinceEver",
	        &fort::Time::SinceEver,
	        R"pydoc(
The negative infinite time.

Returns:
    Time: A Time representing -∞
)pydoc"
	    )
	    .def_static(
	        "Forever",
	        &fort::Time::Forever,
	        R"pydoc(
The positive infinitie time.

Returns:
    Time: A Time representing +∞
)pydoc"
	    )
	    .def_static(
	        "Now",
	        &fort::Time::Now,
	        R"pydoc(
Gets the current Time

Returns:
    Time: the current Time
)pydoc"
	    )
	    .def_static(
	        "Parse",
	        &fort::Time::Parse,
	        py::arg("input"),
	        R"pydoc(
Parses a RFC3339 string to a Time.

Parses a `RFC3339 <https://www.ietf.org/rfc/rfc3339.txt>`_ string
(i.e. '1970-01-01T00:00:00.000Z') to a Time.

Args:
    input (str): the string to parse

Returns:
    fort_myrmidon.Time: a Time that represent input

Raises:
    Error: if input is a Time that is not representable.
)pydoc"
	    )
	    .def(
	        "ToTimestamp",
	        [](const fort::Time &t) -> double {
		        if (t.IsInfinite() == true) {
			        return t.IsForever()
			                   ? std::numeric_limits<double>::infinity()
			                   : -std::numeric_limits<double>::infinity();
		        }
		        auto   ts  = t.ToTimestamp();
		        double res = ts.seconds();
		        res += 1e-9 * ts.nanos();
		        return res;
	        },
	        R"pydoc(
Converts to a float as returned by :func:`time.time` or
:meth:`datetime.datetime.timestamp`

Returns:
    float: an amount of second since the system's epoch
)pydoc"
	    )
	    .def(
	        "ToDateTime",
	        [](const fort::Time &t) -> std::chrono::system_clock::time_point {
		        if (t.IsInfinite() == true) {
			        throw std::runtime_error(
			            "Cannot cast " + t.Format() +
			            " to a datetime.datetime object"
			        );
		        }
		        auto ts = t.ToTimestamp();
		        return std::chrono::system_clock::time_point(
		            std::chrono::seconds(ts.seconds()) +
		            std::chrono::nanoseconds(ts.nanos())
		        );
	        },
	        R"pydoc(
Converts to a :class:`datetime.datetime`

The returned object will be a naïve datetime, i.e. expressed in
localtime without any timezone information.

Returns:
    datetime.datetime: a naive datetime.datetime object.
)pydoc"
	    )
	    .def(
	        "Add",
	        &fort::Time::Add,
	        py::arg("d"),
	        R"pydoc(
Adds a Duration to a Time

Note: `self` remains unchanged.

Args:
    d (Duration): the Duration to add

Returns:
    Time: a new Time representing `self + d`

Raises:
    RuntimeError: if the resulting Time is not representable.
)pydoc"
	    )
	    .def(
	        "Round",
	        &fort::Time::Round,
	        py::arg("d"),
	        R"pydoc(
Rounds a Time to the closest Duration

Rounds a Time to the closest Duration. Only multiple of seconds and
power of 10 of Nanosecond smaller than a second are supported.

Args:
    d (Duration): a multiple of a second or a power of 10 of a
        nanosecond.

Returns:
    Time: a new Time rounded to d

Raises:
    ValueError: if d is incorrect
)pydoc"
	    )
	    .def(
	        "Reminder",
	        &fort::Time::Reminder,
	        py::arg("d"),
	        R"pydoc(
Gets the remaider Duration of self.Round(d)

Args:
    d (Duration): the duration to round to.

Returns:
    Duration: the reminder of :meth:`Round(d)`
)pydoc"
	    )
	    .def(
	        "After",
	        &fort::Time::After,
	        py::arg("other"),
	        R"pydoc(
Tests if this Time is after other

Similar to `self > other`. `__gt__` operator is also provided.

Args:
    other (Time): the other Time to test.

Returns:
    bool:  result of `self > other`
)pydoc"
	    )
	    .def(
	        "Before",
	        &fort::Time::Before,
	        py::arg("other"),
	        R"pydoc(
Tests if this Time is before other

Similar to `self < other`. `__lt__` operator is also provided.

Args:
    other (Time): the other Time to test.

Returns:
    bool:  result of `self < other`
)pydoc"
	    )
	    .def(
	        "Equals",
	        &fort::Time::Equals,
	        py::arg("other"),
	        R"pydoc(
Tests if this Time is exactly equal to other

Similar to `self == other`. `__eq__` operator is also provided.

Args:
    other (Time): the other Time to test.

Returns:
    bool:  result of `self == other`
)pydoc"
	    )
	    .def(
	        "IsForever",
	        &fort::Time::IsForever,
	        R"pydoc(
Tests if this Time is +∞

Returns:
    bool: ``True`` if this time is :meth:`Time.Forever`
)pydoc"
	    )
	    .def(
	        "IsSinceEver",
	        &fort::Time::IsSinceEver,
	        R"pydoc(
Tests if this Time is -∞

Returns:
    bool: ``True`` if this time is :meth:`Time.SinceEver`
)pydoc"
	    )
	    .def(
	        "IsInfinite",
	        &fort::Time::IsInfinite,
	        R"pydoc(
Tests if this Time is + or - ∞

Returns:
    bool: ``True`` if this time is :meth:`Time.SinceEver` or
    :meth:`Time.Forever`
)pydoc"
	    )
	    .def(
	        "Sub",
	        &fort::Time::Sub,
	        R"pydoc(
Measure the Duration between two Time

Similar to `self - other`. `__sub__` operator is also provided.

Args:
    other (Time): the other Time to substract.

Returns:
    bool:  result of `self - other`

Raises:
    Error: if the result would not fit in a Duration (i.e. if one
        of the :meth:`Time.IsInfinite`)
)pydoc"
	    )
	    .def("__str__", &fort::Time::Format)
	    .def("__repr__", &fort::Time::DebugString)
	    .def(py::self == py::self)
	    .def(py::self < py::self)
	    .def(py::self <= py::self)
	    .def(py::self > py::self)
	    .def(py::self >= py::self)
	    .def(
	        "__sub__",
	        [](const fort::Time &a, const fort::Time &b) -> fort::Duration {
		        return a.Sub(b);
	        },
	        py::is_operator()
	    )
	    .def(
	        "__add__",
	        [](const fort::Time &t, const fort::Duration &d) -> fort::Time {
		        return t.Add(d);
	        },
	        py::is_operator()
	    )
	    .def(
	        "__add__",
	        [](const fort::Time &t, const fort::Duration &d) -> fort::Time {
		        return t.Add(-1 * d);
	        },
	        py::is_operator()
	    );

	py::implicitly_convertible<double, fort::Time>();
	py::implicitly_convertible<
	    std::chrono::system_clock::time_point,
	    fort::Time>();
}
