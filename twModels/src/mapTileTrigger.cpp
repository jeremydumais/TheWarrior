#include "mapTileTrigger.hpp"
#include "mapTileTriggerEventConverter.hpp"

using namespace std;

MapTileTrigger::MapTileTrigger() 
    : event(MapTileTriggerEvent::None),
      condition(MapTileTriggerCondition::None),
      action(MapTileTriggerAction::None),
      actionProperties(map<string, string>())
{
}

MapTileTrigger::MapTileTrigger(MapTileTriggerEvent event, 
                               MapTileTriggerCondition condition, 
                               MapTileTriggerAction action,
                               const std::map<std::string, std::string> &actionProperties) 
    : event(event),
      condition(condition),
      action(action),
      actionProperties(actionProperties)
{
}

MapTileTriggerEvent MapTileTrigger::getEvent() const
{
    return event;
}

MapTileTriggerCondition MapTileTrigger::getCondition() const
{
    return condition;
}

MapTileTriggerAction MapTileTrigger::getAction() const
{
    return action;
}

const std::map<std::string, std::string>& MapTileTrigger::getActionProperties() const
{
    return actionProperties;
}

void MapTileTrigger::setEvent(MapTileTriggerEvent event) 
{
    this->event = event;
}

void MapTileTrigger::setCondition(MapTileTriggerCondition condition) 
{
    this->condition = condition;
}

void MapTileTrigger::setAction(MapTileTriggerAction action) 
{
    this->action = action;
}

void MapTileTrigger::setActionProperties(const std::map<std::string, std::string> &properties)
{
    this->actionProperties = properties;
}
