Linking Tracking Data
=====================

.. note::
   All examples assumes the **fort_myrmidon** module to be included as:

   .. code-block:: python

	  import fort_myrmidon as fm


When working with **fort-myrmidon** the first steps is to link a
``.myrmidon`` file with some tracking data, generally created with
**fort-leto** from the FORT system. These consist of folder with the
tracking data, alongside video stream and close-up files of the
detected tags.

In **fort-myrmidon** these `Tracking Data Directories (TDD)` must be
assigned to a `Space` [#space]_. A `Space` is a 2D
Cartesian space on its own, meaning that two coordinates from
different `Space` should never be compared. This is useful with
experiment with a different camera for the nest and foraging area,
where positions should not be mixed together.


Manipulating `Experiment` files
+++++++++++++++++++++++++++++++

In order to do so, one should first `create` [#experimentCreate]_ a
new `Experiment` [#experiment]_ associated with a file path. Then this
file can then be `saved` [#experimentSave]_. While saving, the
target file can be renamed, but must remain in the same directory to
keep the relative links working.

.. code-block:: python

   # python
   e = fm.Experiment('<exp_path>.myrmidon') # no file saved yet
   e.Save('<exp_path>.myrmidon') # file now exists


.. code-block:: R

   # R
   e <- fmExperimentCreate('<exp_path>.myrmidon') # no file created yet
   e$save('<exp_path>.myrmidon') # file now exists

.. code-block:: c++

   // C++
   #include <fort/myrmidon/Experiment.hpp>

   // will not create file yet
   auto e = fort::myrmidon::Experiment::Create("<exp_path>.myrmidon");
   e->Save("<exp_path>.myrmidon"); // file now exists


.. warning::

   If you modify an `experiment` object, modifications are not
   reflected automatically on the file system, you must `save` them explicitly.


Defining `Spaces`
+++++++++++++++++

The next step is to `create a Space` [#createSpace]_ in that
`experiment`. This Space has an ID and a `name` that will be used to
report coordinates. There are no restriction on the `Space` name.

.. code-block:: python

   # python
   s = e.CreateSpace(name = "nest")
   print("Space '%s' has ID: %d" %(s.Name,s.ID))
   # outputs: Space 'nest' has ID: 1

.. code-block:: R

   # R
   s <- e$createSpace('nest')
   printf("Space '%s' has ID: %d\n",s$name,s$ID)
   # outputs: Space 'nest' has ID: 1

.. code-block:: c++

   // C++
   #include <fort/myrmidon/Experiment.hpp>

   auto s = e->CreateSpace("nest");
   std::cerr << "Space '" << s->Name() << "' has ID: " << s->ID() << std::endl;
   // outputs: Space 'nest' has ID: 1

.. note::

   For efficient data report, many objects such `Space` or `Ant` [#ant]_ are
   uniquely identified with a unique Identifier for that
   type. **fort-myrmidon** uses Identifier starting at 1 and growing,
   and search to group these identifiers together for a given type, to avoid
   missing identifier in a sequence. You cannot choose explicitly the
   value for these IDs, excepting by defining objects in the same order.


Linking Tracking Data Directories
+++++++++++++++++++++++++++++++++

Finally, we can `add a Tracking Data Directory (TDD)` [#addTDD]_ some data to
the experiment. This will return to us an **URI** used to identify
this particular TDD, especially to eventually `remove` [#removeTDD]_
them or to `report` [#reportStats]_ tracking data statistics they contain.

.. code-block:: python

   #python
   tddURI = e.AddTrackingDataDirectory(s.ID,"my_exp.0000")
   # e.RemoveTrackingDataDirectory(tddURI)

.. code-block:: R

   #R
   tddURI <- e$addTrackingDataDirectory(s$ID,"my_exp.0000")
   # e$removeTrackingDataDirectory(tddURI)

.. code-block:: c++

   //C++
   #include <fort/myrmidon/Experiment.hpp>

   auto tddURI = e->AddTrackingDataDirectory(s->ID(),"my_exp.0000")
   // e->RemoveTrackingDataDirectory(tddURI)

TDD consistency
+++++++++++++++

Internally, **fort-myrmidon** ensure the following invariant and will raise exception if :

* A TDD is assigned to multiple spaces.
* In a space, two TDDs contains data overlapping in space
* If all the TDDs are not using the same tag family.



.. [#experiment] Python: :py:class:`fort_myrmidon.Experiment`, R: :obj:`fmExperiment`, C++: :cpp:class:`fort::myrmidon::Experiment`
.. [#space] Python: :py:class:`fort_myrmidon.Space`, R: :obj:`fmSpace`, C++: :cpp:class:`fort::myrmidon::Space`
.. [#ant] Python: :py:class:`fort_myrmidon.Ant`, R: :obj:`fmAnt`, C++: :cpp:class:`fort::myrmidon::Ant`
.. [#experimentCreate] Python: :py:meth:`fort_myrmidon.Experiment.__init__`, R: :obj:`fmExperimentCreate()`, C++: :cpp:func:`fort::myrmidon::Experiment::Create`
.. [#experimentSave] Python: :py:meth:`fort_myrmidon.Experiment.Save`, R: :obj:`fmExperiment$save()`, C++ :cpp:func:`fort::myrmidon::Experiment::Save`
.. [#createSpace] Python: :py:func:`fort_myrmidon.Experiment.CreateSpace`, R: :obj:`fmExperiment$createSpace`, C++: :cpp:func:`fort::myrmidon::Experiment::CreateSpace`
.. [#addTDD] Python: :py:meth:`fort_myrmidon.Experiment.AddTrackingDataDirectory`, R :obj:`fmExperiment$addTrackingDataDirectory()`, :cpp:func:`fort::myrmidon::Experiment::AddTrackingDataDirectory`
.. [#removeTDD] Python: :py:meth:`fort_myrmidon.Experiment.RemoveTrackingDataDirectory`, R :obj:`fmExperiment$removeTrackingDataDirectory()`, :cpp:func:`fort::myrmidon::Experiment::RemoveTrackingDataDirectory`
.. [#reportStats] Python: :py:meth:`fort_myrmidon.Query.GetDataInformations`, R :obj:`fmQueryGetDataInformations()`, :cpp:func:`fort::myrmidon::Query::GetDataInformations`
