#pragma once

#include "fort/studio/widget/vectorgraphics/Capsule.hpp"
#include "fort/studio/widget/vectorgraphics/Vector.hpp"
#include <QColor>
#include <QDebug>
#include <QModelIndex>
#include <QPointF>
#include <QString>

#include <slog++/Attribute.hpp>

namespace slog {
template <typename Str>
inline constexpr Attribute QColor(Str &&name, const QColor &color) {
	return slog::Group(
	    std::forward<Str>(name),
	    slog::Int("R", color.red()),
	    slog::Int("G", color.green()),
	    slog::Int("B", color.blue()),
	    slog::Int("A", color.alpha())
	);
}

template <typename Str>
inline constexpr Attribute QPointF(Str &&name, const QPointF &p) {
	return slog::Group(
	    std::forward<Str>(name),
	    slog::Float("x", p.x()),
	    slog::Float("y", p.y())
	);
}

template <typename Str>
inline Attribute QModelIndex(Str &&name, const QModelIndex &index) {
	QString result;
	QDebug(&result).nospace() << index;

	return slog::String(std::forward<Str>(name), result.toStdString());
}

template <typename Str>
inline constexpr Attribute Vector(Str &&name, const Vector &v) {
	return slog::Group(
	    std::forward<Str>(name),
	    slog::QPointF("start", v.startPos()),
	    slog::QPointF("end", v.endPos())
	);
}

template <typename Str>
inline constexpr Attribute Capsule(Str &&name, const Capsule &v) {
	return slog::Group(
	    std::forward<Str>(name),
	    slog::QPointF("C1", v.c1Pos()),
	    slog::Float("R1", v.r1()),
	    slog::QPointF("C2", v.c2Pos()),
	    slog::Float("R2", v.r2())
	);
}

} // namespace slog
