.. api_typedef:

API generic types
=================

Simple typedefs
+++++++++++++++

.. doxygentypedef:: fort::myrmidon::TagID

.. doxygentypedef:: fort::myrmidon::AntID

.. doxygentypedef:: fort::myrmidon::ZoneID

.. doxygentypedef:: fort::myrmidon::SpaceID

.. doxygentypedef:: fort::myrmidon::AntShapeTypeID

.. doxygentypedef:: fort::myrmidon::MeasurementTypeID

.. doxygenvariable:: fort::myrmidon::HEAD_TAIL_MEASUREMENT_TYPE

.. doxygentypedef:: fort::myrmidon::Vector2dList

.. doxygenenum:: fort::myrmidon::AntMetaDataType

.. doxygentypedef:: fort::myrmidon::AntStaticValue

.. doxygentypedef:: fort::myrmidon::TypedCapsuleList

.. doxygentypedef:: fort::myrmidon::InteractionID

.. doxygentypedef:: fort::myrmidon::InteractionTypes

Structures
++++++++++

These structure are the results returned by
:cpp:class:`fort::myrmidon::Query`

General Information queries
***************************

.. doxygenstruct:: fort::myrmidon::ComputedMeasurement
   :members:

.. doxygenstruct:: fort::myrmidon::TagStatistics
   :members:

.. doxygenstruct:: fort::myrmidon::ExperimentDataInfo
   :members:

.. doxygenstruct:: fort::myrmidon::SpaceDataInfo
   :members:

.. doxygenstruct:: fort::myrmidon::TrackingDataDirectoryInfo
   :members:

Snapshot Queries
****************

.. doxygenstruct:: fort::myrmidon::IdentifiedFrame
   :members:

.. doxygenstruct:: fort::myrmidon::Collision
   :members:

.. doxygenstruct:: fort::myrmidon::CollisionFrame
   :members:


Time-spanning Queries
*********************

.. doxygenstruct:: fort::myrmidon::AntTrajectory
   :members:

.. doxygenstruct:: fort::myrmidon::AntTrajectorySegment
   :members:

.. doxygenstruct:: fort::myrmidon::AntInteraction
   :members:
