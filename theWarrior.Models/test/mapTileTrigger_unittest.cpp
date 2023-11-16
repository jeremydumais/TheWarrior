#include "mapTileTrigger.hpp"
#include <gtest/gtest.h>

using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerEvent;
using thewarrior::models::MapTileTriggerCondition;
using thewarrior::models::MapTileTriggerAction;

MapTileTrigger getTriggerSample1() {
    return MapTileTrigger (MapTileTriggerEvent::SteppedOn,
            MapTileTriggerCondition::MustBeFacing,
            MapTileTriggerAction::DenyMove,
            { { "Test1", "Test2" }, { "Test3", "Test4" } });
}

TEST(MapTileTrigger_DefaultConstructor, ReturnSuccess) {
    MapTileTrigger trigger;
    ASSERT_EQ(MapTileTriggerEvent::None, trigger.getEvent());
    ASSERT_EQ(MapTileTriggerCondition::None, trigger.getCondition());
    ASSERT_EQ(MapTileTriggerAction::None, trigger.getAction());
    ASSERT_EQ(0, trigger.getActionProperties().size());
}

TEST(MapTileTrigger_Constructor, WithNoneProperties_ReturnSuccess) {
    MapTileTrigger trigger(MapTileTriggerEvent::None,
                           MapTileTriggerCondition::None,
                           MapTileTriggerAction::None,
                           std::map<std::string, std::string>());
    ASSERT_EQ(MapTileTriggerEvent::None, trigger.getEvent());
    ASSERT_EQ(MapTileTriggerCondition::None, trigger.getCondition());
    ASSERT_EQ(MapTileTriggerAction::None, trigger.getAction());
    ASSERT_EQ(0, trigger.getActionProperties().size());
}

TEST(MapTileTrigger_OperatorEqual, WithDefaultTrigger_ReturnTrue) {
    ASSERT_EQ(MapTileTrigger(), MapTileTrigger());
}

TEST(MapTileTrigger_OperatorEqual, WithSameValueTrigger_ReturnTrue) {
    ASSERT_EQ(getTriggerSample1(), getTriggerSample1());
}

TEST(MapTileTrigger_OperatorEqual, WithEventDifferent_ReturnFalse) {
    MapTileTrigger val2(MapTileTriggerEvent::ActionButtonPressed,
            MapTileTriggerCondition::MustBeFacing,
            MapTileTriggerAction::DenyMove,
            { { "Test1", "Test2" }, { "Test3", "Test4" } });
    ASSERT_FALSE(getTriggerSample1() == val2);
}

TEST(MapTileTrigger_OperatorEqual, WithConditionDifferent_ReturnFalse) {
    MapTileTrigger val2(MapTileTriggerEvent::SteppedOn,
            MapTileTriggerCondition::MustHaveItem,
            MapTileTriggerAction::DenyMove,
            { { "Test1", "Test2" }, { "Test3", "Test4" } });
    ASSERT_FALSE(getTriggerSample1() == val2);
}

TEST(MapTileTrigger_OperatorEqual, WithActionDifferent_ReturnFalse) {
    MapTileTrigger val2(MapTileTriggerEvent::SteppedOn,
            MapTileTriggerCondition::MustBeFacing,
            MapTileTriggerAction::ChangeMap,
            { { "Test1", "Test2" }, { "Test3", "Test4" } });
    ASSERT_FALSE(getTriggerSample1() == val2);
}

TEST(MapTileTrigger_OperatorEqual, WithActionPropsDifferent_ReturnFalse) {
    MapTileTrigger val2(MapTileTriggerEvent::SteppedOn,
            MapTileTriggerCondition::MustBeFacing,
            MapTileTriggerAction::DenyMove,
            { { "Test5", "Test6" } });
    ASSERT_FALSE(getTriggerSample1() == val2);
}

TEST(MapTileTrigger_OperatorNotEqual, WithDefaultTrigger_ReturnFalse) {
        ASSERT_FALSE(MapTileTrigger() != MapTileTrigger());
}

TEST(MapTileTrigger_OperatorNotEqual, WithSameValueTrigger_ReturnFalse) {
        ASSERT_FALSE(getTriggerSample1() != getTriggerSample1());
}

TEST(MapTileTrigger_OperatorNotEqual, WithEventDifferent_ReturnTrue) {
    MapTileTrigger val2(MapTileTriggerEvent::ActionButtonPressed,
            MapTileTriggerCondition::MustBeFacing,
            MapTileTriggerAction::DenyMove,
            { { "Test1", "Test2" }, { "Test3", "Test4" } });
                ASSERT_NE(getTriggerSample1(), val2);
}

TEST(MapTileTrigger_OperatorNotEqual, WithConditionDifferent_ReturnTrue) {
    MapTileTrigger val2(MapTileTriggerEvent::SteppedOn,
            MapTileTriggerCondition::MustHaveItem,
            MapTileTriggerAction::DenyMove,
            { { "Test1", "Test2" }, { "Test3", "Test4" } });
                ASSERT_NE(getTriggerSample1(), val2);
}

TEST(MapTileTrigger_OperatorNotEqual, WithActionDifferent_ReturnTrue) {
    MapTileTrigger val2(MapTileTriggerEvent::SteppedOn,
            MapTileTriggerCondition::MustBeFacing,
            MapTileTriggerAction::ChangeMap,
            { { "Test1", "Test2" }, { "Test3", "Test4" } });
                ASSERT_NE(getTriggerSample1(), val2);
}

TEST(MapTileTrigger_OperatorNotEqual, WithActionPropsDifferent_ReturnTrue) {
    MapTileTrigger val2(MapTileTriggerEvent::SteppedOn,
            MapTileTriggerCondition::MustBeFacing,
            MapTileTriggerAction::DenyMove,
            { { "Test5", "Test6" } });
                ASSERT_NE(getTriggerSample1(), val2);
}

// TODO: 0.3.3 Unit test all the other methods
