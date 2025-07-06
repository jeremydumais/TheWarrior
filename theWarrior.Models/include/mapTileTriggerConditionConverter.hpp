#pragma once

#include <string>
#include <vector>
#include "mapTileTrigger.hpp"
#include <boost/optional.hpp>

namespace thewarrior::models {

class MapTileTriggerConditionConverter {
 public:
    MapTileTriggerConditionConverter() = delete;
    static std::string conditionToString(MapTileTriggerCondition condition);
    static boost::optional<MapTileTriggerCondition> conditionFromString(const std::string &conditionStr);
    static std::vector<std::string> allConditionsToString();
};

}  // namespace thewarrior::models
