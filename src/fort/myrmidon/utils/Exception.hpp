#pragma once

#include <cpptrace/exceptions.hpp>
#include <slog++/Attribute.hpp>

namespace fort {
namespace myrmidon {
namespace utils {
inline const char *What(const std::exception &e) {
	auto te = dynamic_cast<const cpptrace::exception *>(&e);
	if (te == nullptr) {
		return e.what();
	}
	return te->message();
}

inline ::slog::Attribute Err(const std::exception &e) {
	auto te = dynamic_cast<const cpptrace::exception *>(&e);
	if (te == nullptr) {
		return ::slog::Err(e.what());
	}
	return ::slog::Group(
	    "error",
	    ::slog::String("message", te->message()),
	    ::slog::MapContainer(
	        "stacktrace",
	        te->trace().begin(),
	        te->trace().end(),
	        [](std::string &&key, const auto &a) {
		        return slog::String(std::move(key), a.to_string());
	        }
	    )
	);
}
} // namespace utils
} // namespace myrmidon
} // namespace fort
