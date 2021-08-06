#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <map>
#include <string>

enum class MapTileTriggerEvent
{
    None,
    SteppedOn,
    MoveUpPressed,
    MoveDownPressed,
    MoveLeftPressed,
    MoveRightPressed,
    ActionButtonPressed
};

enum class MapTileTriggerCondition
{
    None,
    MustBeFacing,
    MustHaveItem
};

enum class MapTileTriggerAction
{
    None,
    OpenChest,
    ChangeMap
};

class MapTileTrigger 
{
public:
    MapTileTrigger();
    MapTileTrigger(MapTileTriggerEvent event, 
                   MapTileTriggerCondition condition, 
                   MapTileTriggerAction action,
                   const std::map<std::string, std::string> &actionProperties);
    MapTileTriggerEvent getEvent() const;
    MapTileTriggerCondition getCondition() const;
    MapTileTriggerAction getAction() const;
    const std::map<std::string, std::string> &getActionProperties() const;
private:
    friend class boost::serialization::access;
    MapTileTriggerEvent event;
    MapTileTriggerCondition condition;
    MapTileTriggerAction action;
    std::map<std::string, std::string> actionProperties;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        if(version > 2) {
            ar & event;
            ar & condition;
            ar & action;
            ar & actionProperties;
        }
    }
};



