#include "worldState.hpp"
#include <gtest/gtest.h>

using namespace thewarrior::models;

class WorldStateFreshNew : public ::testing::Test {
 public:
    WorldState controller;
};

class WorldState3TilesIn2MapsProcessed : public ::testing::Test {
 public:
    WorldState3TilesIn2MapsProcessed() {
        controller.addTileActionProcessed("Test", 2);
        controller.addTileActionProcessed("Test", 3);
        controller.addTileActionProcessed("AnotherMap", 1);
    }
    WorldState controller;
};


TEST_F(WorldStateFreshNew, isTileActionAlreadyProcessed_WithMinus1_ReturnFalse) {
    ASSERT_FALSE(controller.isTileActionAlreadyProcessed("Test", -1));
}

TEST_F(WorldStateFreshNew, isTileActionAlreadyProcessed_With0_ReturnFalse) {
    ASSERT_FALSE(controller.isTileActionAlreadyProcessed("Test", 0));
}

TEST_F(WorldStateFreshNew, isTileActionAlreadyProcessed_With10_ReturnFalse) {
    ASSERT_FALSE(controller.isTileActionAlreadyProcessed("Test", 10));
}

TEST_F(WorldStateFreshNew, addTileActionProcessed_WithTestMapAnd2_ReturnSuccess) {
    controller.addTileActionProcessed("Test", 2);
    ASSERT_TRUE(controller.isTileActionAlreadyProcessed("Test", 2));
}

TEST_F(WorldState3TilesIn2MapsProcessed, isTileActionAlreadyProcessed_WithTestAnd10_ReturnFalse) {
    ASSERT_FALSE(controller.isTileActionAlreadyProcessed("Test", 10));
}

TEST_F(WorldState3TilesIn2MapsProcessed, isTileActionAlreadyProcessed_WithTestAnd2_ReturnTrue) {
    ASSERT_TRUE(controller.isTileActionAlreadyProcessed("Test", 2));
}

TEST_F(WorldState3TilesIn2MapsProcessed, isTileActionAlreadyProcessed_WithTestAnd3_ReturnTrue) {
    ASSERT_TRUE(controller.isTileActionAlreadyProcessed("Test", 2));
}

TEST_F(WorldState3TilesIn2MapsProcessed, isTileActionAlreadyProcessed_WithTestAnd4_ReturnFalse) {
    ASSERT_FALSE(controller.isTileActionAlreadyProcessed("Test", 4));
}

