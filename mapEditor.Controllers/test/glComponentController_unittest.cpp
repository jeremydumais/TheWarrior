#include <gtest/gtest.h>
#include <map>
#include "glComponentController.hpp"
#include "mainController.hpp"
#include "mapTileTrigger.hpp"
#include "monsterZone.hpp"
#include "rgbItemColor.hpp"

using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerEvent;
using thewarrior::models::MapTileTriggerCondition;
using thewarrior::models::MapTileTriggerAction;
using thewarrior::models::MonsterZone;
using thewarrior::models::RGBItemColor;

namespace mapeditor::controllers::glcomponentcontroller::unittest {

class SampleGLComponentController : public ::testing::Test {
 public:
    SampleGLComponentController() {
        mainController.setGLComponentController(&glComponentController);
        mainController.createMap(6, 6);
        auto map { mainController.getMap() };
        map->addTexture({
                "tex1",
                "tex1.png",
                512, 256,
                32, 32
                });
        map->addTexture({
                "tex2",
                "tex2.png",
                1024, 512,
                32, 32
                });
        map->addMonsterZone(MonsterZone("Zone1", RGBItemColor("Black", "#000000")));
        auto &tile { map->getTileForEditing(0) };
        tile.setTextureName("tex1");
        tile.setTextureIndex(0);
        tile.setObjectTextureName("tex1");
        tile.setObjectTextureIndex(0);
        tile.addTrigger(MapTileTrigger(MapTileTriggerEvent::MoveUpPressed,
                    MapTileTriggerCondition::None,
                    MapTileTriggerAction::DenyMove,
                    {}));
        auto &tile1 { map->getTileForEditing(1) };
        tile1.addTrigger(MapTileTrigger(MapTileTriggerEvent::MoveUpPressed,
                    MapTileTriggerCondition::MustBeFacing,
                    MapTileTriggerAction::ChangeMap,
                    {{ "a", "b"}, {"c", "d"}}));
        auto &tile3 { map->getTileForEditing(3) };
        tile3.addTrigger(MapTileTrigger(MapTileTriggerEvent::MoveUpPressed,
                    MapTileTriggerCondition::MustBeFacing,
                    MapTileTriggerAction::DenyMove,
                    {{"a", "b"}}));
        glComponentController.setCurrentMap(map);
    }
    MainController mainController;
    GLComponentController glComponentController;
};


// N = Not assigned, A = Assigned
//      0 1 2 3 4 5
//      -----------
//   0  N N N N N N
//   6  N N A N N N
//  12  N A A A N N
//  18  N N A N N N
//  24  N N N N A N
//  30  N N N N N N
class SampleGLComponentControllerWithTilesAssigned : public ::testing::Test {
 public:
    SampleGLComponentControllerWithTilesAssigned() {
        mainController.setGLComponentController(&glComponentController);
        mainController.createMap(6, 6);
        auto map { mainController.getMap() };
        map->addTexture({
                "tex1",
                "tex1.png",
                512, 256,
                32, 32
                });
        auto &tile8 { map->getTileForEditing(8) };
        tile8.setTextureName("tex1");
        tile8.setTextureIndex(0);
        auto &tile13 { map->getTileForEditing(13) };
        tile13.setTextureIndex(0);
        auto &tile14 { map->getTileForEditing(14) };
        tile14.setTextureIndex(0);
        auto &tile15 { map->getTileForEditing(15) };
        tile15.setTextureIndex(1);
        auto &tile20 { map->getTileForEditing(20) };
        tile20.setObjectTextureIndex(1);
        auto &tile28 { map->getTileForEditing(28) };
        tile28.setObjectTextureName("tex1");
        glComponentController.setCurrentMap(map);
    }
    MainController mainController;
    GLComponentController glComponentController;
};

TEST(GLComponentController_getAlreadyUsedTextureNames, DefaultConstructor_ReturnEmptyVector) {
    GLComponentController glComponentController;
    ASSERT_EQ(0, glComponentController.getAlreadyUsedTextureNames().size());
}

TEST(GLComponentController_getAlreadyUsedMonsterZoneNames, DefaultConstructor_ReturnEmptyVector) {
    GLComponentController glComponentController;
    ASSERT_EQ(0, glComponentController.getAlreadyUsedMonsterZoneNames().size());
}

TEST_F(SampleGLComponentController, getAlreadyUsedTextureNames_WithTwoTexture_ReturnVectorSizeTwo) {
    ASSERT_EQ(2, glComponentController.getAlreadyUsedTextureNames().size());
}

TEST_F(SampleGLComponentController, getAlreadyUsedMonsterZoneNames_WithOneZone_ReturnVectorSizeOne) {
    ASSERT_EQ(1, glComponentController.getAlreadyUsedMonsterZoneNames().size());
    ASSERT_EQ("Zone1", glComponentController.getAlreadyUsedMonsterZoneNames()[0]);
}

TEST_F(SampleGLComponentController, isTextureUsedInMap_WithUnusedTexture_ReturnFalse) {
    ASSERT_FALSE(glComponentController.isTextureUsedInMap("x"));
}

TEST_F(SampleGLComponentController, isTextureUsedInMap_WithUsedTexture_ReturnTrue) {
    ASSERT_TRUE(glComponentController.isTextureUsedInMap("tex1"));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnLeft_ReturnFalse) {
    ASSERT_FALSE(glComponentController.isShrinkMapImpactAssignedTiles(-1, 0, 0, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnLeft_ReturnTrue) {
    ASSERT_TRUE(glComponentController.isShrinkMapImpactAssignedTiles(-2, 0, 0, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnTop_ReturnFalse) {
    ASSERT_FALSE(glComponentController.isShrinkMapImpactAssignedTiles(0, -1, 0, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnTop_ReturnTrue) {
    ASSERT_TRUE(glComponentController.isShrinkMapImpactAssignedTiles(0, -2, 0, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnRight_ReturnFalse) {
    ASSERT_FALSE(glComponentController.isShrinkMapImpactAssignedTiles(0, 0, -1, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnRight_ReturnTrue) {
    ASSERT_TRUE(glComponentController.isShrinkMapImpactAssignedTiles(0, 0, -2, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnBottom_ReturnFalse) {
    ASSERT_FALSE(glComponentController.isShrinkMapImpactAssignedTiles(0, 0, 0, -1));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnBottom_ReturnTrue) {
    ASSERT_TRUE(glComponentController.isShrinkMapImpactAssignedTiles(0, 0, 0, -2));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, resizeMap_WithMinusOneOnLeft_ReturnSuccess) {
    glComponentController.resizeMap(-1, 0, 0, 0);
    ASSERT_EQ(5, glComponentController.getMap()->getWidth());
    ASSERT_EQ(6, glComponentController.getMap()->getHeight());
}

TEST_F(SampleGLComponentController, getMonsterZoneByName_WithEmpty_ReturnEmpty) {
    const auto zoneOpt = glComponentController.getMonsterZoneByName("");
    ASSERT_FALSE(zoneOpt.has_value());
}

TEST_F(SampleGLComponentController, getMonsterZoneByName_WithZone1_ReturnZone) {
    const auto zoneOpt = glComponentController.getMonsterZoneByName("Zone1");
    ASSERT_TRUE(zoneOpt.has_value());
    ASSERT_EQ("Zone1", zoneOpt.value().m_name);
}

TEST_F(SampleGLComponentController, applyDenyZone_WithInvalidEventStr_ReturnFalse) {
    ASSERT_FALSE(glComponentController.applyDenyZone("a"));
    ASSERT_EQ("Unrecognized trigger event string a", glComponentController.getLastError());
}

TEST_F(SampleGLComponentController, applyDenyZone_WithMoveUpPressedAndEventIsAlreadyUsedForAnotherAction_ReturnFalse) {
    glComponentController.selectTilesForEditing({1});
    ASSERT_FALSE(glComponentController.applyDenyZone("MoveUpPressed"));
    ASSERT_EQ("Trigger MoveUpPressed is already used for another purpose", glComponentController.getLastError());
}

TEST_F(SampleGLComponentController, applyDenyZone_WithMoveUpPressedAndEventIsAlreadyUsedForAnotherActionOn1Tile_ReturnFalse) {
    glComponentController.selectTilesForEditing({0, 1});
    ASSERT_FALSE(glComponentController.applyDenyZone("MoveUpPressed"));
    ASSERT_EQ("Trigger MoveUpPressed is already used for another purpose", glComponentController.getLastError());
}

TEST_F(SampleGLComponentController, applyDenyZone_WithMoveUpPressedAndEventIsAlreadyUsedSameAction_ReturnFalse) {
    glComponentController.selectTilesForEditing({0, 1});
    ASSERT_FALSE(glComponentController.applyDenyZone("MoveUpPressed"));
    ASSERT_EQ("Trigger MoveUpPressed is already used for another purpose", glComponentController.getLastError());
}

TEST_F(SampleGLComponentController, applyDenyZone_WithMoveUpPressedAndEventIsAlreadyUsedSameAction_ReturnTrue) {
    glComponentController.selectTilesForEditing({0});
    ASSERT_TRUE(glComponentController.applyDenyZone("MoveUpPressed"));
    const auto tiles = glComponentController.getCurrentMapTiles();
    const auto newTrigger = tiles[0]->findConstTrigger(MapTileTriggerEvent::MoveUpPressed);
    ASSERT_EQ(MapTileTriggerEvent::MoveUpPressed, newTrigger->getEvent());
    ASSERT_EQ(MapTileTriggerCondition::None, newTrigger->getCondition());
    ASSERT_EQ(MapTileTriggerAction::DenyMove, newTrigger->getAction());
    ASSERT_EQ(0, newTrigger->getActionProperties().size());
}

TEST_F(SampleGLComponentController, applyDenyZone_WithMoveUpPressedAndEventIsAlreadyUsedSameActionOn1Tile_ReturnTrue) {
    glComponentController.selectTilesForEditing({0, 2, 3});
    ASSERT_TRUE(glComponentController.applyDenyZone("MoveUpPressed"));
    const auto tiles = glComponentController.getCurrentMapTiles();
    const auto triggerTile0 = tiles[0]->findConstTrigger(MapTileTriggerEvent::MoveUpPressed);
    ASSERT_EQ(MapTileTriggerEvent::MoveUpPressed, triggerTile0->getEvent());
    ASSERT_EQ(MapTileTriggerCondition::None, triggerTile0->getCondition());
    ASSERT_EQ(MapTileTriggerAction::DenyMove, triggerTile0->getAction());
    ASSERT_EQ(0, triggerTile0->getActionProperties().size());
    const auto newTrigger = tiles[1]->findConstTrigger(MapTileTriggerEvent::MoveUpPressed);
    ASSERT_EQ(MapTileTriggerEvent::MoveUpPressed, newTrigger->getEvent());
    ASSERT_EQ(MapTileTriggerCondition::None, newTrigger->getCondition());
    ASSERT_EQ(MapTileTriggerAction::DenyMove, newTrigger->getAction());
    ASSERT_EQ(0, newTrigger->getActionProperties().size());
    const auto triggerTile3 = tiles[2]->findConstTrigger(MapTileTriggerEvent::MoveUpPressed);
    ASSERT_EQ(MapTileTriggerEvent::MoveUpPressed, triggerTile3->getEvent());
    ASSERT_EQ(MapTileTriggerCondition::MustBeFacing, triggerTile3->getCondition());
    ASSERT_EQ(MapTileTriggerAction::DenyMove, triggerTile3->getAction());
    ASSERT_EQ(1, triggerTile3->getActionProperties().size());
}

TEST_F(SampleGLComponentController, clearDenyZones_WithNoTriggersTile_ReturnSuccess) {
    glComponentController.selectTilesForEditing({2});
    glComponentController.clearDenyZones();
    const auto tiles = glComponentController.getCurrentMapTiles();
    ASSERT_EQ(1, tiles.size());
    ASSERT_EQ(0, tiles[0]->getTriggers().size());
}

TEST_F(SampleGLComponentController, clearDenyZones_WithOneTriggerTile_ReturnSuccess) {
    glComponentController.selectTilesForEditing({0});
    glComponentController.clearDenyZones();
    const auto tiles = glComponentController.getCurrentMapTiles();
    ASSERT_EQ(1, tiles.size());
    ASSERT_EQ(0, tiles[0]->getTriggers().size());
}

TEST_F(SampleGLComponentController, clearDenyZones_WithOneTriggerNotDenyMove_ReturnSuccess) {
    glComponentController.selectTilesForEditing({1});
    glComponentController.clearDenyZones();
    const auto tiles = glComponentController.getCurrentMapTiles();
    ASSERT_EQ(1, tiles.size());
    ASSERT_EQ(1, tiles[0]->getTriggers().size());
}

TEST_F(SampleGLComponentController, clearDenyZones_WithTwoTriggersDenyMoveAndOneNot_ReturnSuccess) {
    glComponentController.selectTilesForEditing({0, 1, 3});
    glComponentController.clearDenyZones();
    const auto tiles = glComponentController.getCurrentMapTiles();
    ASSERT_EQ(3, tiles.size());
    ASSERT_EQ(0, tiles[0]->getTriggers().size());
    ASSERT_EQ(1, tiles[1]->getTriggers().size());
    ASSERT_EQ(0, tiles[2]->getTriggers().size());
}

}  // namespace mapeditor::controllers::glcomponentcontroller::unittest
