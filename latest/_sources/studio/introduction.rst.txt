Introduction
============

**fort-studio** is a graphical interface used for the edition of
``.myrmidon`` files, and the visualization of the tracking data.

It follows a workflow similar to **fort-myrmidon** but allows
graphical edition of *Identification Ant Poses*, *Ant virtual shapes*,
*Ant measurements* and *Zone geometries*.


Interface Organization
**********************

**fort-studio** is organized by *workspaces*, i.e. views that focus on a specific tasks:

* *General* workspace (:obj:`Alt + 1`) : General information on the
  experiment, ant listing and space / tracking data directory setup.
* *Statistics* workspace (:obj:`Alt + 2`): Displays statistics on tag
  detection count
* *Identification* workspace (:obj:`Alt + 3`): Edit ant identification
  definition and geometric pose
* *Ant shape* workspace (:obj:`Alt + 4`): Edit ant's virtual shape
* *Measurement* workspace (:obj:`Alt + 5`): Perform diverse measurement
  on each Ant
* *Zoning* workspace (:obj:`Alt + 6`): Defines geometric zone for each space
* *Ant Metadata* workspace (:obj:`Alt + 7`): Defines Ant timed
  Key/Value pairs
* *Visualization* workspace (:obj:`Alt + 8`): Visualizes
  identifications and collisions during the experiment, edit ant
  metadata based on tracking video.


Workspace can be selected through the :obj:`Workspace` menu or
toolbar, or through the mentioned shortcut.

.. note::

   Shortcuts in **fort-studio** are organized between *action
   shortcuts* (which modifies the experiment data) and *navigation
   and selection shortcuts*. The *action shortcuts* always use the
   :obj:`Ctrl` modifier. The *navigation shortcuts* uses the :obj:`Alt`
   modifier, or no modifier at all.
