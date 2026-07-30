#include "mapTileTriggerActionConverter.hpp"
#include <algorithm>
#include <boost/algorithm/string.hpp>

namespace thewarrior::models {

std::string MapTileTriggerActionConverter::actionToString(MapTileTriggerAction action) {
    return allActionsToString()[static_cast<size_t>(action)];
}

boost::optional<MapTileTriggerAction> MapTileTriggerActionConverter::actionFromString(const std::string &actionStr) {
    auto allActionsStr { allActionsToString() };

    auto iter { std::ranges::find_if(allActionsStr, [&actionStr](const std::string &actionStr2) {
        return boost::iequals(actionStr, actionStr2);
        }) };
    if (iter != allActionsStr.end()) {
        return static_cast<MapTileTriggerAction>(iter - allActionsStr.begin());
    }
    return {};
}

std::vector<std::string> MapTileTriggerActionConverter::allActionsToString() {
    return { "None",
             "OpenChest",
             "ChangeMap",
             "DenyMove",
             "ConversationScenario" };
}

}  // namespace thewarrior::models
