#pragma once

#include <cpptrace/exceptions.hpp>
#include <slog++/Attribute.hpp>
#include <type_traits>

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

template <typename T, typename Str>
requires std::derived_from<std::decay_t<T>, cpptrace::exception_with_message>      &&
    std::is_convertible_v<std::decay_t<Str>, std::string> static inline T
         Wrap(const std::exception &e, Str &&message) {
    auto te = dynamic_cast<const cpptrace::lazy_exception *>(&e);
    if (te == nullptr) {
        return T{std::string{std::forward<Str>(message)} + ": " + e.what()};
    }
    return te->wrap<T>(
        std::string{std::forward<Str>(message)} + ": " + te->message()
    );
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
