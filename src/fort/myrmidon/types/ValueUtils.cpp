#include "ValueUtils.hpp"

#include <type_traits>
#include <iomanip>

namespace fort {
namespace myrmidon {

template<class> inline constexpr bool always_false_v = false;

ValueType ValueUtils::Type(const Value & value) {
	return std::visit([](auto&& arg) -> ValueType {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, bool>) {
			return ValueType::BOOL;
		} else if constexpr (std::is_same_v<T, int>) {
			return ValueType::INT;
		} else if constexpr (std::is_same_v<T, double>) {
			return ValueType::DOUBLE;
		} else if constexpr (std::is_same_v<T, std::string>) {
			return ValueType::STRING;
		} else if constexpr (std::is_same_v<T, fort::Time>) {
			return ValueType::TIME;
		} else {
			static_assert(always_false_v<T>, "non-exhaustive visitor!");
		}
	}, value);
}

const std::string & ValueUtils::TypeName(ValueType type) {
	static std::vector<std::string> names =
		{ "Bool","Int","Double","String","Time"};
	size_t idx = size_t(type);
	if ( idx >= names.size() ) {
		throw std::invalid_argument("Unknown ValueType value "
		                            + std::to_string(idx));
	}
	return names.at(idx);
}

std::string ValueUtils::TypeName(const Value & value) {
	return TypeName(Type(value));
}


Value ValueUtils::Default(ValueType type) {
	static std::vector<Value> defaults =
		{
		 false,
		 0,
		 0.0,
		 std::string(""),
		 Time(),
		};

	size_t idx = size_t(type);
	if ( idx >= defaults.size() ) {
		throw std::invalid_argument("Unknown ValueType value "
		                            + std::to_string(idx));
	}

	return defaults[idx];
}

Value ValueUtils::Parse(ValueType type, const std::string & str) {
	auto value = Default(type);
	std::visit([&str](auto &&arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr ( std::is_same_v<T, bool> ) {
			std::istringstream iss(str);
			bool res;
			iss >> std::boolalpha >> res;
			if ( iss.good() == false ) {
				throw std::invalid_argument("Invalid string '"
				                            + str
				                            + "' for AntMetadata::Value");
			 }
			 arg = res;
		} else if constexpr ( std::is_same_v<T,int32_t> ) {
			arg = std::stoi(str);
		} else if constexpr ( std::is_same_v<T,double> ) {
			arg = std::stod(str);
		} else if constexpr ( std::is_same_v<T,std::string> ) {
			arg = str;
		} else if constexpr ( std::is_same_v<T,Time> ) {
			arg = Time::Parse(str);
		} else {
			static_assert(always_false_v<T>, "non-exhaustive visitor!");
		}
	},value);
	return value;
}

ValueUtils::ValuedTimeRangeList
ValueUtils::BuildRanges(const std::map<Time,Value> & values) {
	if ( values.empty() ) {
		return {};
	}
	ValuedTimeRangeList res;
	res.reserve(values.size()+1);

	using M = std::map<Time,Value>;
	M::const_iterator prev = values.cbegin();
	for ( auto it = values.cbegin()++; it != values.cend(); ++it) {
		if ( prev->second == it->second ) {
			continue;
		}
		res.push_back({.Value = prev->second,
		               .Start = prev->first,
		               .End = it->first});
		prev = it;
	}

	res.push_back({.Value = prev->second,
	               .Start = prev->first,
	               .End = fort::Time::Forever()});
	return res;
}


ValueUtils::ValuedTimeRangeList
ValueUtils::FindConflicts(const std::map<Time,Value> & values,
                          const Value & defaultValue,
                          const ValuedTimeRange & r) {
	ValueUtils::ValuedTimeRangeList res;
	for ( const auto & u : BuildRanges(values) ) {
		if ( r.End <= u.Start || u.End <= r.Start ) {
			continue;
		}
		if ( u.Value == defaultValue || u.Value == r.Value ) {
			continue;
		}
		res.push_back(u);
	}
	return res;
}

std::map<Time,Value>::const_iterator
previous(const std::map<Time,Value> & values,
         const fort::Time & time) {
	if ( values.empty() || values.begin()->first.IsSinceEver() == false ) {
		throw std::invalid_argument("values does not contain -∞ time");
	}
	return std::prev(values.upper_bound(time));
}


void CleanUp(const std::map<Time,Value> & values,
             ValueUtils::Operations & operations) {
	std::map<Time,Value> finalValues = values;
	for ( const auto & [time,value] : operations.ToSet ) {
		finalValues[time] = value;
	}
	for ( const auto & time : operations.ToDelete ) {
		finalValues.erase(time);
	}

	if ( finalValues.empty() ) {
		return;
	}
	auto prev = finalValues.begin();
	for ( auto it = std::next(prev);
	      it != finalValues.end();
	      ++it) {
		if ( ! ( prev->second == it->second ) ) {
			prev = it;
			continue;
		}
		// duplicate value found
		operations.ToSet.erase(std::remove_if(operations.ToSet.begin(),
		                                      operations.ToSet.end(),
		                                      [&it](const std::tuple<Time,Value> & t) {
			                                      return std::get<0>(t) == it->first;
		                                      }),
		                       operations.ToSet.end());
		if ( values.count(it->first) ) {
			operations.ToDelete.push_back(it->first);
		}
	}

	std::sort(operations.ToDelete.begin(),
	          operations.ToDelete.end());
}


ValueUtils::Operations
ValueUtils::OverwriteRanges(const std::map<Time,Value> & values,
                            const ValuedTimeRange & r) {
	Operations res;
	if ( r.Start == r.End ) {
		return res;
	}
	res.ToSet.push_back({r.Start,r.Value});
	auto it = previous(values,r.Start);
	for (++it; it != values.lower_bound(r.End); ++it) {
		res.ToDelete.push_back(it->first);
	}
	if ( r.End.IsForever() == false
	     && ( it == values.end() || r.End < it->first ) ) {
		res.ToSet.push_back({r.End,std::prev(it)->second});
	}
	CleanUp(values,res);
	return res;
}

ValueUtils::Operations
ValueUtils::MergeRanges(const std::map<Time,Value> & values,
                        const Value & defaultValue,
                        const ValuedTimeRange & r) {

	Operations res;
	if ( r.Start == r.End || r.Value == defaultValue ) {
		return res;
	}
	auto it = previous(values,r.Start);
	bool lastWasSet = false;
	if ( it->second == defaultValue ) {
		lastWasSet = true;
		res.ToSet.push_back({r.Start,r.Value});
	}

	for(++it; it != values.lower_bound(r.End); ++it) {
		if ( it->second == defaultValue ) {
			lastWasSet = true;
			res.ToSet.push_back({it->first,r.Value});
		} else {
			lastWasSet = false;
		}
	}

	if ( lastWasSet == true
	     && r.End.IsForever() == false
	     && ( it == values.end() || r.End < it->first ) ) {
		res.ToSet.push_back({r.End,defaultValue});
	}

	CleanUp(values,res);
	return res;
}


} // namespace myrmidon
} // namespace fort
