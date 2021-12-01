#pragma once

#include <variant>
#include <cstdint>
#include <string>
#include <iostream>

#include <fort/time/Time.hpp>

namespace fort {
namespace myrmidon {

/**
 * C++ type for named values.
 *
 * A c++ type that can hold only one of any ValueType.
 */
typedef std::variant<bool,int32_t,double,std::string,Time> Value;


/**
 * AntMetaDataType enumerates possible type for Value
 *
 */
enum class ValueType {
                      /**
                       * A boolean
                       */
                      BOOL = 0,
                      /**
                       * An integer
                       */
                      INT,
                      /**
                       * a float
                       */
                      DOUBLE,
                      /**
                       * a std::string
                       */
                      STRING,
                      /**
                       * a Time
                       */
                      TIME,
};

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
};


} // namespace myrmidon
} // namespace fort


/**
 * C++ Formatting operator for Value
 * @param out the std::ostream to format the value to
 * @param v the fort::myrmidon::Value to format
 *
 * @return a reference to out
 */
std::ostream & operator<<(std::ostream & out, const fort::myrmidon::Value & v);


bool operator==(const fort::myrmidon::Value & a,
                const fort::myrmidon::Value & b);
