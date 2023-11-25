#include <algorithm>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include "gameMap.hpp"
#include "glComponentController.hpp"
#include "mapTileTrigger.hpp"
#include "mapTileTriggerDTO.hpp"
#include "tilePropsComponentController.hpp"

using mapeditor::controllers::GLComponentController;
using mapeditor::controllers::TilePropsComponentController;
using mapeditor::controllers::MapTileTriggerDTO;
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

TEST_F(TilesPropsControllerSample1, GetTilesCommonTriggers_WithTwoTilesOneCommonTrig_ReturnOneTrig) {
    glController.selectTilesForEditing({1, 3});
    auto triggers = tilesController.getTilesCommonTriggers();
    ASSERT_EQ(1, triggers.size());
    ASSERT_EQ("SteppedOn", triggers.cbegin()->event);
}

TEST_F(TilesPropsControllerSample1, GetTilesCommonTriggers_WithTwoTilesOneCommonTrigDiffCond_ReturnEmpty) {
    auto tilesToAdjust = map->getTilesForEditing({3});
    tilesToAdjust[0]->findTrigger(MapTileTriggerEvent::SteppedOn)->setCondition(MapTileTriggerCondition::MustHaveItem);
    glController.selectTilesForEditing({1, 3});
    auto triggers = tilesController.getTilesCommonTriggers();
    ASSERT_EQ(0, triggers.size());
}

TEST_F(TilesPropsControllerSample1, GetTilesCommonTriggers_WithTwoTilesOneCommonTrigDiffAction_ReturnEmpty) {
    auto tilesToAdjust = map->getTilesForEditing({3});
    tilesToAdjust[0]->findTrigger(MapTileTriggerEvent::SteppedOn)->setAction(MapTileTriggerAction::OpenChest);
    glController.selectTilesForEditing({1, 3});
    auto triggers = tilesController.getTilesCommonTriggers();
    ASSERT_EQ(0, triggers.size());
}

TEST_F(TilesPropsControllerSample1, GetTilesCommonTriggers_WithTwoTilesOneCommonTrigDiffActionProps_ReturnEmpty) {
    auto tilesToAdjust = map->getTilesForEditing({3});
    tilesToAdjust[0]->findTrigger(MapTileTriggerEvent::SteppedOn)->setActionProperties({{"Test1", "Test2"}, {"Test3", "Test5"}});
    glController.selectTilesForEditing({1, 3});
    auto triggers = tilesController.getTilesCommonTriggers();
    ASSERT_EQ(0, triggers.size());
}

TEST_F(TilesPropsControllerSample1, findMapTileTriggerByEvent_WithUnknownEvent_ReturnEmpty) {
    glController.selectTilesForEditing({1, 3});
    ASSERT_FALSE(tilesController.findMapTileTriggerByEvent("M").has_value());
}

TEST_F(TilesPropsControllerSample1, findMapTileTriggerByEvent_WithNotUsedEvent_ReturnEmpty) {
    glController.selectTilesForEditing({1, 3});
    ASSERT_FALSE(tilesController.findMapTileTriggerByEvent("MoveDownPressed").has_value());
}

TEST_F(TilesPropsControllerSample1, findMapTileTriggerByEvent_WithCommonEvent_ReturnTriggerDTO) {
    glController.selectTilesForEditing({1, 3});
    auto expected = MapTileTrigger(MapTileTriggerEvent::SteppedOn,
            MapTileTriggerCondition::MustBeFacing,
            MapTileTriggerAction::DenyMove,
            { { "Test1", "Test2" }, { "Test3", "Test4" } });
    auto triggerDTO = tilesController.findMapTileTriggerByEvent("SteppedOn");
    ASSERT_TRUE(triggerDTO.has_value());
    ASSERT_EQ("SteppedOn", triggerDTO->event);
}

TEST_F(TilesPropsControllerSample1, SetTilesTextureName_WithNoSelectedTile_ReturnSuccess) {
    glController.selectTilesForEditing({});
    ASSERT_EQ(0, glController.getCurrentMapTiles().size());
    ASSERT_NO_THROW(tilesController.setTilesTextureName(""));
}

TEST_F(TilesPropsControllerSample1, SetTilesTextureName_WithOneSelectedTile_ReturnSuccess) {
    glController.selectTilesForEditing({1});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(1, tiles.size());
    tilesController.setTilesTextureName("Test");
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ("Test", tile->getTextureName());
    });
}

TEST_F(TilesPropsControllerSample1, SetTilesTextureName_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setTilesTextureName("Test");
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ("Test", tile->getTextureName());
    });
}

TEST_F(TilesPropsControllerSample1, SetTilesTextureIndex_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setTilesTextureIndex(3);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(3, tile->getTextureIndex());
    });
}

TEST_F(TilesPropsControllerSample1, SetTilesObjectTextureName_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setTilesObjectTextureName("Test");
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ("Test", tile->getObjectTextureName());
    });
}

TEST_F(TilesPropsControllerSample1, SetTilesObjectTextureIndex_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setTilesObjectTextureIndex(3);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(3, tile->getObjectTextureIndex());
    });
}

TEST_F(TilesPropsControllerSample1, SetTilesObjectAbovePlayer_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setTilesObjectAbovePlayer(true);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(true, tile->getObjectAbovePlayer());
    });
}

TEST_F(TilesPropsControllerSample1, SetTilesCanSteppedOn_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setTilesCanSteppedOn(false);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(false, tile->canPlayerSteppedOn());
    });
}

TEST_F(TilesPropsControllerSample1, SetTilesIsWallToClimb_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setTilesIsWallToClimb(true);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(true, tile->getIsWallToClimb());
    });
}

TEST_F(TilesPropsControllerSample1, SetTilesMonsterZoneIndex_WithTwoSelectedTiles_ReturnSuccess) {
    glController.selectTilesForEditing({1, 3});
    auto tiles = glController.getCurrentMapTiles();
    ASSERT_EQ(2, tiles.size());
    tilesController.setTilesMonsterZoneIndex(4);
    std::for_each(tiles.begin(), tiles.end(), [](const auto *tile) {
        ASSERT_EQ(4, tile->getMonsterZoneIndex());
    });
}

TEST_F(TilesPropsControllerSample1, AddTilesTrigger_WithTwoSelectedTiles_ReturnSuccess) {
    std::map<std::string, std::string> expectedActionProps { { "Test1", "Test2" }, { "Test3", "Test4" } };
    glController.selectTilesForEditing({2, 4});
    ASSERT_EQ(0, tilesController.getTilesCommonTriggers().size());
    tilesController.addTilesTrigger(MapTileTriggerDTO { "MoveRightPressed",
            "MustBeFacing",
            "DenyMove",
            expectedActionProps });
    auto commonTriggers = tilesController.getTilesCommonTriggers();
    ASSERT_EQ(1, commonTriggers.size());
    ASSERT_EQ("MoveRightPressed", commonTriggers.begin()->event);
    ASSERT_EQ("MustBeFacing", commonTriggers.begin()->condition);
    ASSERT_EQ("DenyMove", commonTriggers.begin()->action);
    ASSERT_EQ(expectedActionProps, commonTriggers.begin()->actionProperties);
}

TEST_F(TilesPropsControllerSample1, AddTilesTrigger_WithTwoSelectedTilesAndInvalidDTO_ReturnFalse) {
    std::map<std::string, std::string> expectedActionProps { { "Test1", "Test2" }, { "Test3", "Test4" } };
    glController.selectTilesForEditing({2, 4});
    ASSERT_FALSE(tilesController.addTilesTrigger(MapTileTriggerDTO { "M",
            "MustBeFacing",
            "DenyMove",
            expectedActionProps }));
}

TEST_F(TilesPropsControllerSample1, UpdateTilesTrigger_WithInvalidOldDTO_ReturnFalse) {
    glController.selectTilesForEditing({1, 3});
    ASSERT_FALSE(tilesController.updateTilesTrigger(MapTileTriggerDTO { "M",
            "MustBeFacing",
            "DenyMove",
            {} },
            MapTileTriggerDTO { "MoveRightPressed",
            "MustBeFacing",
            "DenyMove",
            {} }));
}

TEST_F(TilesPropsControllerSample1, UpdateTilesTrigger_WithInvalidNewDTO_ReturnFalse) {
    glController.selectTilesForEditing({1, 3});
    ASSERT_FALSE(tilesController.updateTilesTrigger(
            MapTileTriggerDTO { "SteppedOn",
            "MustBeFacing",
            "DenyMove",
            { { "Test1", "Test2" }, { "Test3", "Test4" } }},
            MapTileTriggerDTO { "M",
            "MustBeFacing",
            "DenyMove",
            {} }));
}

TEST_F(TilesPropsControllerSample1, UpdateTilesTrigger_WithTriggerUpdateAllTilesUpdated_ReturnTrue) {
    glController.selectTilesForEditing({1, 3});
    ASSERT_TRUE(tilesController.updateTilesTrigger(
            MapTileTriggerDTO { "SteppedOn",
            "MustBeFacing",
            "DenyMove",
            { { "Test1", "Test2" }, { "Test3", "Test4" } }},
            MapTileTriggerDTO { "MoveDownPressed",
            "MustBeFacing",
            "DenyMove",
            { { "Test1", "Test2" }, { "Test3", "Test4" } }}));
}

TEST_F(TilesPropsControllerSample1, UpdateTilesTrigger_WithTriggerUpdateInitialNotCommon_ReturnFalse) {
    glController.selectTilesForEditing({2, 4});
    ASSERT_FALSE(tilesController.updateTilesTrigger(
            MapTileTriggerDTO { "SteppedOn",
            "MustBeFacing",
            "DenyMove",
            { { "Test1", "Test2" }, { "Test3", "Test4" } }},
            MapTileTriggerDTO { "MoveDownPressed",
            "MustBeFacing",
            "DenyMove",
            { { "Test1", "Test2" }, { "Test3", "Test4" } }}));
}

TEST_F(TilesPropsControllerSample1, DeleteTilesTrigger_WithInvalidDTO_ReturnFalse) {
    glController.selectTilesForEditing({1, 3});
    ASSERT_FALSE(tilesController.deleteTilesTrigger(
            MapTileTriggerDTO { "S",
            "MustBeFacing",
            "DenyMove",
            { { "Test1", "Test2" }, { "Test3", "Test4" } }}));
}

TEST_F(TilesPropsControllerSample1, DeleteTilesTrigger_WithTriggerUpdateAllTilesUpdated_ReturnTrue) {
    glController.selectTilesForEditing({1, 3});
    ASSERT_TRUE(tilesController.deleteTilesTrigger(
            MapTileTriggerDTO { "SteppedOn",
            "MustBeFacing",
            "DenyMove",
            { { "Test1", "Test2" }, { "Test3", "Test4" } }}));
}

TEST_F(TilesPropsControllerSample1, DeleteTilesTrigger_WithTriggerUpdateInitialNotCommon_ReturnFalse) {
    glController.selectTilesForEditing({2, 4});
    ASSERT_FALSE(tilesController.deleteTilesTrigger(
            MapTileTriggerDTO { "SteppedOn",
            "MustBeFacing",
            "DenyMove",
            { { "Test1", "Test2" }, { "Test3", "Test4" } }}));
}

