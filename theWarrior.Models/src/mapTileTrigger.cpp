#include "../include/mapTileTrigger.hpp"
#include "mapTileTriggerEventConverter.hpp"

namespace thewarrior::models {

MapTileTrigger::MapTileTrigger()
    : m_event(MapTileTriggerEvent::None),
      m_condition(MapTileTriggerCondition::None),
      m_actionData(LegacyMapTileTriggerAction {}) {
}

MapTileTrigger::MapTileTrigger(MapTileTriggerEvent event,
                               MapTileTriggerCondition condition,
                               MapTileTriggerAction action,
                               const std::map<std::string, std::string> &actionProperties)
    : m_event(event),
      m_condition(condition),
      m_actionData(LegacyMapTileTriggerAction {
          .type = action,
          .properties = actionProperties
      }) {
}

bool operator==(const MapTileTrigger &lhs, const MapTileTrigger &rhs) {
    return lhs.getEvent() == rhs.getEvent() &&
        lhs.getCondition() == rhs.getCondition() &&
        lhs.getActionData() == rhs.getActionData();
}

bool operator!=(const MapTileTrigger &lhs, const MapTileTrigger &rhs) {
    return !(lhs == rhs);
}

MapTileTriggerEvent MapTileTrigger::getEvent() const {
    return m_event;
}

MapTileTriggerCondition MapTileTrigger::getCondition() const {
    return m_condition;
}

MapTileTriggerAction MapTileTrigger::getAction() const {
    if (const auto *legacy = boost::get<LegacyMapTileTriggerAction>(&m_actionData)) {
        return legacy->type;
    }
    return MapTileTriggerAction::ConversationScenario;
}

const std::map<std::string, std::string>& MapTileTrigger::getActionProperties() const {
    static const std::map<std::string, std::string> EmptyProperties;
    const auto *legacy = boost::get<LegacyMapTileTriggerAction>(&m_actionData);
    return legacy == nullptr ? EmptyProperties : legacy->properties;
}

const MapTileTriggerActionData &MapTileTrigger::getActionData() const {
    return m_actionData;
}

const ConversationScenario *MapTileTrigger::getConversationScenario() const {
    const auto *action = boost::get<StartConversationMapTileTriggerAction>(&m_actionData);
    return action == nullptr ? nullptr : &action->scenario;
}

void MapTileTrigger::setEvent(MapTileTriggerEvent event) {
    m_event = event;
}

void MapTileTrigger::setCondition(MapTileTriggerCondition condition) {
    m_condition = condition;
}

void MapTileTrigger::setAction(MapTileTriggerAction action) {
    if (auto *legacy = boost::get<LegacyMapTileTriggerAction>(&m_actionData)) {
        legacy->type = action;
        return;
    }
    m_actionData = LegacyMapTileTriggerAction {
        .type = action,
        .properties = {}
    };
}

void MapTileTrigger::setActionProperties(const std::map<std::string, std::string> &properties) {
    if (auto *legacy = boost::get<LegacyMapTileTriggerAction>(&m_actionData)) {
        legacy->properties = properties;
        return;
    }
    m_actionData = LegacyMapTileTriggerAction {
        .type = MapTileTriggerAction::None,
        .properties = properties
    };
}

void MapTileTrigger::setConversationScenario(const ConversationScenario &scenario) {
    m_actionData = StartConversationMapTileTriggerAction { .scenario = scenario };
}

}  // namespace thewarrior::models
