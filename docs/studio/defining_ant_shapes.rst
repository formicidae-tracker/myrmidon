Defining Ant Shapes
===================

.. figure:: ../_static/images/ant_shape_workspace.png
   :name: ant_shape_workspace_fig
   :alt: The *Ant Shape* workspace
   :align: center

   **fort-studio** *Ant Shape* workspace

   [1]: *Ant Close-Up View* that allow to specify the virtual shape of
   a given Ant. [2]: *Ant Close-Ups List*, similarly to the *Tag
   Close-Up List*, allows to select a Close-Up for an Ant, but here
   organize Close-Up per Ant instead of per Tag. [3]: The toolbar that
   allow the edition of the Capsules defining the shape of the
   Ant. [4] *Ant Shape Type List* is used to define the list of shape
   types for the Experiment.


The *Ant Shape* workspace is used to define, view and edit the virtual
shape of any Ant.

.. warning::

   **fort-studio** defines no default list of ant shape type. Before
   drawing any shape, one must define the wanted shape type for the
   experiment in the *Ant Shape Type List* (see figure
   :numref:`ant_shape_workspace_fig`).


.. note::

   To avoid defining the same shape for every single ant in the
   experiment, the :obj:`Edit > Clone Current Ant Shape` action can be
   used. Individual shapes can still be edited afterwards.


Shortcuts
---------

The same navigation shortcuts from the *Identification* workspace are
available:

* Move to previous/next tag: :obj:`Alt+W`/:obj:`Alt+S`
* Move to previous/next close-up: :obj:`Alt+A`/:obj:`Alt+D`
