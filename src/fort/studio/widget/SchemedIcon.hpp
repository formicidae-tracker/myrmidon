#pragma once

#include <concepts>

#include <QApplication>
#include <QIcon>
#include <QPainter>
#include <QString>
#include <QStyleHints>

#include <qapplication.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <slog++/slog++.hpp>

namespace details {
template <typename T>
concept HasSetIcon = requires(T * object, const QIcon &icon) {
	{ object->setIcon(icon) } -> std::same_as<void>;
};
} // namespace details

template <typename T>
requires details::HasSetIcon<T> inline void
         addSchemedIcon(T *object, const QString &filepath) {

	using PixmapCache = QMap<Qt::ColorScheme, QPixmap>;

	auto setIconFromScheme = [object,
	                          pixmap = QIcon(filepath).pixmap({48, 48}),
	                          cache  = PixmapCache{},
	                          filepath](Qt::ColorScheme scheme) mutable {
		if (cache.contains(scheme) == false) {
			auto   color = QApplication::palette().buttonText().color();
			QImage img   = pixmap.toImage().convertToFormat(
                QImage::Format_ARGB32_Premultiplied
            );
			QPainter p(&img);
			p.setCompositionMode(QPainter::CompositionMode_SourceIn);
			p.fillRect(img.rect(), color);
			p.end();

			cache[scheme] = QPixmap::fromImage(img);
		}
		object->setIcon(cache[scheme]);
	};

	setIconFromScheme(QApplication::styleHints()->colorScheme());

	object->connect(
	    QApplication::styleHints(),
	    &QStyleHints::colorSchemeChanged,
	    object,
	    std::move(setIconFromScheme),
	    Qt::QueuedConnection
	);
}
