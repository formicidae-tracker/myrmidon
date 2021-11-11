Introduction
============

**fort-myrmidon** has two main purposes:

 1. The edition of ``.myrmidon`` files a collection of meta-data on a
    tracking experiment of the FORT system. This includes how to
    identify the individuals in the experiment, how to describe, and
    how to represent them.

 2. To perform efficient queries on large tracking data sets. This
    includes identification and pose correction, collision detection,
    trajectories and interactions determination.


**fort-studio** is a graphical interface of **fort-myrmidon** to help
 with the edition of this metadata. However, it cannot perform nor
 display any complex queries.


The first part of this documentation describes **fort-myrmidon**
concepts alongside its API. The presentation will follow the expected
workflow for processing the tracking data of an experiment. The second
part is dedicated to **fort-studio** manual, and finally the last part
will gives more details on the **fort-myrmidon** API.
