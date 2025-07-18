#pragma once

#include <map>
#include <string>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

namespace thewarrior::models {

enum class MapTileTriggerEvent {
    None,
    SteppedOn,
    MoveUpPressed,
    MoveDownPressed,
    MoveLeftPressed,
    MoveRightPressed,
    ActionButtonPressed
};

enum class MapTileTriggerCondition {
    None,
    MustBeFacing,
    MustHaveItem
};

enum class MapTileTriggerAction {
    None,
    OpenChest,
    ChangeMap,
    DenyMove
};

class MapTileTrigger {
 public:
    MapTileTrigger();
    MapTileTrigger(MapTileTriggerEvent event,
                   MapTileTriggerCondition condition,
                   MapTileTriggerAction action,
                   const std::map<std::string, std::string> &actionProperties);
    friend bool operator==(const MapTileTrigger &lhs, const MapTileTrigger &rhs);
    friend bool operator!=(const MapTileTrigger &lhs, const MapTileTrigger &rhs);
    MapTileTriggerEvent getEvent() const;
    MapTileTriggerCondition getCondition() const;
    MapTileTriggerAction getAction() const;
    const std::map<std::string, std::string> &getActionProperties() const;
    void setEvent(MapTileTriggerEvent event);
    void setCondition(MapTileTriggerCondition condition);
    void setAction(MapTileTriggerAction action);
    void setActionProperties(const std::map<std::string, std::string> &properties);

 private:
    friend class boost::serialization::access;
    MapTileTriggerEvent m_event;
    MapTileTriggerCondition m_condition;
    MapTileTriggerAction m_action;
    std::map<std::string, std::string> m_actionProperties;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        if (version > 0) {
            ar & m_event;
            ar & m_condition;
            ar & m_action;
            ar & m_actionProperties;
        }
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::MapTileTrigger, 1)
