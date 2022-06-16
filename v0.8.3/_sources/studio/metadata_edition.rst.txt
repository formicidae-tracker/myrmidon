Ant Metadata Edition
====================

.. figure:: ../_static/images/metadata_workspace.png
   :name: metadata_workspace_fig
   :alt: The *Ant Metadata* workspace
   :align: center

   **fort-studio** *Ant Metadata* workspace

   [1]: *Ant Metadata Tree View* displays and allow edition of each
   ant user metadata [2]: *Key Definitions List* allows the definition
   of the key available to the Experiment.


The *Ant Metadata* workspace allows the edition of each Ant user
defined metadata. Metadata are timed values associated to a specific
key. For example a ``isAlive`` boolean key can be defined, with its
default value to ``true``. Then for specific Ant, one can mark them
"dead" by setting ``isAlive`` to ``false`` after a given time.

For this purpose, one must use the *Key Definitions List* to define
the key and its default value. Then after selecting the ``isAlive``
row under the wanted Ant, one could use the :obj:`+` button to add a
timed value to ``false`` for this specific Ant.

.. note::

   **fort-studio** does not define any key for an Experiment. User
   should define the wanted key first.


Efficient Input of Times
------------------------

The time format used by **fort-studio** is defined by the RFC 3339. It
can be cumbersome to enter them to the microsecond. However, while
displaying any Close-Up or Video Frame, one could use the *Copy
Current Time* action (:obj:`Ctrl+Shift+C`) to copy the exact RFC
3339 string corresponding to the displayed Close-Up or Video Frame in
the copy-paste buffer.
