#pragma once

#include "Value.hpp"

namespace fort {
namespace myrmidon {

/**
 * Utilites function for Value
 */

struct ValueUtils  {
	/**
	 * Returns the type of a value.
	 */
	static ValueType Type(const Value & value);

	/**
	 * Returns the default value for an Value
	 */
	static Value Default(ValueType type);

	/**
	 * Parses to an Value
	 */
	static Value Parse(ValueType type, const std::string & name);

	/**
	 * Gives the type name of a ValueType
	 * @param type the type to query for its name
	 * @return the conventional name for type
	 * @throws std::invalid_argument if type is unknown
	 */

	static const std::string & TypeName(ValueType type);

	/**
	 * Gives the type name of a Value
	 * @param value the the value to query for its type's name
	 * @return the conventional name for type's value name
	 */
	static std::string TypeName(const Value & value);

	/**
	 * Represents a Time range assigned to a Value
	 *
	 * This Time range is valid for [Start;End[
	 */
	struct ValuedTimeRange {
		/**
		 * The Value on the Time range
		 */
		myrmidon::Value  Value;
		/**
		 * First valid time for the range.
		 */
		Time             Start;
		/**
		 * Last valid time for the range.
		 */
		Time             End;
	};
	/**
	 *  A List of ValuedTimeRange.
	 */
	typedef std::vector<ValuedTimeRange> ValuedTimeRangeList;

	/**
	 * Gets the ValuedTimeRange from a list of Time'd Value
	 * @param values a list of timed values, such as returned by Ant::GetValues()
	 * @return a ValuedTimeRangeList that would correspond to values
	 */
	static ValuedTimeRangeList BuildRanges(const std::map<Time,Value> & values);

	/**
	 * Finds the intersection of a ValuedTimeRange and a ValuedTimeRangeList
	 * @param ranges the ranges to find any intersection
	 * @param r the ValuedTimeRange to find any intersection
	 * @return the ValuedTimeRange in ranges that intersects with r
	 */
	static ValuedTimeRangeList IntersectionOf(const ValuedTimeRangeList & ranges,
	                                          const ValuedTimeRange & r);

	/**
	 * Represents the list of operation to perform with
	 * Ant::SetValue() and Ant::DeleteValue to merge a range in an
	 * existing list.
	 *
	 * @note it is implied that this operation only should happend on
	 * a given common key.
	 */
	struct Operations {
		/**
		 * Arguments to feed on Ant::SetValue().
		 */
		std::vector<std::tuple<Time,Value>> ToSet;
		/**
		 * Arguments to feed on Ant::DeleteValue()
		 */
		std::vector<Time>                   ToRemove;
	};

	/**
	 * Merges a ValuedTimeRange with a ValuedTimeRangeList by modifying the former.
	 *
	 * Once done, the ranges time boundaries not equal to defaultValue
	 * will not modified. However the range r may be modified or even
	 * splitted in several part.
	 *
	 * @param ranges the ranges to merge
	 * @param defaultValue value to be considered as an empty range
	 * @param r the range to merge
	 * @return the Operations to actually perform the merge
	 */
	static Operations MergeByReducingRange(const ValuedTimeRangeList & ranges,
	                                       const Value & defaultValue,
	                                       const ValuedTimeRange & r);
	/**
	 * Merges a ValuedTimeRange with a ValuedTimeRangeList by modifying the latter.
	 *
	 * Once done, the range r would be left unmodified, but
	 * ValuedTimeRange in ranges can be modified or suppressed.
	 *
	 * @param ranges the ranges to merge
	 * @param defaultValue value to be considered as an empty range
	 * @param r the range to merge
	 * @return the Operations to actually perform the merge
	 */
	static Operations MergeByOverwrittingRanges(const ValuedTimeRangeList & ranges,
	                                            const Value & defaultValue,
	                                            const ValuedTimeRange & r);

};

} // namespace myrmidon
} // namespace fort
