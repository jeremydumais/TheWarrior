#include "mapTileTriggerDTOUtils.hpp"
#include <fmt/format.h>
#include "mapTileDTOUtils.hpp"
#include "mapTileTrigger.hpp"
#include "mapTileTriggerActionConverter.hpp"
#include "mapTileTriggerConditionConverter.hpp"
#include "mapTileTriggerEventConverter.hpp"

using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerActionConverter;
using thewarrior::models::MapTileTriggerConditionConverter;
using thewarrior::models::MapTileTriggerEventConverter;

namespace mapeditor::controllers {

MapTileTriggerDTO MapTileTriggerDTOUtils::fromMapTileTrigger(const MapTileTrigger &trigger) {
    MapTileTriggerDTO dto;
    dto.event = MapTileTriggerEventConverter::eventToString(trigger.getEvent());
    dto.condition = MapTileTriggerConditionConverter::conditionToString(trigger.getCondition());
    dto.action = MapTileTriggerActionConverter::actionToString(trigger.getAction());
    dto.actionProperties = trigger.getActionProperties();
    return dto;
}

MapTileTrigger MapTileTriggerDTOUtils::toMapTileTrigger(const MapTileTriggerDTO &dto) {
    MapTileTrigger trigger;
    auto event = MapTileTriggerEventConverter::eventFromString(dto.event);
    if (event.has_value()) {
        trigger.setEvent(event.value());
    } else {
        throw std::invalid_argument(fmt::format("Unknown trigger event value : {0}", dto.event));
    }
    auto condition = MapTileTriggerConditionConverter::conditionFromString(dto.condition);
    if (condition.has_value()) {
        trigger.setCondition(condition.value());
    } else {
        throw std::invalid_argument(fmt::format("Unknown trigger condition value : {0}", dto.condition));
    }
    auto action = MapTileTriggerActionConverter::actionFromString(dto.action);
    if (action.has_value()) {
        trigger.setAction(action.value());
    } else {
        throw std::invalid_argument(fmt::format("Unknown trigger action value : {0}", dto.action));
    }
    trigger.setActionProperties(dto.actionProperties);
    return trigger;
}

}  // namespace mapeditor::controllers
