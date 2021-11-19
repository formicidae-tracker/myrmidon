#pragma once

#include <type_traits>
#include <cstdint>

#include <fort/time/Time.hpp>

#include "TraitsCategory.hpp"

namespace fort {
namespace myrmidon {

template <typename T, typename = int>
struct has_end_field : std::false_type { };

template <typename T>
struct has_end_field <T, decltype((void) T::End, 0)> : std::true_type { };

template <typename T, typename = int>
struct has_end_member : std::false_type { };

template <typename T>
struct has_end_member <T, decltype(std::declval<T&>().End(), 0)> : std::true_type { };

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
struct data_traits {
	typedef typename T::data_category data_category;
	const static bool spaced_data = has_space_field<T>::value;

	template <typename Actual = T,
	          std::enable_if_t<data_traits<Actual>::spaced_data,bool> = true>
	static uint32_t space(const Actual & v) {
		return v.Space;
	}

	template <typename Actual = T,
	          std::enable_if_t<std::is_same<typename data_traits<Actual>::data_category,
	                                        timed_data>::value,bool> = true>
	static const fort::Time & time(const Actual & v) {
		return v.FrameTime;
	}

	template <typename Actual = T,
	          std::enable_if_t<std::is_same<typename data_traits<Actual>::data_category,
	                                        time_ranged_data>::value,bool> = true>
	static const fort::Time & start(const Actual & v) {
		return v.Start;
	}

	template <typename Actual = T,
	          std::enable_if_t<std::is_same<typename data_traits<Actual>::data_category,
	                                        time_ranged_data>::value
	                           && has_end_field<Actual>::value,bool> = true>
	static const fort::Time & end(const Actual & v) {
		return v.End;
	}

	template <typename Actual = T,
	          std::enable_if_t<std::is_same<typename data_traits<Actual>::data_category,
	                                        time_ranged_data>::value
	                           && has_end_member<Actual>::value,bool> = true>
	static fort::Time end(const Actual & v) {
		return v.End();
	}

	template <typename Actual = T,
	          std::enable_if_t<std::is_same<typename data_traits<Actual>::data_category,
	                                        timed_data>::value,bool> = true>
	static inline bool compare(const Actual & a,
	                           const Actual & b) {
		return data_traits<T>::time(a) < data_traits<T>::time(b);
	}

	template <typename Actual = T,
	          std::enable_if_t<std::is_same<typename data_traits<Actual>::data_category,
	                                        time_ranged_data>::value,bool> = true>
	static inline bool compare(const Actual & a,
	                           const Actual & b) {
		return data_traits<T>::end(a) < data_traits<T>::end(b);
	}
};


} /* namespace myrmidon */
} /* namespace fort */
