#include "mapTileTriggerConditionConverter.hpp"
#include <algorithm>
#include <boost/algorithm/string.hpp>

namespace thewarrior::models {

std::string MapTileTriggerConditionConverter::conditionToString(MapTileTriggerCondition condition) {
    return allConditionsToString()[static_cast<size_t>(condition)];
}

boost::optional<MapTileTriggerCondition> MapTileTriggerConditionConverter::conditionFromString(const std::string &conditionStr) {
    auto allConditionsStr { allConditionsToString() };

    auto iter { find_if(allConditionsStr.begin(), allConditionsStr.end(), [&conditionStr](const std::string &conditionStr2) {
        return boost::iequals(conditionStr, conditionStr2);
        }) };
    if (iter != allConditionsStr.end()) {
        return static_cast<MapTileTriggerCondition>(iter - allConditionsStr.begin());
    }
    return {};
}

std::vector<std::string> MapTileTriggerConditionConverter::allConditionsToString() {
    return { "None",
             "MustBeFacing",
             "MustHaveItem" };
}

}  // namespace thewarrior::models
