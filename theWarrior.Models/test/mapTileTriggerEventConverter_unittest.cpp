#include "mapTileTriggerEventConverter.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace thewarrior::models;

TEST(MapTileTriggerEventConverter_allEventsToString, Return7Events)
{
    ASSERT_EQ(7, MapTileTriggerEventConverter::allEventsToString().size());
}

TEST(MapTileTriggerEventConverter_eventToString, withNone_ReturnNoneStr)
{
    MapTileTrigger trigger;
    ASSERT_EQ("None"s, MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withSteppedOn_ReturnSteppedOnStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::SteppedOn,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           map<string, string>());
    ASSERT_EQ("SteppedOn"s, MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withMoveUpPressed_ReturnMoveUpPressedStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::MoveUpPressed,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           map<string, string>());
    ASSERT_EQ("MoveUpPressed"s, MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withMoveDownPressed_ReturnMoveDownPressedStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::MoveDownPressed,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           map<string, string>());
    ASSERT_EQ("MoveDownPressed"s, MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withMoveLeftPressed_ReturnMoveLeftPressedStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::MoveLeftPressed,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           map<string, string>());
    ASSERT_EQ("MoveLeftPressed"s, MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withMoveRightPressed_ReturnMoveRightPressedStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::MoveRightPressed,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           map<string, string>());
    ASSERT_EQ("MoveRightPressed"s, MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventToString, withActionButtonPressed_ReturnActionButtonPressedStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::ActionButtonPressed,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           map<string, string>());
    ASSERT_EQ("ActionButtonPressed"s, MapTileTriggerEventConverter::eventToString(trigger.getEvent()));
}

TEST(MapTileTriggerEventConverter_eventFromString, withNone_ReturnNoneEvent)
{
    ASSERT_EQ(MapTileTriggerEvent::None, MapTileTriggerEventConverter::eventFromString("None"s));
}

TEST(MapTileTriggerEventConverter_eventFromString, withNoneLowerCase_ReturnNoneEvent)
{
    ASSERT_EQ(MapTileTriggerEvent::None, MapTileTriggerEventConverter::eventFromString("none"s));
}

TEST(MapTileTriggerEventConverter_eventFromString, withSteppedOn_ReturnSteppedOnEvent)
{
    ASSERT_EQ(MapTileTriggerEvent::SteppedOn, MapTileTriggerEventConverter::eventFromString("SteppedOn"s));
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveUpPressed_ReturnMoveUpPressedEvent)
{
    ASSERT_EQ(MapTileTriggerEvent::MoveUpPressed, MapTileTriggerEventConverter::eventFromString("MoveUpPressed"s));
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveDownPressed_ReturnMoveDownPressedEvent)
{
    ASSERT_EQ(MapTileTriggerEvent::MoveDownPressed, MapTileTriggerEventConverter::eventFromString("MoveDownPressed"s));
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveLeftPressed_ReturnMoveLeftPressedEvent)
{
    ASSERT_EQ(MapTileTriggerEvent::MoveLeftPressed, MapTileTriggerEventConverter::eventFromString("MoveLeftPressed"s));
}

TEST(MapTileTriggerEventConverter_eventFromString, withMoveRightPressed_ReturnMoveRightPressedEvent)
{
    ASSERT_EQ(MapTileTriggerEvent::MoveRightPressed, MapTileTriggerEventConverter::eventFromString("MoveRightPressed"s));
}

TEST(MapTileTriggerEventConverter_eventFromString, withActionButtonPressed_ReturnActionButtonPressedEvent)
{
    ASSERT_EQ(MapTileTriggerEvent::ActionButtonPressed, MapTileTriggerEventConverter::eventFromString("ActionButtonPressed"s));
}

TEST(MapTileTriggerEventConverter_eventFromString, withNonExistantEvent_ReturnEmpty)
{
    ASSERT_FALSE(MapTileTriggerEventConverter::eventFromString("blabla"s).has_value());
}
