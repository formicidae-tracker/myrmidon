#pragma once

#include <utility>
#include <string>
#include <unordered_map>
#include <vector>

#include <fort/time/Time.hpp>
#include <fort/myrmidon/types/AntStaticValue.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

typedef std::pair<Time,AntStaticValue>   AntTimedValue;

typedef std::unordered_map<std::string,std::vector<AntTimedValue> > AntDataMap;
typedef std::unordered_map<std::string,const std::vector<AntTimedValue> > AntConstDataMap;

} // namespace priv
} // namespace myrmidon
} // namespace fort
