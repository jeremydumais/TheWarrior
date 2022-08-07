#pragma once

#include "mapTileTrigger.hpp"
#include <boost/optional.hpp>
#include <string>
#include <vector>

namespace thewarrior::models {

class MapTileTriggerConditionConverter
{
public:
    MapTileTriggerConditionConverter() = delete;
    static std::string conditionToString(MapTileTriggerCondition condition);
    static boost::optional<MapTileTriggerCondition> conditionFromString(const std::string &conditionStr);
    static std::vector<std::string> allConditionsToString();
};

} // namespace thewarrior::models
