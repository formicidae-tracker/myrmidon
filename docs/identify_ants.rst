.. identify_ants:

Identifying `Ants`
==================

The main purpose of a tracking `Experiment` [#experiment]_ is to study
`Ant` [#ant]_ movements and Interactions.

Creating `Ants`
+++++++++++++++

The first step would be to `create an Ant` [#createAnt]_ for each
individuals in our `Experiment`. Like `Spaces`, `Ants` have a unique
ID in the `Experiment` which cannot be choosen, but will be generated
starting from 1.

.. code-block:: python

   #python
   ants = [e.CreateAnt(),e.CreateAnt(),e.CreateAnt()]
   for i,a in enumerate(ants):
       print("Ant at index %d has ID: %s"%(i,fm.FormatAntID(a.ID)))
   # outputs:
   #    Ant at index 0 has ID: 001
   #    Ant at index 1 has ID: 002
   #    Ant at index 2 has ID: 003

.. code-block:: R

   #R
   ants <- list(e$createAnt(),e$createAnt(),e$createAnt())
   for (i in seq(1,3)) {
       printf("Ant at index %d has ID: %s\n",i,fmFormatAntID(ants[[i]]$ID))
   }
   # outputs:
   #    Ant at index 1 has ID: 001
   #    Ant at index 2 has ID: 002
   #    Ant at index 3 has ID: 003

.. code-block:: c++

   //C++
   #include <fort/myrmidon/Experiment.hpp>
   std::vector<fort::myrmidon::Ant> ants = { e->reateAnt(),e->CreateAnt(),e->CreateAnt()};
   for ( size_t i = 0; i < ants.size(); ++i ) {
       std::cerr << "Ants at index " << i << " has ID: " << fort::myrmidon::FormatAntID(ants[i]->ID()) << std::endl;
   }
   //outputs:
   //    Ant at index 0 has ID: 001
   //    Ant at index 1 has ID: 002
   //    Ant at index 2 has ID: 003


.. note::

   If you delete an `Ant` an there would be a gap in the set of all
   ant IDs, the next time an Ant would be created, it will have an ID
   that will fill that gap.



Identifying `Ants` with `Tags`
++++++++++++++++++++++++++++++

.. [#experiment] Python: :py:class:`py_fort_myrmidon.Experiment`, R: :obj:`fmExperiment`, C++: :cpp:class:`fort::myrmidon::Experiment`
.. [#ant] Python: :py:class:`py_fort_myrmidon.Ant`, R: :obj:`fmAnt`, C++: :cpp:class:`fort::myrmidon::Ant`
