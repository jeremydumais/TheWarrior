#include "../include/mapTileTrigger.hpp"
#include "mapTileTriggerEventConverter.hpp"

namespace thewarrior::models {

MapTileTrigger::MapTileTrigger()
    : m_event(MapTileTriggerEvent::None),
      m_condition(MapTileTriggerCondition::None),
      m_action(MapTileTriggerAction::None),
      m_actionProperties(std::map<std::string, std::string>()) {
}

MapTileTrigger::MapTileTrigger(MapTileTriggerEvent event,
                               MapTileTriggerCondition condition,
                               MapTileTriggerAction action,
                               const std::map<std::string, std::string> &actionProperties)
    : m_event(event),
      m_condition(condition),
      m_action(action),
      m_actionProperties(actionProperties) {
}

bool operator==(const MapTileTrigger &lhs, const MapTileTrigger &rhs) {
    return lhs.getEvent() == rhs.getEvent() &&
        lhs.getCondition() == rhs.getCondition() &&
        lhs.getAction() == rhs.getAction() &&
        lhs.getActionProperties() == rhs.getActionProperties();
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
    return m_action;
}

const std::map<std::string, std::string>& MapTileTrigger::getActionProperties() const {
    return m_actionProperties;
}

void MapTileTrigger::setEvent(MapTileTriggerEvent event) {
    m_event = event;
}

void MapTileTrigger::setCondition(MapTileTriggerCondition condition) {
    m_condition = condition;
}

void MapTileTrigger::setAction(MapTileTriggerAction action) {
    m_action = action;
}

void MapTileTrigger::setActionProperties(const std::map<std::string, std::string> &properties) {
    m_actionProperties = properties;
}

}  // namespace thewarrior::models
