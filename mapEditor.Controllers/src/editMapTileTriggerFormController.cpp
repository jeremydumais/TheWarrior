#include "editMapTileTriggerFormController.hpp"
#include <fmt/format.h>
#include <stdexcept>
#include "mapTileTrigger.hpp"
#include "mapTileTriggerActionConverter.hpp"
#include "mapTileTriggerConditionConverter.hpp"
#include "mapTileTriggerEventConverter.hpp"
#include "mapTileTriggerDTOUtils.hpp"

using thewarrior::models::MapTileTriggerAction;
using thewarrior::models::MapTileTriggerActionConverter;
using thewarrior::models::MapTileTriggerConditionConverter;
using thewarrior::models::MapTileTriggerEvent;
using thewarrior::models::MapTileTriggerEventConverter;

namespace mapeditor::controllers {

EditMapTileTriggerFormController::EditMapTileTriggerFormController(const boost::optional<mapeditor::controllers::MapTileTriggerDTO> currentTrigger,
        const std::vector<MapTileTriggerDTO> &allTriggers)

    : m_currentTrigger(currentTrigger),
      m_allTriggers(allTriggers),
      m_updatedTrigger(currentTrigger.has_value() ?
            currentTrigger.value() :
            MapTileTriggerDTO {}),
      m_lastError("") {
}

const boost::optional<mapeditor::controllers::MapTileTriggerDTO> &EditMapTileTriggerFormController::getCurrentTrigger() const {
    return m_currentTrigger;
}

int EditMapTileTriggerFormController::getTriggerEventIndex() const {
    if (m_currentTrigger.has_value()) {
        auto event = MapTileTriggerEventConverter::eventFromString(m_currentTrigger->event);
        if (event.has_value()) {
            return static_cast<int>(event.value());
        }
    }
    return -1;
}

int EditMapTileTriggerFormController::getTriggerConditionIndex() const {
    if (m_currentTrigger.has_value()) {
        auto condition = MapTileTriggerConditionConverter::conditionFromString(m_currentTrigger->condition);
        if (condition.has_value()) {
            return static_cast<int>(condition.value());
        }
    }
    return -1;
}

int EditMapTileTriggerFormController::getTriggerActionIndex() const {
    if (m_currentTrigger.has_value()) {
        auto action = MapTileTriggerActionConverter::actionFromString(m_currentTrigger->action);
        if (action.has_value()) {
            return static_cast<int>(action.value());
        }
    }
    return -1;
}

const MapTileTriggerDTO &EditMapTileTriggerFormController::getUpdatedTrigger() const {
    return m_updatedTrigger;
}

const std::string &EditMapTileTriggerFormController::getLastError() const {
    return m_lastError;
}

bool EditMapTileTriggerFormController::isUpdatedTriggerValid() {
    try {
        auto trigger =  MapTileTriggerDTOUtils::toMapTileTrigger(m_updatedTrigger);
        if (trigger.getEvent() == MapTileTriggerEvent::None) {
            m_lastError = "You must select an event!";
            return false;
        }
        if (trigger.getAction() == MapTileTriggerAction::None) {
            m_lastError = "You must select an action!";
            return false;
        }
    } catch (std::invalid_argument &err) {
        m_lastError = fmt::format("Invalid values for MapTileTriggerDTO. Error : {0}", err.what());
        return false;
    }
    return true;
}

void EditMapTileTriggerFormController::setEvent(const std::string &event) {
    m_updatedTrigger.event = event;
}

void EditMapTileTriggerFormController::setCondition(const std::string &condition) {
    m_updatedTrigger.condition = condition;
}

void EditMapTileTriggerFormController::setAction(const std::string &action) {
    m_updatedTrigger.action = action;
}

void EditMapTileTriggerFormController::setActionProperties(const std::map<std::string, std::string> &props) {
    m_updatedTrigger.actionProperties = props;
}


}  // namespace mapeditor::controllers
