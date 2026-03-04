#pragma once

#include "fort/myrmidon/priv/Ant.hpp"
#include "fort/myrmidon/priv/Identification.hpp"
#include "fort/myrmidon/types/Typedefs.hpp"
#include "fort/time/Time.hpp"
#include <Eigen/src/Core/Matrix.h>
#include <fort/myrmidon/Shapes.hpp>
#include <slog++/Attribute.hpp>
#include <utility>

namespace slog {

template <typename Str>
inline Attribute FortTime(Str &&name, const fort::Time &time) {
	return slog::String(std::forward<Str>(name), time.Format());
}

template <typename Str>
inline constexpr Attribute TagID(Str &&name, fort::myrmidon::TagID tagID) {
	return slog::Pointer(std::forward<Str>(name), (void *)(size_t{tagID}));
}

template <typename Str>
inline Attribute
Identification(Str &&name, const fort::myrmidon::priv::Identification &idt) {
	return slog::Group(
	    std::forward<Str>(name),
	    slog::TagID("tagID", idt.TagValue()),
	    slog::Int("antID", idt.Target()->AntID()),
	    slog::FortTime("start", idt.Start()),
	    slog::FortTime("end", idt.End())
	);
}

template <typename Str>
inline constexpr Attribute Vector2d(Str &&name, const Eigen::Vector2d &p) {
	return slog::Group(
	    std::forward<Str>(name),
	    slog::Float("x", p.x()),
	    slog::Float("y", p.y())
	);
}

template <typename Str>
inline constexpr Attribute
Capsule(Str &&name, const fort::myrmidon::Capsule &c) {
	return slog::Group(
	    std::forward<Str>(name),
	    Vector2d("c1", c.C1()),
	    slog::Float("r1", c.R1()),
	    Vector2d("c2", c.C2()),
	    slog::Float("r2", c.R2())
	);
}
} // namespace slog
