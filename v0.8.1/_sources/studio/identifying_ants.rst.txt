Identifying Ants
================

.. figure:: ../_static/images/identification_workspace.png
   :name: identification_workspace_fig
   :alt: The *Identification* workspace
   :align: center

   **fort-studio** *Identification* workspace

   [1]: The *Tag Close-Ups View* displays a close-up on a tag, and
   allow the user to specify the head and tail of an Ant. [2]: The
   *Tag Close-Ups List* shows available tags, and allow to view a
   specific close-up by using the scrollbar. [3]: The *Identification
   toolbar* allows to create an Ant identified by the currently
   displayed tag. [4]: The *Identifications List* displays all
   identifications defined in the experiment. [5]: The *Tag
   Statistics*: summarize the tag detection statistics for the
   currently displayed tag. The histogram shows the distribution of
   detection gap for the tag.

The creation of Ant and their Identification can be efficiently
performed in the *Identification* workspace. It has been design to
optimize the following workflow.

1. Select the next available tag in the *Tag Close-Ups List*
   (shortcut: :obj:`Alt+W`/:obj:`Alt+S`).
2. Scroll to a suitable Close-Up in the *Tag Close-Ups List*
   (shortcut: :obj:`Alt+A`/:obj:`Alt+D`).
3. Specify the tip of the gaster and tip of the head of the ant **in
   that specific order** in the *Tag Close-Up View*.
4. Create an Ant identified by the current tag with the *Create Ant*
   button (shortcut: :obj:`Ctrl+A`). Its pose information will be
   computed from the tail-head measurement made in the previous step.

.. note::

   **fort-studio** uses an average of the tail-head measurement to
   estimate the ant pose relatively from the tag. By repeating the
   third step for different close-ups, this pose may be improved.

Advanced features
+++++++++++++++++

Editing Identifications
-----------------------

The start and end time of any identification can be modified in the
*Identifications List*. By using the *Add Identification to...*
button, one can then use different tag to identify the same Ant.

The physical tag size can also be edited in this list. Please note
that it is only supposed to be done for a very few individuals
(Queens) as the default tag size can be chosen in the *General*
workspace.

Filtering Tags
--------------

The *Hide Used Tags* option can be used to filter out tag already used
by an Identification in the *Tag Close-Ups List*. The *Hide current
tag* button can be used to temporarily hide a tag. This action could be
used to temporarily discard in the list tags that correspond to
spurious detection.

Shortcuts
+++++++++

* Move to previous/next tag: :obj:`Alt+W`/:obj:`Alt+S`
* Move to previous/next close-up: :obj:`Alt+A`/:obj:`Alt+D`
* Create an Ant Identified By Current Tag: :obj:`Ctrl+A`
* Add Identification To... : :obj:`Ctrl+I`
* Delete current pose estimation : :obj:`Ctrl+Shift+D`
* Hide current tag: :obj:`Ctrl+Shift+H`
* Show all hidden tag: :obj:`Ctrl+Shift+O`
