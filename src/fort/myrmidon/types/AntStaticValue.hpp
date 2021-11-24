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
 * A c++ type that can hold only one of any #AntMetaDataType.
 */
typedef std::variant<bool,int32_t,double,std::string,Time> AntStaticValue;


/**
 * AntMetaDataType enumerates possible type for AntStaticValue
 *
 */
enum class AntMetaDataType {
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

AntMetaDataType TypeForAntStaticValue(const AntStaticValue & value);


} // namespace myrmidon
} // namespace fort


/**
 * C++ Formatting operator for AntStaticValue
 * @param out the std::ostream to format the value to
 * @param v the fort::myrmidon::AntStaticValue to format
 *
 * @return a reference to out
 */
std::ostream & operator<<(std::ostream & out, const fort::myrmidon::AntStaticValue & v);


bool operator==(const fort::myrmidon::AntStaticValue & a,
                const fort::myrmidon::AntStaticValue & b);
