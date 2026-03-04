#pragma once

#include <cpptrace/exceptions.hpp>

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
} // namespace utils
} // namespace myrmidon
} // namespace fort
