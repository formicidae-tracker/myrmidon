#pragma once

#include <type_traits>

#include "TraitCategory.hpp"
#include "Typedefs.hpp"
#include "MaybeDeref.hpp"

namespace fort {
namespace myrmidon {

template <typename T, typename = int>
struct has_end_field : std::false_type { };

template <typename T>
struct has_end_field <T, decltype((void) T::End, 0)> : std::true_type { };

template <typename T, typename = int>
struct has_end_member : std::false_type { };

template <typename T>
struct has_end_member <T, decltype((void) T::End(), 0)> : std::true_type { };

template <typename T, typename = int>
struct has_space_field : std::false_type { };

template <typename T>
struct has_space_field <T, decltype((void) T::Space, 0)> : std::true_type { };


template <typename T>
class pointed_type_if_any {
	template <typename U=T>
	static auto test(int) -> std::remove_reference<decltype(*std::declval<U>())>;
	static auto test(...) -> std::remove_cv<T>;

public:
    using type = typename decltype(test(0))::type;
};

template <typename T>
struct data_trait {
	typedef typename T::data_category data_category;
	const static bool spaced_data = has_space_field<T>::value;

	template <std::enable_if_t<spaced_data,bool> = true>
	static uint32_t space(const T & v) {
		return v.Space;
	}

	template <std::enable_if_t<std::is_same<data_category,timed_data>::value,bool> = true>
	static const fort::Time & time(const T & v) {
		return v.FrameTime;
	}

	template <std::enable_if_t<std::is_same<data_category,time_ranged_data>::value,bool> = true>
	static const fort::Time & start(const T & v) {
		return v.Start;
	}

	template <std::enable_if_t<std::is_same<data_category,time_ranged_data>::value
	                           && has_end_field<T>::value,bool> = true>
	static const fort::Time & end(const T & v) {
		return v.End;
	}

	template <std::enable_if_t<std::is_same<data_category,time_ranged_data>::value
	                           && has_end_member<T>::value,bool> = true>
	static fort::Time end(const T & v) {
		return v.End();
	}

	template <std::enable_if_t<std::is_same<data_category,timed_data>::value,bool> = true>
	static inline bool compare(const T & a,
	                           const T & b) {
		return data_trait<T>::time(a) < data_trait<T>::time(b);
	}

	template <std::enable_if_t<std::is_same<data_category,time_ranged_data>::value,bool> = true>
	static inline bool compare(const T & a,
	                           const T & b) {
		return data_trait<T>::end(a) < data_trait<T>::end(b);
	}
};


} /* namespace myrmidon */
} /* namespace fort */
