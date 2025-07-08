Installation
============

It is recommended to install **fort-myrmidon**, **py-fort-myrmidon** via ``pip``
and to install **fort-studio** using ``flatpak``.

**fort-studio** through flatpak
+++++++++++++++++++++++++++++++


Simply adds the following remotes to install fort studio through its appstream identifier `io.github.formicidae_tracker.Studio`

```bash
flatpak remote-add --if-not-exists --user flathub https://dl.flathub.org/repo/flathub.flatpakrepo
flatpak remote-add --if-not-exists --user tuleu.science https://packages.tuleu.science/tuleu.science.flatpakrepo
flatpak install io.github.formicidae_tracker.Studio
```

.. note::

   **fort-studio** used to be distribued via a snap. This is not maintained
   anymore.

**fort-myrmidon** and python bindings
+++++++++++++++++++++++++++++++++++++

The bindings with it C++ backend can be installed through a wheel.

```bash
pip install formicidae-tracker-myrmidon
```


.. note::

   Only recent linux distribution are supported at the moment (manylinux_2_28).

.. note::

   fort-myrmidon used to be distribued with conda. This is not supported anymore
   since version v0.9.0

Installation from source
++++++++++++++++++++++++

Dependencies
------------

**fort-myrmidon** and **fort-studio** depends on the following libraries:

* eigen3
* protobuf
* tbb >= 2
* libasio
* libavutil
* libavcodec
* libavformat
* libswscale
* GLog ( **fort-studio** only )
* Qt 5 ( **fort-studio** only )
* libqtcharts ( **fort-studio** only )
* pybind11-dev ( Python bindings only )
* python3-pandas ( Python bindings only )
* python3-tqdm ( Python bindings only )
* python3-opencv ( Python bindings only )


Under debian, you can install them with the following command.

```bash
sudo apt install libeigen3-dev libprotobuf-dev protobuf-compiler libtbb-dev libasio-dev libavutil-dev libavcodec-dev libavformat-dev libswscale-dev qt6-base-dev libgoogle-glog-dev qt5-qmake libqt5charts5-dev pybind11-dev python3-tqdm python3-pandas python3-opencv
```



Including **fort-studio** to your project
+++++++++++++++++++++++++++++++++++++++++




Python
------

After installing the **formicidae-tracker-myrmidon** package in an environment,
the **fort_myrmidon** module is available to python in that environment.

.. note::

   The examples in this documentation assumes this module to be included as :

   .. code-block:: python

      import fort_myrmidon as fm

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
G
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
