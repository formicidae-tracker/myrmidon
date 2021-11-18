#include "AntStaticValue.hpp"

#include <type_traits>

namespace fort {
namespace myrmidon {

template<class> inline constexpr bool always_false_v = false;

AntMetaDataType TypeForAntStaticValue(const AntStaticValue & value) {
	return std::visit([](auto&& arg) -> AntMetaDataType {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, bool>) {
			return AntMetaDataType::BOOL;
		} else if constexpr (std::is_same_v<T, int>) {
			return AntMetaDataType::INT;
		} else if constexpr (std::is_same_v<T, double>) {
			return AntMetaDataType::DOUBLE;
		} else if constexpr (std::is_same_v<T, std::string>) {
			return AntMetaDataType::STRING;
		} else if constexpr (std::is_same_v<T, fort::Time>) {
			return AntMetaDataType::TIME;
		} else {
			static_assert(always_false_v<T>, "non-exhaustive visitor!");
		}
	}, value);
}

} // namespace myrmidon
} // namespace fort

std::ostream & operator<<(std::ostream & out, const fort::myrmidon::AntStaticValue & v) {
	auto flags = out.flags();
	std::visit([&out](auto && args) { out << std::boolalpha << args; },v);
	out.flags(flags);
	return out;
}
