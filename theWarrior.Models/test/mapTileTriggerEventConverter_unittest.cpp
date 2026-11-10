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
    const auto actual = MapTileTriggerEventConverter::eventFromString("None");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerEvent::None, *actual);
}

TEST(MapTileTriggerEventConverter_eventFromString, withNoneLowerCase_ReturnNoneEvent) {
    const auto actual = MapTileTriggerEventConverter::eventFromString("none");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerEvent::None, *actual);
}

TEST(MapTileTriggerEventConverter_eventFromString, withSteppedOn_ReturnSteppedOnEvent) {
    const auto actual = MapTileTriggerEventConverter::eventFromString("SteppedOn");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerEvent::SteppedOn, *actual);
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveUpPressed_ReturnMoveUpPressedEvent) {
    const auto actual = MapTileTriggerEventConverter::eventFromString("MoveUpPressed");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerEvent::MoveUpPressed, *actual);
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveDownPressed_ReturnMoveDownPressedEvent) {
    const auto actual = MapTileTriggerEventConverter::eventFromString("MoveDownPressed");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerEvent::MoveDownPressed, *actual);
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveLeftPressed_ReturnMoveLeftPressedEvent) {
    const auto actual = MapTileTriggerEventConverter::eventFromString("MoveLeftPressed");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerEvent::MoveLeftPressed, *actual);
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveRightPressed_ReturnMoveRightPressedEvent) {
    const auto actual = MapTileTriggerEventConverter::eventFromString("MoveRightPressed");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerEvent::MoveRightPressed, *actual);
}

TEST(MapTileTriggerEventConverter_eventFromString, withActionButtonPressed_ReturnActionButtonPressedEvent) {
    const auto actual = MapTileTriggerEventConverter::eventFromString("ActionButtonPressed");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerEvent::ActionButtonPressed, *actual);
}

TEST(MapTileTriggerEventConverter_eventFromString, withNonExistantEvent_ReturnEmpty) {
    ASSERT_FALSE(MapTileTriggerEventConverter::eventFromString("blabla").has_value());
}
