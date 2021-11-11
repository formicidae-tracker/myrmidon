.. time:

Time Representation
-------------------

**fort-myrmidon** uses its own classes to represent
:cpp:class:`fort::Time` and :cpp:class:`fort::Duration`. Their python
equivalents are :py:class:`py_fort_myrmidon.Time` and
:py:class:`py_fort_myrmidon.Duration`. These have been developped to
handle the following features, which are not available with
**std::chrono**, or python and R usual time representations:

* Microseconds precision (actually up to nanosecond).
* Robustness to wall clock reset, that could naturally happen when the
  system clock is reset by **ntp** on windows, or when a leap second
  occurs. Like the go time package, it uses a mix of wall and
  monotonic clocks (issued by the camera/framegrabber). It support
  multiple monotonic clock sources.
* Supports for representing -∞ and +∞ time concepts.

C++
===

Example
+++++++

:cpp:class:`fort::Time` and :cpp:class:`fort::Duration` provides
operators for manipulating duration naturally:

.. code-block:: c++

   #include <iostream>
   #include <fort/time/Time.hpp>

   auto start = fort::Time::Now();
   // do some operation
   ellapsed = fort::Time::Now().Sub(start);
   std::cerr << "operation took: " << ellapsed << std::endl;

   //always true:
   start < fort::Time::Forever();
   start > fort::Time::SinceEver();
   start < start.Add(5 * fort::Duration::Second + 2 * fort::Duration::Millisecond);


Python
======

Due to python and R limitation, we cannot implictly convert native
time object of these object from and to :cpp:class:`fort::Time` and
:cpp:class:`fort::Duration`. Indeed:

* python :py:func:`time.time` representation and R **POSIXct** as a **double**
  precision floating point number of second ellapsed from the system
  epoch, is only guaranted 10us precision for time arround the
  year 2020. However the framegrabber used in the FORT issues time
  which are precise to the microsecond. By converting
  :cpp:class:`fort::Time` objects to these native representation,
  rounding error may occurs and may change the behavior of certain
  queries.
* R only supports 32-bit integer, that may overflow for duration of more
  than 2s when expressed in nanoseconds.

Therefore the c++ classes :cpp:class:`fort::Time` and
:cpp:class:`fort::Duration` are binded to python and R. However the
two bindings differs a bit from the pure c++ implementation.

Python convertion to and from **time.Time**
+++++++++++++++++++++++++++++++++++++++++++

:py:class:`py_fort_myrmidon.Time` objects can be created from and converted to a float
number of seconds from the epoch, as returned by :py:func:`time.time` or
:py:meth:`datetime.datetime.timestamp`, with:

* :py:meth:`py_fort_myrmidon.Time.__init__`
* :py:meth:`py_fort_myrmidon.Time.ToTimestamp`

.. note::
   These conversions have a guaranted precision of only +/-10 us.

Python convertion to and from **datetime.datetime**
+++++++++++++++++++++++++++++++++++++++++++++++++++
:py:class:`py_fort_myrmidon.Time` objects can be created from and
converted to :class:`datetime.datetime`:

* :py:meth:`py_fort_myrmidon.Time.__init__`
* :py:meth:`py_fort_myrmidon.Time.ToDateTime`

However as cpython and c++17 lacks supports for timezone, these
:py:class:`datetime.datetime` objects are treated as naïve objects,
i.e. object wihtout an associated timezone, and assumed to be in local
time. On the contrary, :cpp:class:`fort::Time` uses only UTC time. It
means that:

* :py:meth:`py_fort_myrmidon.Time.ToDateTime`, will return
  values that differs from one can read from the
  :py:meth:`py_fort_myrmidon.Time.__str__` if not careful to
  take into account the local timezone.

* before passing any :py:class:`datetime.datetime` object to
:py:meth:`py_fort_myrmidon.Time.__init__`, one must ensure they are
converted to localtime first. One could use
:py:meth:`datetime.datetime.astimezone` with ``tz=None`` to do that.

Example
+++++++

:py:class:`py_fort_myrmidon.Time` and
:py:class:`py_fort_myrmidon.Duration` provides overloaded operator
that gives a natural feeling on time manipulation.

.. code-block:: python

   import py_fort_myrmidon as fm

   start = fm.Time.Now()
   somefunction()
   ellapsed = fm.Time.Now() - start # equivalent to ellapsed = fm.Time.Now().Sub(start)

   later = start + 5 * fm.Duration.Second # equivalent to later = start.Add(5 * fm.Duration.Second)

   # we can compare Time and Duration
   later > start # equivalent to later.After(start)
   later < start # equivalent to later.Before(start)
   later == start # equivalent to later.Equals(start)
   later >= start
   later <= start

   # always true
   start > fm.Time.SinceEver()

R
===

Conversion to and from **POSIXct**
++++++++++++++++++++++++++++++++++

Example
+++++++


.. code-block:: R

   library(FortMyrmidon)

   start <- fmTimeNow()
   # do some work
   ellapsed <- fmTimeNow()$sub(start_

   later <- start$add(fmSecond(5.0))

   # we can compare time objects
   later > start
   later < start
   later == start
   later <= start
   later >= start

   # we can access +/- ∞ values
   later >= fmTimeForever() # always FALSE
   later > fmTimeSinceEver() # always TRUE
