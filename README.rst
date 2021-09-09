.. raw:: html

    <p align="center"><img src="https://github.com/formicidae-tracker/myrmidon/raw/master/resources/icons/fort-studio.svg" width="200px" alt="fort-studio logo"></p>

``fort-myrmidon`` and ``fort-studio`` — Post-processing API and User Interface
******************************************************************************

|Build Status| |Coverage Status| |Stable Documentation Status| |License|


`FORT Project Wiki <https://github.com/formicidae-tracker/documentation/wiki>`_
• `Report a Bug <https://github.com/fortmicidae-tracker/myrmidon/issues/new>`_
• `Request a Feature <https://github.com/formicidae-tracker/myrmidon/issues/new>`_


**fort-myrmidon** is an API to facilitate access and analyzis of
manage and visualize large Ant tracking datasets. It is used to
manipulate metadata files (.myrmidon). Those files catalogs ant
identifications with the help of fiducial tags, and other properties
such as size and shapes and user-defined metadata. It proposes python
and R bindings with **py_fort_myrmidon** and **r-fort-myrmidon**
packages.

**fort-studio** is a graphical interface that helps manual creation and
editing of **.myrmidon** files, and to provide basic visualization of
tracking datasets.

Installation
------------

**fort-myrmidon** and all the supported bindings (R,python) is primarly
distributed with
[conda](https://docs.conda.io/projects/conda/en/latest/).

**fort-studio** is principally distributed using **snap**. However it is
also distributed with **conda** to allow to pinpoint a perticular
release.

Install **fort-myrmidon** from **conda**
========================================

The following packages are available from the anaconda channel
`formicidae-tracker <https://anaconda.org/formicidae-tracker>`_:

* **libfort-myrmidon**: C++ library and development files
* **r-fort-myrmidon**: R bindings for **libfort-myrmidon**. R 3.6 and 4.0 are currently supported
* **py-fort-myrmidon**: Python bindings for **libfort-myrmidon**
* **fort-studio**: conda distibution of **fort-studio**. It is provided to
  allow pinpointing a perticular version. The snap distribution should
  be preferred in most use case. Please remind that care must be taken
  if you install this conda package, as the command line executable
  **fort-studio** may not match anymore the application **FORT Studio**
  available through the **snap** package.

Install **fort-studio** from snap
=================================

**fort-studio** is available through its own snap. It comes with its own
bundled version of **libfort-myrmidon**. It can be installed using the
following command:

.. code-block:: bash

   sudo snap install fort-studio


If your tracking data is not in your home but on a removable media,
you must allow **fort-studio** the access to removable media using the
following command:

.. code-block:: bash

   sudo connect fort-studio:removable-media :removable-media

Installation from sources
=========================

This project uses CMake as a build system. It requires a minimum
version of 3.11 at least as it needs to fetch some external
dependencies using **fetch_content**. It also requires the following
dependencies:

* OpenCV 3 or 4
* cmake (>=3.11)
* Eigen 3
* Asio Library
* Qt 5
* Google Protobuf (>=3.3.0)
* Google glog
* yaml-cpp
* TBB (==2020.02)


Versioning
----------

We use `SemVer <http://semver.org/>`_ for versioning. For the versions
available, see the `releases
<https://github.com/formicidae-tracker/myrmidon/releases>`_.

Authors
-------

The file `AUTHORS
<https://github.com/formicidae-tracker/myrmidon/blob/master/AUTHORS>`_
lists all copyright holders (physical or moral person) for this
repository.

See also the list of `contributors
<https://github.com/formicidae-tracker/myrmidon/contributors>`_ who
participated in this project.

License
-------

This project is licensed under the GNU Lesser General Public License
v3.0 or later - see the `LICENSE
<https://github.com/formicidae-tracker/myrmidon/blob/master/LICENSE>`_
file for details


.. |Build Status| image:: https://github.com/formicidae-tracker/myrmidon/actions/workflows/build.yml/badge.svg
   :target: https://github.com/formicidae-tracker/myrmidon/actions/workflows/build.yml
.. |Coverage Status| image:: https://codecov.io/gh/formicidae-tracker/myrmidon/branch/master/graph/badge.svg
   :target: https://codecov.io/gh/formicidae-tracker/myrmidon
.. |Stable Documentation Status| image:: https://github.com/formicidae-tracker/myrmidon/actions/workflows/docs.yml/badge.svg
   :target: https://formicidae-tracker.github.io/myrmidon/latest
.. |License| image:: https://img.shields.io/github/license/formicidae-tracker/myrmidon.svg
   :target: https://github.com/formicidae-tracker/myrmidon/blob/master/LICENSE
