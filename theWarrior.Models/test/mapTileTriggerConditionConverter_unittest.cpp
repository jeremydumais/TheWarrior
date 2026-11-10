#include "mapTileTriggerConditionConverter.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace thewarrior::models;

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
    const auto actual = MapTileTriggerConditionConverter::conditionFromString("None"s);
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerCondition::None, *actual);
}

TEST(MapTileTriggerConditionConverter_conditionFromString, withNoneLowerCase_ReturnNoneCondition)
{
    const auto actual = MapTileTriggerConditionConverter::conditionFromString("none"s);
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerCondition::None, *actual);
}

TEST(MapTileTriggerConditionConverter_conditionFromString, withMustBeFacing_ReturnMustBeFacingCondition)
{
    const auto actual = MapTileTriggerConditionConverter::conditionFromString("MustBeFacing"s);
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerCondition::MustBeFacing, *actual);
}

TEST(MapTileTriggerConditionConverter_conditionFromString, withMustHaveItem_ReturnMustHaveItemCondition)
{
    const auto actual = MapTileTriggerConditionConverter::conditionFromString("MustHaveItem"s);
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(MapTileTriggerCondition::MustHaveItem, *actual);
}

TEST(MapTileTriggerConditionConverter_conditionFromString, withNonExistantCondition_ReturnEmpty)
{
    ASSERT_FALSE(MapTileTriggerConditionConverter::conditionFromString("blabla"s).has_value());
}
