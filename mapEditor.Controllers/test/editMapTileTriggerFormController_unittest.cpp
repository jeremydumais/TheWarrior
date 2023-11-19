#include <gtest/gtest.h>
#include "editMapTileTriggerFormController.hpp"
#include "mapTileTriggerDTO.hpp"

using mapeditor::controllers::EditMapTileTriggerFormController;
using mapeditor::controllers::MapTileTriggerDTO;

class EditMapTileTriggerFormControllerAddSample : public ::testing::Test {
 public:
     EditMapTileTriggerFormControllerAddSample()
         : controller({}, { MapTileTriggerDTO {
                 "SteppedOn", "None", "None", {}
                 }}) {
     }
     EditMapTileTriggerFormController controller;
};

class EditMapTileTriggerFormControllerUpdateSample : public ::testing::Test {
 public:
     EditMapTileTriggerFormControllerUpdateSample()
         : controller(MapTileTriggerDTO {
                 "SteppedOn", "None", "ChangeMap", {}
                 },
                 { MapTileTriggerDTO {
                 "SteppedOn", "None", "ChangeMap", {}
                 },
                 {
                  MapTileTriggerDTO {
                  "MoveUpPressed", "MustHaveItem", "OpenChest", {}
                  }
                 }}) {
     }
     EditMapTileTriggerFormController controller;
};

class EditMapTileTriggerFormControllerUpdateInvalidSample : public ::testing::Test {
 public:
     EditMapTileTriggerFormControllerUpdateInvalidSample()
         : controller(MapTileTriggerDTO {
                 "Step", "No", "Non", {}
                 },
                 { MapTileTriggerDTO {
                 "SteppedOn", "None", "None", {}
                 },
                 {
                  MapTileTriggerDTO {
                  "MoveUpPressed", "MustHaveItem", "OpenChest", {}
                  }
                 }}) {
     }
     EditMapTileTriggerFormController controller;
};

TEST_F(EditMapTileTriggerFormControllerAddSample, getCurrentTrigger_ReturnEmpty) {
    ASSERT_FALSE(controller.getCurrentTrigger().has_value());
}

TEST_F(EditMapTileTriggerFormControllerUpdateSample, getCurrentTrigger_ReturnOneTrigger) {
    auto trigger = controller.getCurrentTrigger();
    ASSERT_TRUE(trigger.has_value());
    ASSERT_EQ("SteppedOn", trigger->event);
}

TEST_F(EditMapTileTriggerFormControllerAddSample, getTriggerEventIndex_ReturnMinus1) {
    ASSERT_EQ(-1, controller.getTriggerEventIndex());
}

TEST_F(EditMapTileTriggerFormControllerUpdateSample, getTriggerEventIndex_Return1) {
    ASSERT_EQ(1, controller.getTriggerEventIndex());
}

TEST_F(EditMapTileTriggerFormControllerUpdateInvalidSample, getTriggerEventIndex_ReturnMinus1) {
    ASSERT_EQ(-1, controller.getTriggerEventIndex());
}

TEST_F(EditMapTileTriggerFormControllerAddSample, getTriggerConditionIndex_ReturnMinus1) {
    ASSERT_EQ(-1, controller.getTriggerConditionIndex());
}

TEST_F(EditMapTileTriggerFormControllerUpdateSample, getTriggerConditionIndex_Return0) {
    ASSERT_EQ(0, controller.getTriggerConditionIndex());
}

TEST_F(EditMapTileTriggerFormControllerUpdateInvalidSample, getTriggerConditionIndex_ReturnMinus1) {
    ASSERT_EQ(-1, controller.getTriggerConditionIndex());
}

TEST_F(EditMapTileTriggerFormControllerAddSample, getTriggerActionIndex_ReturnMinus1) {
    ASSERT_EQ(-1, controller.getTriggerActionIndex());
}

TEST_F(EditMapTileTriggerFormControllerUpdateSample, getTriggerActionIndex_Return2) {
    ASSERT_EQ(2, controller.getTriggerActionIndex());
}

TEST_F(EditMapTileTriggerFormControllerUpdateInvalidSample, getTriggerActionIndex_ReturnMinus1) {
    ASSERT_EQ(-1, controller.getTriggerActionIndex());
}

TEST_F(EditMapTileTriggerFormControllerAddSample, isUpdatedTriggerValid_WithNoneEventStr_ReturnFalse) {
    controller.setEvent("None");
    ASSERT_FALSE(controller.isUpdatedTriggerValid());
    ASSERT_EQ("You must select an event!", controller.getLastError());
}

TEST_F(EditMapTileTriggerFormControllerAddSample, isUpdatedTriggerValid_WithNoneActionStr_ReturnFalse) {
    controller.setEvent("SteppedOn");
    controller.setAction("None");
    ASSERT_FALSE(controller.isUpdatedTriggerValid());
    ASSERT_EQ("You must select an action!", controller.getLastError());
}

TEST_F(EditMapTileTriggerFormControllerAddSample, isUpdatedTriggerValid_WithInvalidEventStr_ReturnFalse) {
    controller.setEvent("hh");
    ASSERT_FALSE(controller.isUpdatedTriggerValid());
    ASSERT_EQ("Invalid values for MapTileTriggerDTO. Error : Unknown trigger event value : hh", controller.getLastError());
}

TEST_F(EditMapTileTriggerFormControllerAddSample, isUpdatedTriggerValid_WithInvalidActionStr_ReturnFalse) {
    controller.setAction("hh");
    ASSERT_FALSE(controller.isUpdatedTriggerValid());
    ASSERT_EQ("Invalid values for MapTileTriggerDTO. Error : Unknown trigger action value : hh", controller.getLastError());
}

