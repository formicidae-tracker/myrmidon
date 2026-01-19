Matching Video and Tracking Data
================================

After the analysis of the tracking data, one may want to display the
analysis of the tracking over saved video sequence.

There are a few technical difficulties to ensure that the saved video
file frames matches the tracking data:

* Tracking data frames or query results without an associated video
  frame should be discarded.
* Sequences over multiple video files must be merged together
* The rare case a video frame exist but no tracking data report exist
  ( either a presence or an absence ) should be handled.

**fort-myrmidon** provides an API to handle these difficulties for the
 user. The API decompose this task in three steps.

.. note::

   For the current version, the R api does not provide access to any
   function described here. Indeed it is quite hard to use the R
   opencv bindings to read,modify and write a video file, which is the
   main purpose of the API described here.

1. Find Video Segments Between Two Times
****************************************

First we must *find the VideoSegments* [#findVideoSegments]_ between
two times. This will return a *list of VideoSegment* [#videoSegmentList]_
which will describes the list of video files and ranges of frames in
this files that were acquired in the wanted time range. Indeed, since
the FORT system saves video by default in two hours longs file, it is
possible that a wanted time range spans over multiple file.

.. note::

   It is a query as the raw tracking data must be parsed again to
   obtain the acquisition time of each video frame. Therefore it is
   quite inefficient to run the query below over a large time range.

.. code-block:: python

   # Python
   start = fm.Time.Parse('2019-11-02T20:00:00.000Z')
   end = start.Add(30 * fm.Duration.Second)
   segments = fm.Query.FindVideoSegments(e, space = 1, start = start, end = end)

.. code-block:: c++

   // C++
   #include <fort/myrmidon/Query.hpp>
   #include <fort/myrmidon/Video.hpp>

   auto start = fort::Time::Parse("2019-11-02T20:00:00.000Z");
   auto end = start.Add(30 * fort::Duration::Second);
   VideoSegment::List segments;
   fort::myrmidon::Query::FindVideoSegments(e,segments,1,start,end);

2. Match Query Results with Video Segments
******************************************

The *list of VideoSegment* acquired will only contains, for each video
frame were this information is available, the acquisition time in a
*VideoFrameData* [#videoFrameData]_ object. This object contains other
variable to hold any kind over query result, however those will only
be populated once we *matched query result with the video segments*
[#videoSegmentMatch]_

Please note in the example below, that the match operation also takes
a *list of VideoSegment*.

.. code-block:: python

   # Python
   trajectories,interactions = fm.Query.ComputeAntInteractions(e,reportFullTrajectories = True)

   # must be call once for each containers
   fm.VideoSegment.Match(segments,trajectories)
   fm.VideoSegment.Match(segments,interactions)

.. code-block:: c++

   // C++
   std::vector<AntTrajectory::Ptr> trajectories;
   std::vector<AntInteraction::Ptr> interactions;
   fort::myrmidon::Query::ComputeAntInteractions(e,trajectories,interactions);

   // must be called once over each containers
   fort::myrmidon::VideoSegment::Match(segments,trajectories.begin(),trajectories.end());
   fort::myrmidon::VideoSegment::Match(segments,interactions.begin(),interactions.end());

Matching User Specific Data
---------------------------

It may be planed for the future to match any kind of timed object, but
it is not part yet of the public API.

3. Iterate Over All Video Frames in the VideoSequence
*****************************************************

The last steps will be to iterate over all video frames. Each API
provides utility function or object for this purpose.

.. warning::

   Data contained in the query results is kept as the original
   tracking resolution, but it is very likely that the FORT system has
   saved the video file at a much lower resolution (1080p by
   default). So while drawing, you must take care to scale
   the geometric data accordingly.

Python
------

A :py:class:`fort_myrmidon.VideoSequence` context manager is
provided to iterate over the *VideoSegments*.

.. code-block:: python

   def DrawInteractions(frame,data):
       if data.Empty():
           return
       for interaction in data.Interactions:
           # draw here the interaction on frame using opencv
           # functions.
           pass
       # you may want to display the frame or write it to another file

   with fm.VideoSequence(segments) as sequence:
       for frame,data in sequence:
           DrawInteractions(frame,data)

C++
---

A :cpp:func:`fort::myrmidon::VideoSequence::ForEach` method is
provided.

.. code-block:: c++

   auto drawInteractions
       = [](cv::Mat & frame,
            const fort::myrmidon::VideoFrameData & data) {
                if ( data.Empty() ) {
                    return;
                }
                for ( const auto & i : data.Interactions ) {
                    //Use here opencv method to draw on frame
                }
                // You may want to display or write the frame to another file.
             };

   fort::myrmidon::VideoSequence::ForEach(segments,
                                          drawInteractions);


.. [#VideoSegmentList] Python: :py:class:`fort_myrmidon.VideoSegmentList`, R: N.A., C++: :cpp:type:`fort::myrmidon::VideoSegment::List`
.. [#VideoFrameData] Python: :py:class:`fort_myrmidon.VideoFrameData`, R: N.A., C++: :cpp:class:`fort::myrmidon::VideoFrameData`
.. [#findVideoSegments] Python: :py:meth:`fort_myrmidon.Query.FindVideoSegments`, R: N.A., C++: :cpp:func:`fort::myrmidon::Query::FindVideoSegments`
.. [#videoSegmentMatch] Python: :py:meth:`fort_myrmidon.VideoSegment.Match`, R: N.A., C++: :cpp:func:`fort::myrmidon::VideoSegment::Match`
