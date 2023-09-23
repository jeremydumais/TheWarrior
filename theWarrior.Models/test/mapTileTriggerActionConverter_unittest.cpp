#include "mapTileTriggerActionConverter.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace thewarrior::models;

TEST(MapTileTriggerActionConverter_allActionsToString, Return3Actions)
{
    ASSERT_EQ(4, MapTileTriggerActionConverter::allActionsToString().size());
}

TEST(MapTileTriggerActionConverter_actionToString, withNone_ReturnNoneStr)
{
    MapTileTrigger trigger;
    ASSERT_EQ("None"s, MapTileTriggerActionConverter::actionToString(trigger.getAction()));
}

TEST(MapTileTriggerActionConverter_actionToString, withOpenChest_ReturnOpenChestStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::None,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::OpenChest,
                           map<string, string>());
    ASSERT_EQ("OpenChest"s, MapTileTriggerActionConverter::actionToString(trigger.getAction()));
}

TEST(MapTileTriggerActionConverter_actionToString, withChangeMap_ReturnChangeMapStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::None,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::ChangeMap,
                           map<string, string>());
    ASSERT_EQ("ChangeMap"s, MapTileTriggerActionConverter::actionToString(trigger.getAction()));
}

TEST(MapTileTriggerActionConverter_actionToString, withDenyMove_ReturnDenyMoveStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::None,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::DenyMove,
                           map<string, string>());
    ASSERT_EQ("DenyMove"s, MapTileTriggerActionConverter::actionToString(trigger.getAction()));
}

TEST(MapTileTriggerActionConverter_actionFromString, withNone_ReturnNoneAction)
{
    ASSERT_EQ(MapTileTriggerAction::None, MapTileTriggerActionConverter::actionFromString("None"s));
}

TEST(MapTileTriggerActionConverter_actionFromString, withNoneLowerCase_ReturnNoneAction)
{
    ASSERT_EQ(MapTileTriggerAction::None, MapTileTriggerActionConverter::actionFromString("none"s));
}

TEST(MapTileTriggerActionConverter_actionFromString, withOpenChest_ReturnOpenChestAction)
{
    ASSERT_EQ(MapTileTriggerAction::OpenChest, MapTileTriggerActionConverter::actionFromString("OpenChest"s));
}

TEST(MapTileTriggerActionConverter_actionFromString, withChangeMap_ReturnChangeMapAction)
{
    ASSERT_EQ(MapTileTriggerAction::ChangeMap, MapTileTriggerActionConverter::actionFromString("ChangeMap"s));
}

TEST(MapTileTriggerActionConverter_actionFromString, withDenyMove_ReturnDenyMoveAction)
{
    ASSERT_EQ(MapTileTriggerAction::DenyMove, MapTileTriggerActionConverter::actionFromString("DenyMove"s));
}

TEST(MapTileTriggerActionConverter_actionFromString, withNonExistantAction_ReturnEmpty)
{
    ASSERT_FALSE(MapTileTriggerActionConverter::actionFromString("blabla"s).has_value());
}
