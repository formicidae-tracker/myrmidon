.. defining_ant_metadata

Defining Ant Metadata
=====================

Once ants are created and identified, the next steps is to fill some
metadata on the ants that will be used for interaction detection,
visualisation or for any pre-defined usage.

Ant virtual shapes
******************

.. _ant_shape_fig:
..
   .. figure:: ant_virutal_shape.png


	  Ant virtual shape in **fort-myrmidon**.

	  Ant virtual shape is defined as a collection of typed capsule. Here
	  each type is assigned an unique color. A capsule is defined as
	  region inside and between two circles.

In **fort-myrmidon** each ant has a virtual shape defined as an union
of `capsules` [#capsule]_, as shown in figure
:numref:`ant_shape_fig`. There are no predefined type in a new
experiment, and they must be *created* [#createAntShapeType]_ before
*adding a capsule to an ant* [#addCapsule]_.

Capsule are defined in the ant reference frame.

.. code-block:: python

   #python
   import py_fort_myrmidon as fm

   e = fm.Experiment("defining_ant_metadata.myrmidon")

   a = e.CreateAnt()

   headID = e.CreateAntShapeType("head")
   antennaID = e.CreateAntShapeType("antennas")
   bodyID = e.CreateAntShapeType("body")


   a.AddCapsule(headID,fm.Capsule(c1 = [1,2], c2 = [3,4], r1 = 0.5, r2 = 1.0 ))
   a.AddCapsule(antennaID,fm.Capsule(c1 = [1,2], c2 = [3,4], r1 = 0.5, r2 = 1.0 ))
   a.AddCapsule(antennaID,fm.Capsule(c1 = [1,2], c2 = [3,4], r1 = 0.5, r2 = 1.0 ))
   a.AddCapsule(bodyID,fm.Capsule(c1 = [1,2], c2 = [3,4], r1 = 0.5, r2 = 1.0 ))
   a.AddCapsule(bodyID,fm.Capsule(c1 = [1,2], c2 = [3,4], r1 = 0.5, r2 = 1.0 ))

   for typeID,c in a.Capsules:
       print(typeID,c)

.. code-block:: R

   #R
   library(FortMyrmidon)

   e <- fmExperimentCreate("defining_ant_metadata.myrmidon")

   a <- e$createAnt()

   headID = e$createAntShapeType("head")
   antennaID = e$createAntShapeType("antennas")
   bodyID = e$createAntShapeType("body")


   a$addCapsule(headID,fmCapsule(c1 = c(1,2), c2 = (3,4), r1 = 0.5, r2 = 1.0 ))
   a$addCapsule(antennaID,fmCapsule(c1 = c(1,2), c2 = (3,4), r1 = 0.5, r2 = 1.0 ))
   a$addCapsule(antennaID,fmCapsule(c1 = c(1,2), c2 = (3,4), r1 = 0.5, r2 = 1.0 ))
   a$addCapsule(bodyID,fmCapsule(c1 = c(1,2), c2 = (3,4), r1 = 0.5, r2 = 1.0 ))
   a$addCapsule(bodyID,fmCapsule(c1 = c(1,2), c2 = (3,4), r1 = 0.5, r2 = 1.0 ))

   a$capsules # a data.frame with all capsules data

   for ( typedCapsule in a$capsules ) {
       print(typedCapsule$type,typedCapsule$capsule)
   }


.. code-block:: c++

   #include <fort/myrmidon/Experiment.hpp>

   using namespace fort::myrmidon;

   auto e = Experiment::Create("defining_ant_metadata.myrmidon");
   auto a = e->CreateAnt();

   headID = e->CreateAntShapeType("head");
   antennaID = e->CreateAntShapeType("antennas");
   bodyID = e->CreateAntShapeType("body");


   a->AddCapsule(headID,
                 std::make_shared<Capsule>(Eigen::Vector2d(1,2),
                                           Eigen::Vector2d(3,4),
                                           0.5,
                                           1.0));
   a->AddCapsule(antennaID,
                 std::make_shared<Capsule>(Eigen::Vector2d(1,2),
                                           Eigen::Vector2d(3,4),
                                           0.5,
                                           1.0));
   a->AddCapsule(antennaID,
                 std::make_shared<Capsule>(Eigen::Vector2d(1,2),
                                           Eigen::Vector2d(3,4),
                                           0.5,
                                           1.0));
   a->AddCapsule(bodyID,
                 std::make_shared<Capsule>(Eigen::Vector2d(1,2),
                                           Eigen::Vector2d(3,4),
                                           0.5,
                                           1.0));
   a->AddCapsule(bodyID,
                 std::make_shared<Capsule>(Eigen::Vector2d(1,2),
                                           Eigen::Vector2d(3,4),
                                           0.5,
                                           1.0));

   for ( const auto & [typeID,capsule] : a->Capsules() ) {
       std::cerr << typeID << " " << *capsule << std::endl;
   }


User-defined timed metadata system
**********************************

**fort-studio** provides the ability to define timed key/value pair
for each ant. Like ant shape type, these keys and their default value
must be *defined* [#setMetadataKey]_ in the experiment before,
optionally, be *modified* [#setValue]_ or *accessed* [#getValue]_ for
any ant. Type of key is strongly typed and set in the experiment:
i.e. if the key ``"group"`` is defined to be a ``str``, assigning an
``int`` will result an error. Possible types are restricted to
boolean, integer, float, strings or `Time` [#time]_.

.. code-block:: python

   #python
   e.SetMetaDataKey("group","worker")
   e.SetMetaDataKey("alive",True)

   t = fm.Time.Now()

   a.GetValue("group",t) # is a worker
   a.SetValue("alive",False,t)
   a.GetValue("alive",t) # False
   a.GetValue("alive",t.Add(-1 * fm.Duration.Second)) # True, it was alive before t

.. code-block:: R

   #R
   e$setMetaDataKey("group","worker")
   e$setMetaDataKey("alive",TRUE)

   t <- fmTimeNow()

   a$getValue("group",t) # is a "worker"
   a$setValue("alive",FALSE,t)
   a$getValue("alive",t) # FALSE
   a.GetValue("alive",t$add(fmSecond(-1))) # TRUE, it was alive before t

.. code-block:: c++

   // C++
   e->SetMetaDataKey("group","worker");
   e->SetMetaDataKey("alive",true);

   t = fort::Time::Now()

   a->GetValue("group",t) // is a "worker"
   a->SetValue("alive",false,t)
   a->GetValue("alive",t) # false
   a->GetValue("alive",t.Add(-1 * fort::Duration::Second)) # true, it was alive before t


Ant visualization data
**********************

It is possible to *set or access* [#visualization]_ properties on how
ants should be displayed in **fort-studio**.

.. [#time] Python: :py:class:`py_fort_myrmidon.Time`, R: :obj:`fmTime`, C++: :cpp:class:`fort::Time`
.. [#capsule] Python: :py:class:`py_fort_myrmidon.Capsule`, R: :obj:`fmCapsule`, C++: :cpp:class:`fort::myrmidon::Capsule`
.. [#createAntShapeType] Python: :py:meth:`py_fort_myrmidon.Experiment.CreateAntShapeType`, R: :obj:`fmExperiment$createAntShapeType()`, C++: :cpp:func:`fort::myrmidon::Experiment::CreateAntShapeType`
.. [#addCapsule] Python: :py:meth:`py_fort_myrmidon.Ant.AddCapsule`, R: :obj:`fmAnt$addCapsule()`, C++: :cpp:func:`fort::myrmidon::Ant::AddCapsule`
.. [#setMetadataKey] Python: :py:meth:`py_fort_myrmidon.Experiment.SetMetaDataKey`, R: :obj:`fmExperiment$setMetaDataKey()`, C++: :cpp:func:`fort::myrmidon::Experiment::SetMetaDataKey`
.. [#setValue] Python: :py:meth:`py_fort_myrmidon.Ant.SetValue`, R: :obj:`fmAnt$setValue()`, C++: :cpp:func:`fort::myrmidon::Ant::SetValue`
.. [#getValue] Python: :py:meth:`py_fort_myrmidon.Ant.GetValue`, R: :obj:`fmAnt$getValue()`, C++: :cpp:func:`fort::myrmidon::Ant::GetValue`
.. [#visualization] Python: :py:attr:`py_fort_myrmidon.Ant.DisplayStatus` and :py:attr:`py_fort_myrmidon.Ant.DisplayColor`, R: :obj:`fmAnt$displayStatus` and :obj:`fmAnt$displayColor`, C++ :cpp:func:`fort::myrmidon::Ant::DisplayStatus` and :cpp:func:`fort::myrmidon::Ant::DisplayColor`
