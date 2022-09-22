#include "Value.hpp"

#include <iostream>
#include <iomanip>

namespace fort {
namespace myrmidon {

std::ostream & operator<<(std::ostream & out, const fort::myrmidon::Value & v) {
	auto flags = out.flags();
	std::visit([&out](auto && args) { out << std::boolalpha << args; },v);
	out.flags(flags);
	return out;
}

template <typename T>
inline static bool Equals(const fort::myrmidon::Value & a,
                          const T & b) {
	return std::get<T>(a) == b;
}

bool operator==(const fort::myrmidon::Value & a,
                const fort::myrmidon::Value & b) {


	if ( a.index() != b.index() ) {
		return false;
	}
	if ( a.index() == std::variant_npos ) {
		return true;
	}
	return std::visit([&](auto && arg) -> bool {
		                  return Equals(a,arg);
	                  },b);
}

} // namespace myrmidon
} // namespace fort
