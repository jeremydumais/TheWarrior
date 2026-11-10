#pragma once

#include <string>
#include <map>
#include <optional>
#include "conversationScenario.hpp"

namespace mapeditor::controllers {

struct MapTileTriggerDTO {
    std::string event = "None";
    std::string condition = "None";
    std::string action = "None";
    std::map<std::string, std::string> actionProperties = {};
    std::optional<thewarrior::models::ConversationScenario> conversationScenario;
    bool operator==(const MapTileTriggerDTO &other) const {
        return event == other.event &&
            condition == other.condition &&
            action == other.action &&
            actionProperties == other.actionProperties &&
            conversationScenario == other.conversationScenario;
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
        } else if (actionProperties != other.actionProperties) {
            return actionProperties < other.actionProperties;
        } else if (conversationScenario.has_value() != other.conversationScenario.has_value()) {
            return conversationScenario.has_value() < other.conversationScenario.has_value();
        } else if (conversationScenario.has_value()) {
            return conversationScenario->getId() < other.conversationScenario->getId();
        } else {
            return false;
        }
    }
};

}  // namespace mapeditor::controllers
