.. identify_ants:

Identifying Ants
================

The main purpose of a tracking *Experiment* [#experiment]_ is to study
*Ant* [#ant]_ movements and Interactions.

Creating Ants
*************

The first step would be to *create an Ant* [#createAnt]_ for each
individual in our experiment. Like spaces, ants have a unique
ID in the experiment which cannot be chosen, but will be generated
starting from 1.

.. code-block:: python

   #python
   import fort_myrmidon as fm

   e = fm.Experiment("identifying-ants.myrmidon")

   ants = [e.CreateAnt(),e.CreateAnt(),e.CreateAnt()]
   for i,a in enumerate(ants):
       print("Ant at index %d has ID: %s"%(i,fm.FormatAntID(a.ID)))
   # outputs:
   #    Ant at index 0 has ID: 001
   #    Ant at index 1 has ID: 002
   #    Ant at index 2 has ID: 003

.. code-block:: R

   #R
   library(FortMyrmidon)

   e <- fmExperimentCreate("identifying-ants.myrmidon")

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

   auto e = fort::myrmidon::Experiment::Create("identifying-ant.myrmidon");

   std::vector<fort::myrmidon::Ant> ants = { e->reateAnt(),e->CreateAnt(),e->CreateAnt()};
   for ( size_t i = 0; i < ants.size(); ++i ) {
       std::cerr << "Ants at index " << i << " has ID: " << fort::myrmidon::FormatAntID(ants[i]->ID()) << std::endl;
   }
   //outputs:
   //    Ant at index 0 has ID: 001
   //    Ant at index 1 has ID: 002
   //    Ant at index 2 has ID: 003


.. note::

   If you delete an ant and there would be a gap in the set of all
   ant IDs, the next time an ant would be created, it will have an ID
   that will fill that gap.



Associating Ants with Tags
***************************


Creation of an Identification
-----------------------------

Next we use *Identifications* [#identification]_ to associate an ant
with a tag ID. This relation is not a direct 1:1 relation, has
identifications are bounded in time, i.e. an ant is associated
with a tag from a given time up to another. As **fort-myrmidon**
uses its own *Time* [#time]_ objects, these value can effectively be
set to -∞ [#timeSinceEver]_ or +∞ [#timeForever]_ for a given
identification. Modeled as such, one ant could then be identified
by several different tags over time.

Internally, **fort-myrmidon** ensure that these two conditions are met:

* For any given time, any ant is identified by a unique tag ID (or
  none).

* For any given time, any tag ID identifies only a unique ant (or
  none).

To ensure this, one cannot create directly identification, but must
*create an Identification through an Experiment* [#addIdentification]_

.. code-block:: python

   #python
   for tagID,a in enumerate(ants):
       i = e.AddIdentification(antID = ants[0].ID,
                               tagID = tagID) #start and end have default values to -∞/+∞
	   print(i)
   # outputs:
   #     Identification{ID: 0x000 ↦ 001, From: -∞, To: +∞}
   #     Identification{ID: 0x001 ↦ 002, From: -∞, To: +∞}
   #     Identification{ID: 0x002 ↦ 003, From: -∞, To: +∞}

.. code-block:: R

   #R
   for ( i in seq(1:length(ants) ) {
       identification <- e$addIdentification(ants[[i]]$ID,i-1,fmTimeSinceEver(),fmTimeForever())
	   print(identification)
   }
   # outputs:
   #     fmIdentification( $tagValue = 0x000, $targetAntID = 001, $start = -∞, $end = +∞ )
   #     fmIdentification( $tagValue = 0x001, $targetAntID = 002, $start = -∞, $end = +∞ )
   #     fmIdentification( $tagValue = 0x002, $targetAntID = 003, $start = -∞, $end = +∞ )

.. code-block:: c++

   //C++
   for ( size_t i = 0; i < ants.size(); ++i) {
       auto identification = e->AddIdentification(ants[i]->ID(),i,fort::Time::SinceEver(),fort::Time::Forever());
	   std::cerr << *identification << std::endl;
   }
   // outputs:
   //     Identification{ID: 0x000 ↦ 001, From: -∞, To: +∞}
   //     Identification{ID: 0x001 ↦ 002, From: -∞, To: +∞}
   //     Identification{ID: 0x002 ↦ 003, From: -∞, To: +∞}

After being added to the experiment each identification will be
found in the targeted *ant listing of identifications* [#antIdentifications]_:

.. code-block:: python

   #python
   for a in ants:
       print("Ant %s is identified by:" % fm.FormatAntID(a.ID))
	   for i in a.Identifications:
	       print(" * %s" % i)
   # outputs:
   #     Ant 001 is identified by:
   #      * Identification{ID: 0x000 ↦ 001, From: -∞, To: +∞}
   #     Ant 002 is identified by:
   #      * Identification{ID: 0x001 ↦ 002, From: -∞, To: +∞}
   #     Ant 003 is identified by:
   #      * Identification{ID: 0x002 ↦ 003, From: -∞, To: +∞}


.. code-block:: R

   #R
   for (a in ants) {
       printf("Ant %s is identified by:\n", fmFormatAntID(a$ID))
	   for (i in a$identifications){
	       printf(" * %s\n", capture.output(i))
	   }
   }
   # outputs:
   #     Ant 001 is identified by:
   #      * fmIdentification( $tagValue = 0x000 , $targetAntID = 001 , $start = -∞, $end = +∞ )
   #     Ant 002 is identified by:
   #      * fmIdentification( $tagValue = 0x001 , $targetAntID = 002 , $start = -∞, $end = +∞ )
   #     Ant 003 is identified by:
   #      * fmIdentification( $tagValue = 0x002 , $targetAntID = 003 , $start = -∞, $end = +∞ )

.. code-block:: c++

   //c++
   for (const auto & a : ants ) {
       std::cerr << "Ant "
	             << fort::myrmidon::FormatAntID(a->ID())
				 << " is identified by:"
				 << std::endl;
	   for ( const auto & i :  a->Identifications() ) {
	       std::cerr << " * " << *i << std::endl;
	   }
   }
   // outputs:
   //     Ant 001 is identified by:
   //      * Identification{ID: 0x000 ↦ 001, From: -∞, To: +∞}
   //     Ant 002 is identified by:
   //      * Identification{ID: 0x001 ↦ 002, From: -∞, To: +∞}
   //     Ant 003 is identified by:
   //      * Identification{ID: 0x002 ↦ 003, From: -∞, To: +∞}

Geometric relation between tags and ants
----------------------------------------

Identfications also holds the relation between the tag center and
orientation to the estimated ant's center and orientation. Usually
this relation is automatically computed from manual measurements made
in **fort-studio** pointing to the tip of the head and tip of the
abdomen.

For some large experiment, it is preferable to infer this relation
from a chosen heuristic, and force **fort-myrmidon** to *set a user
defined ant pose* [#userDefinedAntPose]_. Please remark that this
information should be set for every identification of an Ant.

.. note::

   This relation's translation is the offset of the ant center to the
   tag center, *expressed in the tag coordinate system*. The angle
   correspond to the difference in radians of the ant orientation from
   the tag orientation , with *trigonometric orientation*. As in computer
   vision, the y-axis is conventionally going from top to bottom,
   **positive angles appears clockwise**.

.. code-block:: python

   #python
   ants[0].SetUserDefinedAntPose(antPosition = [0,0], antAngle = 0)
   print("Ant %d has pose %s and angle %f" % (fm.FormatAntID(ants[0].ID),ants[0].AntPosition,ants[0].AntAngle))
   # outputs: Ant 001 has pose [0.0 0.0] and angle 0.0

.. code-block:: R

   #R
   ants[[1]]$identifications[[1]]$setUserDefinedAntPose(c(0,0), 0.0)
   printf("Ant %s has pose %s and angle %f", fmFormatAntID(ants[[1]]$ID),paste(ants[[1]]$identifications[[1]]$antPosition,collapse=","),ants[[1]]$identifications[[1]]$antAngle)
   # outputs: Ant 001 has pose 0,0 and angle 0.000000

.. code-block:: c++

   //C++
   ants[0]->SetUserDefinedAntPose(Eigen::Vector2d(0,0), 0.0);
   std::cerr << "Ant " << fort::myrmidon::FormatAnt(ant[0]->ID())
             << " has pose "<< ant[0]->AntPosition().transpose()
             << " and angle " << ant[0]->AntAngle()
             << std::endl;
   // outputs: Ant 001 has pose (0.0 0.0) and angle 0.0

This user defined can be *cleared* [#clearUserDefinedAntPose]_ to
revert to the position/angle derived from manual measurement.


.. [#experiment] Python: :py:class:`fort_myrmidon.Experiment`, R: :obj:`fmExperiment`, C++: :cpp:class:`fort::myrmidon::Experiment`
.. [#ant] Python: :py:class:`fort_myrmidon.Ant`, R: :obj:`fmAnt`, C++: :cpp:class:`fort::myrmidon::Ant`
.. [#identification] Python: :py:class:`fort_myrmidon.Identification`, R: :obj:`fmIdentification`, C++: :cpp:class:`fort::myrmidon::Identification`
.. [#time] Python: :py:class:`fort_myrmidon.Time`, R: :obj:`fmTime`, C++: :cpp:class:`fort::Time`
.. [#timeSinceEver] Python: :py:meth:`fort_myrmidon.Time.SinceEver`, R: :obj:`fmTimeSinceEver()`, C++: :cpp:func:`fort::Time::SinceEver`
.. [#timeForever] Python: :py:meth:`fort_myrmidon.Time.Forever`, R: :obj:`fmTimeForever()`, C++: :cpp:func:`fort::Time::Forever`
.. [#createAnt] Python: :py:meth:`fort_myrmidon.Experiment.CreateAnt`, R: :obj:`fmExperiment$createAnt()`, C++: :cpp:func:`fort::Experiment::CreateAnt`
.. [#addIdentification] Python: :py:meth:`fort_myrmidon.Experiment.AddIdentification`, R: :obj:`fmExperiment$addIdentification()`, C++: :cpp:func:`fort::Experiment::AddIdentification`
.. [#antIdentifications] Python: :py:attr:`fort_myrmidon.Ant.Identifications`, R: :obj:`fmAnt$identifications`, C++ :cpp:func:`fort::myrmidon::Ant::Identifications`
.. [#userDefinedAntPose] Python: :py:meth:`fort_myrmidon.Identification.SetUserDefinedAntPose`, R: :obj:`fmIdentification$setUserDefinedAntPose`, C++ :cpp:func:`fort::myrmidon::Identification::SetUserDefinedAntPose`
.. [#clearUserDefinedAntPose] Python: :py:meth:`fort_myrmidon.Identification.ClearUserDefinedAntPose`, R: :obj:`fmIdentification$clearUserDefinedAntPose`, C++ :cpp:func:`fort::myrmidon::Identification::ClearUserDefinedAntPose`
