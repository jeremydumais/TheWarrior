#pragma once

#include <map>
#include <string>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/version.hpp>
#include <boost/variant.hpp>
#include "conversationScenario.hpp"

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
    DenyMove,
    ConversationScenario
};

struct LegacyMapTileTriggerAction {
    MapTileTriggerAction type = MapTileTriggerAction::None;
    std::map<std::string, std::string> properties;
    friend bool operator==(const LegacyMapTileTriggerAction &, const LegacyMapTileTriggerAction &) = default;

 private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & type;
        ar & properties;
    }
};

struct StartConversationMapTileTriggerAction {
    ConversationScenario scenario;
    friend bool operator==(const StartConversationMapTileTriggerAction &,
                           const StartConversationMapTileTriggerAction &) = default;

 private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & scenario;
    }
};

using MapTileTriggerActionData = boost::variant<
    LegacyMapTileTriggerAction,
    StartConversationMapTileTriggerAction>;

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
    const MapTileTriggerActionData &getActionData() const;
    const ConversationScenario *getConversationScenario() const;
    void setEvent(MapTileTriggerEvent event);
    void setCondition(MapTileTriggerCondition condition);
    void setAction(MapTileTriggerAction action);
    void setActionProperties(const std::map<std::string, std::string> &properties);
    void setConversationScenario(const ConversationScenario &scenario);

 private:
    friend class boost::serialization::access;
    MapTileTriggerEvent m_event;
    MapTileTriggerCondition m_condition;
    MapTileTriggerActionData m_actionData;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        if (version > 0) {
            ar & m_event;
            ar & m_condition;
            if (version >= 2) {
                ar & m_actionData;
            } else {
                MapTileTriggerAction legacyAction = MapTileTriggerAction::None;
                std::map<std::string, std::string> legacyProperties;
                ar & legacyAction;
                ar & legacyProperties;
                if constexpr (Archive::is_loading::value) {
                    m_actionData = LegacyMapTileTriggerAction {
                        .type = legacyAction,
                        .properties = legacyProperties
                    };
                }
            }
        }
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::MapTileTrigger, 2)
