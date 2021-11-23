#pragma once

#include "BindTypes.hpp"

namespace py = pybind11;

void BindTime(py::module_ &);
void BindColor(py::module_ &);
void BindTypes(py::module_ &);
void BindShapes(py::module_ &);

void BindIdentification(py::module_ &);
void BindAnt(py::module_ & );

void BindZone(py::module_ &);
void BindSpace(py::module_ &);

void BindTrackingSolver(py::module_ & );
void BindVideoSegment(py::module_ & );

void BindExperiment(py::module_ & );
void BindMatchers(py::module_ &);
void BindQuery(py::module_ &);
