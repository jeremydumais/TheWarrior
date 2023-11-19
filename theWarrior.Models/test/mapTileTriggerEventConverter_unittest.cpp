#include <gtest/gtest.h>
#include "mapTileTrigger.hpp"
#include "mapTileTriggerEventConverter.hpp"

using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerEventConverter;
using thewarrior::models::MapTileTriggerEvent;
using thewarrior::models::MapTileTriggerCondition;
using thewarrior::models::MapTileTriggerAction;

TEST(MapTileTriggerEventConverter_allEventsToString, Return7Events) {
    ASSERT_EQ(7, MapTileTriggerEventConverter::allEventsToString().size());
}

TEST(MapTileTriggerEventConverter_eventToString, withNone_ReturnNoneStr) {
    MapTileTrigger trigger;
    ASSERT_EQ("None", MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withSteppedOn_ReturnSteppedOnStr) {
    MapTileTrigger trigger(MapTileTriggerEvent::SteppedOn,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           std::map<std::string, std::string>());
    ASSERT_EQ("SteppedOn", MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withMoveUpPressed_ReturnMoveUpPressedStr) {
    MapTileTrigger trigger(MapTileTriggerEvent::MoveUpPressed,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           std::map<std::string, std::string>());
    ASSERT_EQ("MoveUpPressed", MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withMoveDownPressed_ReturnMoveDownPressedStr) {
    MapTileTrigger trigger(MapTileTriggerEvent::MoveDownPressed,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           std::map<std::string, std::string>());
    ASSERT_EQ("MoveDownPressed", MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withMoveLeftPressed_ReturnMoveLeftPressedStr) {
    MapTileTrigger trigger(MapTileTriggerEvent::MoveLeftPressed,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           std::map<std::string, std::string>());
    ASSERT_EQ("MoveLeftPressed", MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withMoveRightPressed_ReturnMoveRightPressedStr) {
    MapTileTrigger trigger(MapTileTriggerEvent::MoveRightPressed,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           std::map<std::string, std::string>());
    ASSERT_EQ("MoveRightPressed", MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withActionButtonPressed_ReturnActionButtonPressedStr) {
    MapTileTrigger trigger(MapTileTriggerEvent::ActionButtonPressed,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           std::map<std::string, std::string>());
    ASSERT_EQ("ActionButtonPressed", MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventFromString, withNone_ReturnNoneEvent) {
    ASSERT_EQ(MapTileTriggerEvent::None, MapTileTriggerEventConverter::eventFromString("None"));
}

TEST(MapTileTriggerEventConverter_eventFromString, withNoneLowerCase_ReturnNoneEvent) {
    ASSERT_EQ(MapTileTriggerEvent::None, MapTileTriggerEventConverter::eventFromString("none"));
}

TEST(MapTileTriggerEventConverter_eventFromString, withSteppedOn_ReturnSteppedOnEvent) {
    ASSERT_EQ(MapTileTriggerEvent::SteppedOn, MapTileTriggerEventConverter::eventFromString("SteppedOn"));
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveUpPressed_ReturnMoveUpPressedEvent) {
    ASSERT_EQ(MapTileTriggerEvent::MoveUpPressed, MapTileTriggerEventConverter::eventFromString("MoveUpPressed"));
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveDownPressed_ReturnMoveDownPressedEvent) {
    ASSERT_EQ(MapTileTriggerEvent::MoveDownPressed, MapTileTriggerEventConverter::eventFromString("MoveDownPressed"));
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveLeftPressed_ReturnMoveLeftPressedEvent) {
    ASSERT_EQ(MapTileTriggerEvent::MoveLeftPressed, MapTileTriggerEventConverter::eventFromString("MoveLeftPressed"));
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveRightPressed_ReturnMoveRightPressedEvent) {
    ASSERT_EQ(MapTileTriggerEvent::MoveRightPressed, MapTileTriggerEventConverter::eventFromString("MoveRightPressed"));
}

TEST(MapTileTriggerEventConverter_eventFromString, withActionButtonPressed_ReturnActionButtonPressedEvent) {
    ASSERT_EQ(MapTileTriggerEvent::ActionButtonPressed, MapTileTriggerEventConverter::eventFromString("ActionButtonPressed"));
}

TEST(MapTileTriggerEventConverter_eventFromString, withNonExistantEvent_ReturnEmpty) {
    ASSERT_FALSE(MapTileTriggerEventConverter::eventFromString("blabla").has_value());
}
