Performing Queries
==================

The main purpose of **fort-myrmidon** is to allow user to perform
*queries* [#query]_, i.e. computations on the whole set of tracking
data. These queries can only be performed once Ant are properly
defined and in some case (such as for collisions and interactions)
their wanted virtual shape are defined.

.. note::

   Here the python and C++ API quite diverges from the R API. While
   the two first reports :obj:`list` or :obj:`std::vector` of object,
   the latter only reports :obj:`data.frame` or :obj:`slist` of
   :obj:`data.frame` with only native R types.

Instantaneous Queries
*********************

Identifying Frames
------------------

The simplest query that could be performed on a tracking data set is
*identifying frames* [#identifyFrames]_. It is a simple
translation of the raw tracking data from the Identification. It
simply returns for each acquired video frame the position and
orientation of each ant, taking into account the geometry relation
between a tag position and orientation and the virtual ant desired
position and orientation.

This query return a list of :obj:`IdentifiedFrame` [#identifiedFrame]_.

.. code-block:: python

   # Python
   frames = fm.Query.IdentifyFrames(e)
   for f in frames:
       print("Got Frame at %s in space %d" % (f.FrameTime,f.Space))
       print("AntID, X, Y, Angle, Zone")
       print(f.Positions)

.. code-block:: R

   # R
   d = fmQueryIdentifyFrames(e);
   d$frames # a data frame summarizing for each frame its dimension, space and acquisition time
   d$positions # a list of data frame with each ant position.

.. code-block:: c++

   // C++
   fort::myrmidon::IdentifyFramesQueryArgs args;
   // modify here default args;
   std::vector<fort::myrmidon::IdentifiedFrame::Ptr> frames;
   fort::myrmidon::Query::IdentifyFrame(e,frames,args);

   for ( const auto & f : frames ) {
       std::cerr << "Got frame at " << f->FrameTime
                 << " in space " << f->Space << std::endl
                 << "AntID, X, Y, Angle, Zone" << std::endl
                 << f->Positions << std::endl;
   }


The main parameters for this query are:

 * *the starting and ending time of the query*: in order to select only
   a range of the acquired tracking data. Since dataset tends to grow
   larger than the available RAM, it is important to only work on
   part of the dataset at a time to keep the memory usage low enough.
 * *Zone reporting*: Choose if the actual zone the ant lies should be
   reported, see :doc:`space_zoning`


Collision Detection
-------------------

Instead of just reporting the ant position, **fort-myrmidon** can also
report any collision detection between ant shapes by *colliding
frames* [#collideFrames]_.

This query returns a collection of :obj:`IdentifiedFrame` and
:obj:`CollisionFrame` [#collisionFrame]_.

.. code-block:: python

   #python
   frames = fm.Query.CollideFrame(e)
   for identified,collided in frames:
       print("Collisions at $s in Space %d." % ( collided.FrameTime, collided.Space) )
       for c in collided.Collisions:
           print("Ants %s and %s collides." % ( fm.FormatAntID(c.IDs[0]), fm.FormatAntID(c.IDs[1])))
		   print(c.Types)

.. code-block:: R

   #R
   d = fmQueryCollideFrames(e)
   d$frames # a data.frame that summarizes frame time, dimensions and space
   d$positions # a list of data.frame with the ant position for each frame
   d$collisions # a data.frame of all collisions in the selected time range. the field frame_row_index refers to the row in frames or the index in positions.

.. code-block:: c++

   //C++
   auto args = fort::myrmidon::Query::CollideFramesArgs();
   // modify here default arguments;
   std::vector<std::pair<fort::myrmidon::IdentifiedFrame::Ptr,
                         fort::myrmidon::CollisionFrame::Ptr>> frames;
   fort::myrmidon::CollideFrames(e,frames,args);

   for ( const auto & [identified,collided] : frames ) {
       std::cerr << "Collisions at " << collided->FrameTime
                 << " in space " << collided->Space
                 << ": " << std::endl;
       for ( const auto & c : collided->Collisions ) {
           std::cerr << "Ants " << fort::myrmidon::FormatAntID(c.IDs.first)
                     << " and " << fort::myrmidon::FormatAntID(c.IDs.second)
                     << " collides." << std::endl
                     << c.Types << std::endl;
       }
   }

The parameters are the same than for `Identifying Frames`_, excepted
than zone are always reported. Indeed zoning is used to prune
undesired collisions as described in :doc:`space_zoning`.

.. note::

   By convention, the order of the :obj:`AntID` in the :obj:`IDs` field
   is always sorted from the smallest to the highest to ensure uniqueness.

Interaction Types
~~~~~~~~~~~~~~~~~

As capsules are typed, for each collision the :obj:`InteractionTypes`
[#interactionTypes]_ are reported. In Python and C++ it is a Nx2
matrix with each column representing the type of each ant which are
colliding with the other one.

For example the matrix:

.. math::

   \begin{bmatrix}
   1 & 1\\
   2 & 1
   \end{bmatrix}

Means body type **1** for the second Ant touches both body type **1**
and **2** of the first ant, but the body type **2** of the second ant
collide with no other parts.

In R, this matrix will be returned as a character: ``'1-1,2-1'``.

Time-spanning Queries
*********************

The next two main queries, instead of report data for a single time,
reports data over a time range. This time range is not predetermined
and is found as the tracking data is parsed: Indeed as it is not
possible to avoid detection losses, each of these queries accepts a
*maximumGap* parameter. If tracking is lost for a duration larger than
this parameter, the considered result (i.e. the trajectory or
interaction) is considered done, and any latter detection or collision
found later will become part of a new result.

.. note::

   If this behavior of segmenting results according to the
   *maximumGap* parameter wants to be avoided, one can set it to a
   large value, e.g. one year.

Ant Trajectories
----------------

Instead of reporting instantaneous detection, *computing ant
trajectories* [#computeAntTrajectories]_ will report for each
individual the time ranges where it was continuously detected,
accordingly to the *maximumGap* parameter.

Results are a list of :obj:`AntTrajectory` [#antTrajectory]_.

.. code-block:: python

   # Python
   trajectories = fm.Query.ComputeAntTrajectories(e,maximumGap = 500 * fm.Duration.Millisecond)
   for t in trajectories:
       print("Ant %s Trajectory from %s to %s in space %d." % (fm.FormatAntID(t.Ant),t.Start,t.End(),t.Space))
       print(t.Positions)


.. code-block:: R

   # R
   d = fmQueryComputeAntTrajectories(e, maximumGap = fmMillisecond(500))
   d$trajectories_summary # a data.frame with the ant, space and starting time of each trajectories
   d$trajectories # a list of data.frame with each ant position for each trajectories

.. code-block:: c++

   // C++
   auto args = fort::myrmidon::Query::ComputeAntTrajectoryArgs();
   // modify here default arguments
   std::vector<fort::myrmidon::AntTrajectory::Ptr> trajectories;
   fort::myrmidon::Query::ComputeAntTrajectories(e,trajectories,args);

As I the case of `Identifying Frames`_, the user can choose if the ant
zone should be reported with the *computeZone* argument.

Ant Interactions
----------------

Instead of just looking at iNstantaneous collision, one can *compute
ant interactions* [#computeAntInteractions]_, i.e. time ranges where ant
collides.

Both :obj:`AntTrajectory` and :obj:`AntInteraction` [#antInteraction]_
may be reported.

.. code-block:: python

   # Python
   trajectories,interactions = fm.Query.ComputeAntInteractions(e)
   for i in interactions:
       print("Ant %s and %s interacts from %s to %s" % (fm.FormatAntID(i.IDs[0]),fm.FormatAntID(i.IDs[1]),i.Start,i.End))


.. code-block:: R

   # R
   d = fmQueryComputeAntInteractions(e)
   d$trajectory_summaries # the summary of trajectory like for fmQueryComputeAntTrajectories
   d$trajectories # the trajectory positions like for fmQueryComputeAntTrajectories
   d$interactions # a data.frame with all interactions.

.. code-block:: c++

   // C++
   auto args = fort::myrmidon::Query::ComputeAntInteractionArgs();
   // modify here default arguments;
   std::vector<fort::myrmidon::AntTrajectory::Ptr> trajectories;
   std::vector<fort::myrmidon::AntInteraction::Ptr> interactions;
   fort::myrmidon::Query::ComputeAntInteractions(e,trajectories,interactions,args);

   for ( const auto & i : interactions ) {
       std::cerr << "Ant " << fort::myrmidon::FormatAntID(i->IDs.first)
                 << " and " << fort::myrmidon::FormatAntID(i->IDs.second)
                 << " interacts from " << i->Start
                 << " to " << i->End
                 << std::endl;
   }

Full vs Summarized Trajectory Report
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As shown by the API, by default, the :obj:`ComputeAntInteractions`
query returns both interactions and trajectories. In that case each
:obj:`AntInteraction` object contains a pair of
:obj:`AntTrajectorySegment` [#antTrajectorySegment]_ that points to a
part of the :obj:`AntTrajectory` corresponding to the current
interaction.

Alternatively to simplify the results and to save memory, one can
disable the report of the full trajectory with the
*reportFullTrajectories* parameter. In that case the list of returned
trajectory will be empty, and instead :obj:`AntTrajectorySummary`
[#antTrajectorySummary]_ will be reported. These object contains the
mean position of the ant, and the list of zone traversed during the
interaction.


The case of R is a bit more complex. If full trajectories are
reported, the ``$interactions`` :obj:`data.frame` will report the
``$trajectories`` index and first and last rows corresponding to the
trajectory segment. Otherwise, only a single :obj:`data.frame` is
returned, with the mean position and zone for each ant.

Building Complex Queries with Matchers
--------------------------------------

Time-spanning queries accepts a *matcher* [#matcher]_ parameter that
could be used to customize the behavior of these queries.


For example one can select only `1-1` interactions type:

.. code-block:: python

   # Python
   trajectories,interactions = fm.Query.ComputeAntInteractions(e,matcher = fm.Matcher.InteractionType(1,1))

.. code-block:: R

   # R
   d = fmQueryComputeAntInteractions(e,matcher = fmMatcherInteractionType(1,1))

.. code-block:: C++

   // C++
   auto args = fort::myrmidon::ComputeAntInteractionsArgs();
   args.Matcher = fort::myrmidon::Matcher::InteractionType(1,1);
   std::vector<fort::myrmidon::AntTrajectory::Ptr> trajectories;
   std::vector<fort::myrmidon::AntInteraction::Ptr> interactions;
   fort::myrmidon::Query::ComputeAntInteractions(e,trajectories,interactions,args);

Or one can prune out interactions were one of the ant have a large
displacement over a given duration:

.. code-block:: python

   # Python
   trajectories,interactions = fm.Query.ComputeAntInteractions(e,matcher = fm.Matcher.AntDisplacement(under = 100, minimumGap = 500 * fm.Duration.Millisecond))

.. code-block:: R

   # R
   d = fmQueryComputeAntInteractions(e,matcher = fmMatcherAntDisplacement(under = 100, minimumGap = fmMillisecond(500)))

.. code-block:: C++

   // C++
   auto args = fort::myrmidon::ComputeAntInteractionsArgs();
   args.Matcher = fort::myrmidon::Matcher::AntDisplacement(100,500 * fort::Duration::Millisecond);
   std::vector<fort::myrmidon::AntTrajectory::Ptr> trajectories;
   std::vector<fort::myrmidon::AntInteraction::Ptr> interactions;
   fort::myrmidon::Query::ComputeAntInteractions(e,trajectories,interactions,args);

More complex matcher can be combined together. Here we only look at
interactions with ant ``001`` and the two above matcher combined:

.. code-block:: python

   # Python
   m = fm.Matcher.And(fm.Matcher.AntID(1),fm.Matcher.InteractionType(1,1),fm.Matcher.AntDisplacement(100,500 * fm.Duration.Millisecond))
   trajectories,interactions = fm.Query.ComputeAntInteractions(e,matcher = m)

.. code-block:: R

   # R
   m <- fmMatcherAnd(list(fmMatcherAntID(1),fmMatcherInteractionType(1,1),fmMatcherAntDisplacement(100,fmMillisecond(500))))
   d <- fmQueryComputeAntInteractions(e,matcher = m)

.. code-block:: c++

   // C++
   auto args = fort::myrmidon::ComputeAntInteractionsArgs();
   args.Matcher = fort::myrmidon::Matcher::And({fort::myrmidon::Matcher::AntID(1),
                                                fort::myrmidon::Matcher::InteractionType(1,1),
												fort::myrmidon::Matcher::AntDisplacement(100,fmMillisecond(500))});
   std::vector<fort::myrmidon::AntTrajectory::Ptr> trajectories;
   std::vector<fort::myrmidon::AntInteraction::Ptr> interactions;
   fort::myrmidon::Query::ComputeAntInteractions(e,trajectories,interactions,args);

More matchers can be found in the :obj:`Matcher` class definition
[#matcher]_.

Miscellaneous Queries
*********************

TODO


.. [#query] Python: :py:class:`fort_myrmidon.Query`, R: :obj:`fmQuery`, C++: :cpp:class:`fort::myrmidon::Query`
.. [#identifyFrames] Python: :py:meth:`fort_myrmidon.Query.IdentifyFrames`, R: :obj:`fmQueryIdentifyFrames()`, C++: :cpp:func:`fort::myrmidon::Query::IdentifyFrames`
.. [#collideFrames] Python: :py:meth:`fort_myrmidon.Query.CollideFrames`, R: :obj:`fmQueryCollideFrames()`, C++: :cpp:func:`fort::myrmidon::Query::CollideFrames`
.. [#computeAntTrajectories] Python: :py:meth:`fort_myrmidon.Query.ComputeAntTrajectories`, R: :obj:`fmQueryComputeAntTrajectories()`, C++: :cpp:func:`fort::myrmidon::Query::ComputeAntTrajectories`
.. [#computeAntInteractions] Python: :py:meth:`fort_myrmidon.Query.ComputeAntInteractions`, R: :obj:`fmQueryComputeAntInteractions()`, C++: :cpp:func:`fort::myrmidon::Query::ComputeAntInteractions`
.. [#identifiedFrame] Python: :py:class:`fort_myrmidon.IdentifiedFrame`, R: see return value of :obj:`fmQueryIdentifyFrames()`, C++: :cpp:class:`fort::myrmidon::IdentifiedFrame`
.. [#collisionFrame] Python: :py:class:`fort_myrmidon.CollisionFrame`, R: see return value of :obj:`fmQueryCollideFrames()`, C++: :cpp:class:`fort::myrmidon::CollisionFrame`
.. [#antTrajectory] Python: :py:class:`fort_myrmidon.AntTrajectory`, R: see return value of :obj:`fmQueryComputeAntTrajectories()`, C++: :cpp:class:`fort::myrmidon::AntTrajectory`
.. [#antInteraction] Python: :py:class:`fort_myrmidon.AntInteraction`, R: see return value of :obj:`fmQueryComputeAntInteractions()`, C++: :cpp:class:`fort::myrmidon::AntInteraction`
.. [#antTrajectorySegment] Python: :py:class:`fort_myrmidon.AntTrajectorySegment`, R: see return value of :obj:`fmQueryComputeAntInteractions()`, C++: :cpp:class:`fort::myrmidon::AntTrajectorySegment`
.. [#antTrajectorySummary] Python: :py:class:`fort_myrmidon.AntTrajectorySummary`, R: see return value of :obj:`fmQueryComputeAntInteractions()`, C++: :cpp:class:`fort::myrmidon::AntTrajectorySummary`
.. [#interactionTypes] Python: :py:attr:`fort_myrmidon.Collision.Types`, R: N.A., C++: :cpp:type:`fort::myrmidon::InteractionTypes`
.. [#matcher] Python: :py:class:`fort_myrmidon.Matcher`, R: :obj:`fmMatcher`, C++: :cpp:class:`fort::myrmidon::Matcher`
