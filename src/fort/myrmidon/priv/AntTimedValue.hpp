#pragma once

#include <utility>
#include <string>
#include <unordered_map>
#include <vector>

#include <fort/time/Time.hpp>
#include <fort/myrmidon/types/Value.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

typedef std::pair<Time,Value>   TimedValue;

typedef std::unordered_map<std::string,std::vector<TimedValue> > AntDataMap;

} // namespace priv
} // namespace myrmidon
} // namespace fort
