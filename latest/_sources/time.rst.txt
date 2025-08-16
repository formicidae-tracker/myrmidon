.. time:

Time Representation
-------------------

**fort-myrmidon** uses its own classes to represent
:cpp:class:`fort::Time` and :cpp:class:`fort::Duration`. These have
been developped to handle the following features, which are not
available with most time object representation:

* Microseconds precision (actually up to nanosecond).
* Robustness to wall clock reset, that could naturally happen when the
  system clock is reset by **ntp** on windows, or when a leap second
  occurs. Like the go time package, it uses a mix of wall and
  monotonic clocks (issued by the camera/framegrabber). It support
  multiple monotonic clock sources ( when tracking data is acquired
  concurrently by multiple clock source).
* Supports for representing -∞ and +∞ time concepts.


Due to python and R limitation, we cannot implictly convert native
time object of these object from and to :cpp:class:`fort::Time` and
:cpp:class:`fort::Duration`. Indeed:

* python :py:func:`time.time` representation and R **POSIXct** as a
  **double** precision floating point number of second ellapsed
  from the system epoch, is only guaranted 10us precision for time
  arround the year 2020. However the framegrabber used in the FORT
  issues time which are precise to the microsecond. By converting
  :cpp:class:`fort::Time` objects to these native representation,
  rounding error may occurs and may change the behavior of certain
  queries.
* R only supports 32-bit integer, that may overflow for duration of
  more than 2s when expressed in nanoseconds.

.. note::

   For the aforementioned reasons, the c++ classes
   :cpp:class:`fort::Time` and :cpp:class:`fort::Duration` are
   **binded** to python and R. It means that you would find
   :py:class:`fort_myrmidon.Time`,
   :py:class:`fort_myrmidon.Duration`, :obj:`fmTime`,
   :obj:`fmDuration` objects, which are used instead of the native
   time object. The following section describes the perticularity of
   casting native time object to and from the **fort-myrmidon**
   object.


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

Python convertion to and from **time.Time**
+++++++++++++++++++++++++++++++++++++++++++

:py:class:`fort_myrmidon.Time` objects can be created from and converted to a float
number of seconds from the epoch, as returned by :py:func:`time.time` or
:py:meth:`datetime.datetime.timestamp`, with:

* :py:meth:`fort_myrmidon.Time.__init__`

  .. code-block:: python

	 # equivalent but less precise than m.Time.Now()
	 fm.Time(time.time())

* :py:meth:`fort_myrmidon.Time.ToTimestamp`

  .. code-block:: python

	 #equivalent to time.time()
	 fm.Time.Now().ToTimestamp()

.. note::
   These conversions have a guaranted precision of only +/-10 us.

Python convertion to and from **datetime.datetime**
+++++++++++++++++++++++++++++++++++++++++++++++++++

:py:class:`fort_myrmidon.Time` objects can be created from and
converted to :class:`datetime.datetime`:

* :py:meth:`fort_myrmidon.Time.FromDateTime`

  .. code-block:: python

	 # equivalent to m.Time.Now()
	 m.Time.FromDateTime(datetime.utcnow()) # use a naïve object
	 m.Time.FromDateTime(datetime.now()) # transform it to a naïve object

* :py:meth:`fort_myrmidon.Time.ToDateTime`

    .. code-block:: python

	 # equivalent to datetime.now()
	 m.Time.Now().ToDateTime() # returns the current time with local timezone

.. note::

   In previous version, exlicit transformation to naïve datetime object was
   required. Now the bindings take care of any needed transformation.

Example
+++++++

:py:class:`fort_myrmidon.Time` and
:py:class:`fort_myrmidon.Duration` provides overloaded operator
that gives a natural feeling on time manipulation.

.. code-block:: python

   import fort_myrmidon as fm

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

R represents times as :obj:`POSIXct` objects, i.e. a numerical number
of seconds since the system epoch, marked with the `POSIXct`
class. One can use the following function to convert to and from a
:obj:`fmTime` object:

* :obj:`fmTimeCreate()` can take a number of second since epoch,
  simply use the :obj:`POSIXct` object to convert it to a :obj:`fmTime`.

  .. code-block:: R

	 # equivalent to fmTimeParse("2019-11-02T23:03:04Z")
	 fmTimeCreate(as.POSIXct("2019/11/02 23:03:04",tz="UTC"))

* :obj:`fmTime$asPOSIXct()` will returns a :obj:`fmTime` as a :obj:`POSIXct` object.

  .. code-block:: R

	 # equivalent to:
	 # d <- as.POSIXct("2019/11/02 23:03:04",tz="UTC")
	 # attr(d,"tzone") <- NULL # fmTime$asPOSIXct() reports no timezone
	 d <- fmTimeParse("2019-11-02T23:03:04Z")$asPOSIXct()


Example
+++++++

Here is an example of the :obj:`fmTime` and :obj:`fmDuration` R API:

.. code-block:: R

   library(FortMyrmidon)

   start <- fmTimeNow()
   # do some work
   ellapsed <- fmTimeNow()$sub(start)

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
