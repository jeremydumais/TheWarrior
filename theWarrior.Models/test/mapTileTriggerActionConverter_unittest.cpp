#include "mapTileTriggerActionConverter.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace thewarrior::models;

TEST(MapTileTriggerActionConverter_allActionsToString, Return5Actions)
{
    ASSERT_EQ(5, MapTileTriggerActionConverter::allActionsToString().size());
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

TEST(MapTileTriggerActionConverter_actionToString,
     withConversationScenario_ReturnConversationScenarioStr)
{
    ASSERT_EQ(
        "ConversationScenario"s,
        MapTileTriggerActionConverter::actionToString(
            MapTileTriggerAction::ConversationScenario));
}

TEST(MapTileTriggerActionConverter_actionFromString, withNone_ReturnNoneAction)
{
    const auto actual = MapTileTriggerActionConverter::actionFromString("None"s);
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerAction::None, *actual);
}

TEST(MapTileTriggerActionConverter_actionFromString, withNoneLowerCase_ReturnNoneAction)
{
    const auto actual = MapTileTriggerActionConverter::actionFromString("none"s);
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerAction::None, *actual);
}

TEST(MapTileTriggerActionConverter_actionFromString, withOpenChest_ReturnOpenChestAction)
{
    const auto actual = MapTileTriggerActionConverter::actionFromString("OpenChest"s);
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerAction::OpenChest, *actual);
}

TEST(MapTileTriggerActionConverter_actionFromString, withChangeMap_ReturnChangeMapAction)
{
    const auto actual = MapTileTriggerActionConverter::actionFromString("ChangeMap"s);
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerAction::ChangeMap, *actual);
}

TEST(MapTileTriggerActionConverter_actionFromString, withDenyMove_ReturnDenyMoveAction)
{
    const auto actual = MapTileTriggerActionConverter::actionFromString("DenyMove"s);
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerAction::DenyMove, *actual);
}

TEST(MapTileTriggerActionConverter_actionFromString,
     withConversationScenario_ReturnConversationScenarioAction)
{
    const auto actual = MapTileTriggerActionConverter::actionFromString("ConversationScenario"s);
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerAction::ConversationScenario, *actual);
}

TEST(MapTileTriggerActionConverter_actionFromString, withNonExistantAction_ReturnEmpty)
{
    ASSERT_FALSE(MapTileTriggerActionConverter::actionFromString("blabla"s).has_value());
}
