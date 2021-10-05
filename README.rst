.. raw:: html

    <p align="center"><img src="https://github.com/formicidae-tracker/myrmidon/raw/master/resources/icons/fort-studio.svg" width="200px" alt="fort-studio logo"></p>

``fort-myrmidon`` and ``fort-studio`` — Post-processing API and User Interface
******************************************************************************

|Build Status| |Coverage Status| |Stable Documentation Status| |License|


`FORT Project Wiki <https://github.com/formicidae-tracker/documentation/wiki>`_
• `Report a Bug <https://github.com/formicidae-tracker/myrmidon/issues/new?template=bug_report.md>`_
• `Request a Feature <https://github.com/formicidae-tracker/myrmidon/issues/new?template=feature_request.md>`_


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

INstallation
------------

**fort-myrmidon** and all the supported bindings (R,python) are primarly
distributed with
[conda](https://docs.conda.io/projects/conda/en/latest/).

**fort-studio** is distributed using **snap**.

Detailed installation instructions can be found in the `documentation
<https:://formicidae-tracker.github.io/myrmidon/latest/installation.html>`_

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
