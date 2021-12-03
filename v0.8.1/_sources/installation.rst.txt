Installation
============

It is recommended to install **fort-myrmidon**, **py-fort-myrmidon**
and **r-fort-myrmidon** via ``conda`` or ``mamba`` and to install
**fort-studio** using ``snap``.

**fort-studio** through snap
++++++++++++++++++++++++++++

**fort-studio** is available on the snap store with the eponymous snap:

.. code-block:: bash

   sudo snap install fort-studio

.. warning::

   As for most other snaps, **fort-studio** uses strict confinement. It
   means that by default, it will only be allowed to access files on the
   current user's home. However, with the FORT system, experiment tends to
   be quite large and stored on external hard drives. To enable
   **fort-studio** to access these files, you must grant the snap right
   to access external hard drives by allowing the ``removable-media``
   connection:

   .. code-block:: bash

	  sudo snap connect fort-studio:removable-media :removable-media

.. note::

   For systems with auxiliary disk, it is recommended to mount
   the disk meant to be used with **fort-studio** under ``/media``, as
   otherwise they won't be accessible through the confinement.

   The ``edge`` channel of this snap is reserved for continuous
   deployment and is automatically updated with any code pushed on the
   ``master`` branch.


**fort-myrmidon** and bindings through ``conda``
++++++++++++++++++++++++++++++++++++++++++++++++

A complete distribution of **libfort-myrmidon** (C++ library)
**py-fort-myrmidon** (python bindings) and **r-fort-myrmidon** (R
bindings) are available on the `formicidae-tracker
<https::/anaconda.org/formicidae-tracker>`_ channel.



Tips on an efficient **conda** usage
------------------------------------

**conda** can be a bit daunting to master at first. The ecosystem of
tools has grown quite large, and it is difficult to get all essential
information in one place. You can find some pointers on essentials
items regarding **fort-myrmidon**.

.. note::

   More general, but detailed information can be found in the
   `"Introduction to Conda for (Data) Scientist"
   <https://carpentries-incubator.github.io/introduction-to-conda-for-data-scientists/aio/index.html>`_.

1. Install **miniconda** instead of **anaconda**. The latter comes with
   a very large number of preinstalled packages, and most of them are
   not required to run **fort-myrmidon**. This could lead to quite
   long solving environment computation time, which are for some users
   quite irritating.

2. **conda** setup: Since **libfort-myrmidon** depends on package from
   **anaconda** and **conda-forge** a careful installation as per
   `conda-forge documentation
   <https://conda-forge.org/docs/user/introduction.html#how-can-i-install-packages-from-conda-forge>`_
   is strongly recommended. In summary, one must perform once:

   .. code-block:: bash

	  conda config --add channels conda-forge
	  conda config --set channel_priority strict

3. Use `mamba <https://github.com/mamba-org/mamba>`_ to install
   **conda** package. This is a tool that is a replacement for the
   **conda** command which is an order of magnitude faster with
   dependencies and environment solving. **fort-myrmidon** packages
   are now built and tested with **mamba**, and therefore is supported.

4. Do not `ever` use the ``base`` environment. This is a good practice
   rule to avoid running into a lot of dependency problems when you may
   want to be able to use different **fort-myrmidon** version on the
   same system. Let says you are running a long experiment for over a
   year and do not want to lose time rewriting all your script because
   of a major update of **fort-myrmidon** but you still want for a new
   project to start to use the latest version of
   **fort-myrmidon**. With the power of **conda** virtual environment,
   you can easily install the two versions, and choose when you run
   your script which version to use. However, if you install everything
   in the ``base`` environment, you would certainly lose the ability
   to do that as it will certainly lead to a lot of conflict between
   your base and environment where you want another **fort-myrmidon**
   version installed. Therefore, `always` make sure:

   a. To activate the environment you want to modify:

	  .. code-block:: bash

		 conda activate my-env-name

	  You can create an environment with:

	  .. code-block:: bash

		 conda create -n my-env-name

   b. The right environment name is displayed in your prompt, and not
      ``base``.

Including **fort-studio** to your project
+++++++++++++++++++++++++++++++++++++++++


Python
------

After installing the **py-fort-myrmidon** package in an environment, the
**py_fort_myrmidon** module is available to python in that
environment.

.. note::

   The examples in this documentation assumes this module to be included as :

   .. code-block:: python

	  import py_fort_myrmidon as fm

R
-

After installing the **r-fort-myrmidon** package in an environment,
the **FortMyrmidon** package is available to the R version `packaged
with this environment`. The system packaged R will not be able to see
nor to use without crashing the **FortMyrmidon** package.

C++
---

After installing the **libfort-myrmidon** package, libraries and
header files for **fort-myrmidon** will be available to the targeted
conda environment. The recommanded way is to use CMake to build your
project and link with **fort-myrmidon**.

.. code-block:: cmake

   find_package(FortMyrmidon REQUIRED)
   include_directories(FORT_MYRMIDON_INCLUDE_DIRS)
   # fort-myrmidon requires at least the c++17 standard
   set(CMAKE_CXX_STANDARD 17)

   # once my-target is created
   target_link_libraries(my-target ${FORT_MYRMIDON_LIBRARIES})


A ``pkg-config`` configuration file is also available, but it is not
nicely integrated with **conda**. Its utilisation should be avoided.


Using **rstudio** with **conda**
--------------------------------

If you are an **rstudio** user, you will remark that it will always
use the system's R version instead of the one you installed with
**conda**, which means that it will never be able to find
**r-fort-myrmidon**. Indeed, **rstudio** will use the first R version
found in the PATH, and if you launch it from the system's menu, it
cannot find the version you installed as a user (it is not
conda-aware), and defaults to the system's one. There are two means to circumvent that:

a. Runs **rstudio** from a terminal within the right environment
   (recommended). One can simply use the following commands to
   activate ``my-env`` and run rstudio within it:

   .. code-block::

	  conda activate my-env-name
	  rstudio


b. Installs and runs **rstudio** from **conda**. It is not recommended as
   it is a more heavy solution and will make your environments much more
   complex and time-consuming to solve.
