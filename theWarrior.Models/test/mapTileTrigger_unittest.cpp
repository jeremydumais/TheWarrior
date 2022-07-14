#include "mapTileTrigger.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(MapTileTrigger_Constructor, withoutArguments_ReturnSuccess)
{
    MapTileTrigger trigger;
    ASSERT_EQ(MapTileTriggerEvent::None, trigger.getEvent());
    ASSERT_EQ(MapTileTriggerCondition::None, trigger.getCondition());
    ASSERT_EQ(MapTileTriggerAction::None, trigger.getAction());
    ASSERT_EQ(0, trigger.getActionProperties().size());
}

TEST(MapTileTrigger_Constructor, withNoProperties_ReturnSuccess)
{
    MapTileTrigger trigger(MapTileTriggerEvent::None,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           map<string, string>());
    ASSERT_EQ(MapTileTriggerEvent::None, trigger.getEvent());
    ASSERT_EQ(MapTileTriggerCondition::None, trigger.getCondition());
    ASSERT_EQ(MapTileTriggerAction::None, trigger.getAction());
    ASSERT_EQ(0, trigger.getActionProperties().size());
}