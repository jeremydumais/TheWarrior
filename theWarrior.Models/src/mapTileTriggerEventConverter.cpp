#include "mapTileTriggerEventConverter.hpp"
#include <algorithm>
#include <boost/algorithm/string.hpp>

namespace thewarrior::models {

std::string MapTileTriggerEventConverter::eventToString(MapTileTriggerEvent event) {
    return allEventsToString()[static_cast<size_t>(event)];
}

boost::optional<MapTileTriggerEvent> MapTileTriggerEventConverter::eventFromString(const std::string &eventStr) {
    auto allEventsStr { allEventsToString() };

    auto iter { find_if(allEventsStr.begin(), allEventsStr.end(), [&eventStr](const std::string &eventStr2) {
        return boost::iequals(eventStr, eventStr2);
        }) };
    if (iter != allEventsStr.end()) {
        return static_cast<MapTileTriggerEvent>(iter - allEventsStr.begin());
    }
    return {};
}

std::vector<std::string> MapTileTriggerEventConverter::allEventsToString() {
    return { "None",
             "SteppedOn",
             "MoveUpPressed",
             "MoveDownPressed",
             "MoveLeftPressed",
             "MoveRightPressed",
             "ActionButtonPressed" };
}

}  // namespace thewarrior::models
