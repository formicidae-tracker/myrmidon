#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/operators.h>
#include <pybind11/chrono.h>

#include <fort/myrmidon/types/Typedefs.hpp>
#include <fort/myrmidon/Shapes.hpp>

namespace fort {
namespace myrmidon {
struct VideoFrameData;
struct VideoSegment;
}
}

PYBIND11_MAKE_OPAQUE(fort::myrmidon::Vector2dList)
PYBIND11_MAKE_OPAQUE(fort::myrmidon::Shape::List)
PYBIND11_MAKE_OPAQUE(std::vector<fort::myrmidon::VideoFrameData>)
PYBIND11_MAKE_OPAQUE(std::vector<fort::myrmidon::VideoSegment>)
