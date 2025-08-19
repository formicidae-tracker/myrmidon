#pragma once

#include <fort/time/Time.hpp>

#include <limits>
#include <map>
#include <sstream>
#include <unordered_map>

#include <cpptrace/cpptrace.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

template <typename T, typename U> class TimeMap {
public:
	inline void Insert(const T &key, const U &value, const Time &time) {
		if (time.IsForever() == true) {
			throw cpptrace::invalid_argument("time value cannot be +∞");
		}
		auto fi = d_map.find(key);
		if (fi == d_map.end()) {
			auto res = d_map.insert(std::make_pair(key, ValuesByTime()));
			fi       = res.first;
		}
		fi->second.insert(std::make_pair(time, value));
	}

	inline void InsertOrAssign(const T &key, const U &value, const Time &time) {
		if (time.IsForever() == true) {
			throw cpptrace::invalid_argument("time value cannot be +∞");
		}
		auto fi = d_map.find(key);
		if (fi == d_map.end()) {
			auto res = d_map.insert(std::make_pair(key, ValuesByTime()));
			fi       = res.first;
		}
		fi->second.insert_or_assign(time, value);
	}

	inline size_t Count(const T &key) const noexcept {
		try {
			return d_map.at(key).size();
		} catch (const std::exception &) {
			return 0;
		}
	}

	inline const U &At(const T &key, const Time &t) const {
		auto fi = d_map.find(key);
		if (fi == d_map.end() || fi->second.empty()) {
			throw cpptrace::out_of_range("Invalid key");
		}
		auto ti = fi->second.upper_bound(t);
		if (ti == fi->second.begin()) {
			throw cpptrace::out_of_range("Invalid time");
		}
		return std::prev(ti)->second;
	}

	inline void Clear() {
		d_map.clear();
	}

	const std::map<Time, U> &Values(const T &key) const {
		return d_map.at(key);
	}

	std::string DebugString() const {
		std::ostringstream oss;
		oss << "keys: " << d_map.size() << std::endl;
		for (const auto &[key, valueByTime] : d_map) {
			for (const auto &[time, value] : valueByTime) {
				oss << " + [" << key << "]{" << time << "}" << value
				    << std::endl;
			}
		}

		return oss.str();
	}

private:
	typedef std::map<Time, U> ValuesByTime;

	std::unordered_map<T, ValuesByTime> d_map;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
