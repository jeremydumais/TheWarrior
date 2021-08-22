#include "mapTileTriggerConditionConverter.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(MapTileTriggerConditionConverter_allConditionsToString, Return3Conditions)
{
    ASSERT_EQ(3, MapTileTriggerConditionConverter::allConditionsToString().size());
}

TEST(MapTileTriggerConditionConverter_conditionToString, withNone_ReturnNoneStr)
{
    MapTileTrigger trigger;
    ASSERT_EQ("None"s, MapTileTriggerConditionConverter::conditionToString(trigger.getCondition()));
}

TEST(MapTileTriggerConditionConverter_conditionToString, withMustBeFacing_ReturnMustBeFacingStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::None,
                           MapTileTriggerCondition::MustBeFacing,
                           MapTileTriggerAction::None,
                           map<string, string>());
    ASSERT_EQ("MustBeFacing"s, MapTileTriggerConditionConverter::conditionToString(trigger.getCondition()));
}

TEST(MapTileTriggerConditionConverter_conditionToString, withMustHaveItem_ReturnMustHaveItemStr)
{
    MapTileTrigger trigger(MapTileTriggerEvent::None,
                           MapTileTriggerCondition::MustHaveItem,
                           MapTileTriggerAction::None,
                           map<string, string>());
    ASSERT_EQ("MustHaveItem"s, MapTileTriggerConditionConverter::conditionToString(trigger.getCondition()));
}

TEST(MapTileTriggerConditionConverter_conditionFromString, withNone_ReturnNoneCondition)
{
    ASSERT_EQ(MapTileTriggerCondition::None, MapTileTriggerConditionConverter::conditionFromString("None"s));
}

TEST(MapTileTriggerConditionConverter_conditionFromString, withNoneLowerCase_ReturnNoneCondition)
{
    ASSERT_EQ(MapTileTriggerCondition::None, MapTileTriggerConditionConverter::conditionFromString("none"s));
}

TEST(MapTileTriggerConditionConverter_conditionFromString, withMustBeFacing_ReturnMustBeFacingCondition)
{
    ASSERT_EQ(MapTileTriggerCondition::MustBeFacing, MapTileTriggerConditionConverter::conditionFromString("MustBeFacing"s));
}

TEST(MapTileTriggerConditionConverter_conditionFromString, withMustHaveItem_ReturnMustHaveItemCondition)
{
    ASSERT_EQ(MapTileTriggerCondition::MustHaveItem, MapTileTriggerConditionConverter::conditionFromString("MustHaveItem"s));
}

TEST(MapTileTriggerConditionConverter_conditionFromString, withNonExistantCondition_ReturnEmpty)
{
    ASSERT_FALSE(MapTileTriggerConditionConverter::conditionFromString("blabla"s).has_value());
}