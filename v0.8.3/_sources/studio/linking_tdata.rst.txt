Linking Tracking Data
=====================



.. figure:: ../_static/images/general_workspace.png
   :name: general_workspace_fig
   :alt: The *General* workspace
   :align: center

   **fort-studio** *General* workspace

   [1]: General property editor, [2]: Space and Tracking Data Directory editor, [3]: Ant listing


*Tracking Data Directories (TDD)* are associated with a *Space* in the
*General* workspace. This operation can be performed in the *Space and
Tracking Data Directory editor* (see figure :numref:`general_workspace_fig`),
by clicking on the :obj:`+` button. After selecting an actual directory,
the user will be prompted to assign it to an existing or a new *Space*.

.. note::

   The very first time a *TDD* is loaded in **fort-studio** will cause
   a bunch of computation to be performed (Computing tag detection
   statistics, Computing Tag Close-Up, Generating Full frames). These
   operations' results are cached and will not be performed the next
   time **fort-studio** loads this *TDD*.
