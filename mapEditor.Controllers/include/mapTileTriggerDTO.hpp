#pragma once

#include <string>
#include <map>

namespace mapeditor::controllers {

struct MapTileTriggerDTO {
    std::string event = "None";
    std::string condition = "None";
    std::string action = "None";
    std::map<std::string, std::string> actionProperties = {};
    bool operator==(const MapTileTriggerDTO &other) const {
        return event == other.event &&
            condition == other.condition &&
            action == other.action &&
            actionProperties == other.actionProperties;
    }
    bool operator!=(const MapTileTriggerDTO &other) const {
        return !(*this == other);
    }
    bool operator<(const MapTileTriggerDTO &other) const {
        // Compare fields in a specific order
        if (event != other.event) {
            return event < other.event;
        } else if (condition != other.condition) {
            return condition < other.condition;
        } else if (action != other.action) {
            return action < other.action;
        } else {
            return actionProperties < other.actionProperties;
        }
    }
};

}  // namespace mapeditor::controllers
