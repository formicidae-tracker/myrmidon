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
	 * Finds ValuedTimeRange that conflicts with a set of timed Value
	 * @param values a set of timed values as returned by Ant::GetValues()
	 * @param defaultValue the value that is considered a default value
	 * @param r the ValuedTimeRange to find any conflict with
	 * @return the ValuedTimeRange that conflict with r
	 */
	static ValuedTimeRangeList FindConflicts(const std::map<Time,Value> & values,
	                                         const Value & defaultValue,
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
		std::vector<Time>                   ToDelete;
	};

	/**
	 * Merges a ValuedTimeRange with a list of timed value.
	 *
	 * This operation will kept the ValuedTimeRange defined by values
	 * which are not defaultValue intact. However r may have its
	 * boundaries modified, or be split in several ValuedTimeRange.
	 *
	 * @param values the values to merge
	 * @param defaultValue the Value to be considered as an empty ValuedTimeRange
	 * @param r the ValuedTimeRange to merge
	 * @return the Operations to actually perform the merge
	 */
	static Operations MergeRanges(const std::map<Time,Value> & values,
	                              const Value & defaultValue,
	                              const ValuedTimeRange & r);
	/**
	 * Overwrites a list of timed values to contain a ValuedTimeRange
	 *
	 * This operation will modifies values to ensure that the wanted
	 * ValuedTimeRange::Value is set over
	 * [ValuedTimeRange::Start;ValuedTimeRange::End[.
	 *
	 * @param values the values to merge with r
	 * @param r the range to ensure existance
	 * @return the Operations to actually perform the overwrite
	 */
	static Operations OverwriteRanges(const std::map<Time,Value> & values,
	                                  const ValuedTimeRange & r);

};

} // namespace myrmidon
} // namespace fort
