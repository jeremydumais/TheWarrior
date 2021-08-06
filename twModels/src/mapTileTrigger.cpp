#include "mapTileTrigger.hpp"

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
