#include <algorithm>
#include <gtest/gtest.h>
#include <memory>
#include "gameMap.hpp"
#include "glComponentController.hpp"
#include "mapTileTrigger.hpp"
#include "tilePropsComponentController.hpp"

using mapeditor::controllers::GLComponentController;
using mapeditor::controllers::TilePropsComponentController;
using thewarrior::models::GameMap;
using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerEvent;
using thewarrior::models::MapTileTriggerCondition;
using thewarrior::models::MapTileTriggerAction;

class TilesPropsControllerSample1 : public ::testing::Test {
 public:
    TilesPropsControllerSample1()
        : map(std::make_shared<GameMap>(3, 3)),
          glController(),
          tilesController(&glController) {
        glController.setCurrentMap(map);
        auto tilesToAdjust = map->getTilesForEditing({1, 3});
        tilesToAdjust[0]->addTrigger(MapTileTrigger(MapTileTriggerEvent::SteppedOn,
            MapTileTriggerCondition::MustBeFacing,
            MapTileTriggerAction::DenyMove,
            { { "Test1", "Test2" }, { "Test3", "Test4" } }));
        tilesToAdjust[1]->addTrigger(MapTileTrigger(MapTileTriggerEvent::SteppedOn,
            MapTileTriggerCondition::MustBeFacing,
            MapTileTriggerAction::DenyMove,
            { { "Test1", "Test2" }, { "Test3", "Test4" } }));
        tilesToAdjust[1]->addTrigger(MapTileTrigger(MapTileTriggerEvent::MoveRightPressed,
            MapTileTriggerCondition::MustHaveItem,
            MapTileTriggerAction::ChangeMap,
            { { "Test1", "Test2" }, { "Test3", "Test4" } }));
    }
    std::shared_ptr<GameMap> map;
    GLComponentController glController;
    TilePropsComponentController tilesController;
};

TEST_F(TilesPropsControllerSample1, GetSelectedTilesCommonTriggers_WithTwoTilesOneCommonTrig_ReturnOneTrig) {
    glController.selectTilesForEditing({1, 3});
    auto triggers = tilesController.getSelectedTilesCommonTriggers();
    ASSERT_EQ(1, triggers.size());
    ASSERT_EQ("SteppedOn", triggers.cbegin()->event);
}

TEST_F(TilesPropsControllerSample1, GetSelectedTilesCommonTriggers_WithTwoTilesOneCommonTrigDiffCond_ReturnEmpty) {
    auto tilesToAdjust = map->getTilesForEditing({3});
    tilesToAdjust[0]->findTrigger(MapTileTriggerEvent::SteppedOn)->setCondition(MapTileTriggerCondition::MustHaveItem);
    glController.selectTilesForEditing({1, 3});
    auto triggers = tilesController.getSelectedTilesCommonTriggers();
    ASSERT_EQ(0, triggers.size());
}

TEST_F(TilesPropsControllerSample1, GetSelectedTilesCommonTriggers_WithTwoTilesOneCommonTrigDiffAction_ReturnEmpty) {
    auto tilesToAdjust = map->getTilesForEditing({3});
    tilesToAdjust[0]->findTrigger(MapTileTriggerEvent::SteppedOn)->setAction(MapTileTriggerAction::OpenChest);
    glController.selectTilesForEditing({1, 3});
    auto triggers = tilesController.getSelectedTilesCommonTriggers();
    ASSERT_EQ(0, triggers.size());
}

TEST_F(TilesPropsControllerSample1, GetSelectedTilesCommonTriggers_WithTwoTilesOneCommonTrigDiffActionProps_ReturnEmpty) {
    auto tilesToAdjust = map->getTilesForEditing({3});
    tilesToAdjust[0]->findTrigger(MapTileTriggerEvent::SteppedOn)->setActionProperties({{"Test1", "Test2"}, {"Test3", "Test5"}});
    glController.selectTilesForEditing({1, 3});
    auto triggers = tilesController.getSelectedTilesCommonTriggers();
    ASSERT_EQ(0, triggers.size());
}

TEST_F(TilesPropsControllerSample1, SetSelectedTilesTextureName_WithNoSelectedTile_ReturnSuccess) {
    glController.selectTilesForEditing({});
    ASSERT_EQ(0, glController.getCurrentMapTiles().size());
    ASSERT_NO_THROW(tilesController.setSelectedTilesTextureName(""));
}

TEST_F(TilesPropsControllerSample1, SetSelectedTilesTextureName_WithOneSelectedTile_ReturnSuccess) {
    glController.selectTilesForEditing({1});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(1, tiles.size());
    tilesController.setSelectedTilesTextureName("Test");
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ("Test", tile->getTextureName());
    });
}

TEST_F(TilesPropsControllerSample1, SetSelectedTilesTextureName_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setSelectedTilesTextureName("Test");
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ("Test", tile->getTextureName());
    });
}

TEST_F(TilesPropsControllerSample1, SetSelectedTilesTextureIndex_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setSelectedTilesTextureIndex(3);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(3, tile->getTextureIndex());
    });
}

TEST_F(TilesPropsControllerSample1, SetSelectedTilesObjectTextureName_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setSelectedTilesObjectTextureName("Test");
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ("Test", tile->getObjectTextureName());
    });
}

TEST_F(TilesPropsControllerSample1, SetSelectedTilesObjectTextureIndex_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setSelectedTilesObjectTextureIndex(3);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(3, tile->getObjectTextureIndex());
    });
}

TEST_F(TilesPropsControllerSample1, SetSelectedTilesObjectAbovePlayer_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setSelectedTilesObjectAbovePlayer(true);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(true, tile->getObjectAbovePlayer());
    });
}

TEST_F(TilesPropsControllerSample1, SetSelectedTilesCanSteppedOn_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setSelectedTilesCanSteppedOn(false);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(false, tile->canPlayerSteppedOn());
    });
}

TEST_F(TilesPropsControllerSample1, SetSelectedTilesIsWallToClimb_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setSelectedTilesIsWallToClimb(true);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(true, tile->getIsWallToClimb());
    });
}

TEST_F(TilesPropsControllerSample1, SetSelectedTilesMonsterZoneIndex_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setSelectedTilesMonsterZoneIndex(4);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(4, tile->getMonsterZoneIndex());
    });
}

