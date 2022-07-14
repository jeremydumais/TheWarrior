#include "gameMapModeController.hpp"
#include <gtest/gtest.h>

class GameMapModeControllerFreshNew : public ::testing::Test
{
public:
    GameMapModeController controller;
};

class GameMapModeController3TilesIn2MapsProcessed : public ::testing::Test
{
public:
    GameMapModeController3TilesIn2MapsProcessed()
    {
        controller.addTileActionProcessed("Test", 2);
        controller.addTileActionProcessed("Test", 3);
        controller.addTileActionProcessed("AnotherMap", 1);
    }
    GameMapModeController controller;
};


TEST_F(GameMapModeControllerFreshNew, isTileActionAlreadyProcessed_WithMinus1_ReturnFalse)
{
    ASSERT_FALSE(controller.isTileActionAlreadyProcessed("Test", -1));
}

TEST_F(GameMapModeControllerFreshNew, isTileActionAlreadyProcessed_With0_ReturnFalse)
{
    ASSERT_FALSE(controller.isTileActionAlreadyProcessed("Test", 0));
}

TEST_F(GameMapModeControllerFreshNew, isTileActionAlreadyProcessed_With10_ReturnFalse)
{
    ASSERT_FALSE(controller.isTileActionAlreadyProcessed("Test", 10));
}

TEST_F(GameMapModeControllerFreshNew, addTileActionProcessed_WithTestMapAnd2_ReturnSuccess)
{
    controller.addTileActionProcessed("Test", 2);
    ASSERT_TRUE(controller.isTileActionAlreadyProcessed("Test", 2));
}

TEST_F(GameMapModeController3TilesIn2MapsProcessed, isTileActionAlreadyProcessed_WithTestAnd10_ReturnFalse)
{
    ASSERT_FALSE(controller.isTileActionAlreadyProcessed("Test", 10));
}

TEST_F(GameMapModeController3TilesIn2MapsProcessed, isTileActionAlreadyProcessed_WithTestAnd2_ReturnTrue)
{
    ASSERT_TRUE(controller.isTileActionAlreadyProcessed("Test", 2));
}

TEST_F(GameMapModeController3TilesIn2MapsProcessed, isTileActionAlreadyProcessed_WithTestAnd3_ReturnTrue)
{
    ASSERT_TRUE(controller.isTileActionAlreadyProcessed("Test", 2));
}

TEST_F(GameMapModeController3TilesIn2MapsProcessed, isTileActionAlreadyProcessed_WithTestAnd4_ReturnFalse)
{
    ASSERT_FALSE(controller.isTileActionAlreadyProcessed("Test", 4));
}