#include <gtest/gtest.h>
#include "mapTile.hpp"
#include "mapTileDTOUtils.hpp"
#include "mapTileTrigger.hpp"

using mapeditor::controllers::MapTileDTO;
using mapeditor::controllers::MapTileDTOUtils;
using thewarrior::models::MapTile;
using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerEvent;

void MapTileDTOComparer(MapTileDTO val1, MapTileDTO val2) {
    ASSERT_EQ(val1.textureName, val2.textureName);
    ASSERT_EQ(val1.textureIndex, val2.textureIndex);
    ASSERT_EQ(val1.objectTextureName, val2.objectTextureName);
    ASSERT_EQ(val1.objectTextureIndex, val2.objectTextureIndex);
    ASSERT_EQ(val1.canSteppedOn, val2.canSteppedOn);
    ASSERT_EQ(val1.objectAbovePlayer, val2.objectAbovePlayer);
    ASSERT_EQ(val1.isWallToClimb, val2.isWallToClimb);
    ASSERT_EQ(val1.monsterZoneIndex, val2.monsterZoneIndex);
    ASSERT_EQ(val1.triggers, val2.triggers);
}

TEST(MapTileDTOUtils_FromMapTile, WithEmptyTile_ReturnEmptyDTO) {
    MapTile tile;
    MapTileDTO expected;
    MapTileDTOComparer(expected, MapTileDTOUtils::fromMapTile(tile));
}

TEST(MapTileDTOUtils_FromMapTile, WithNonEmptyTile_ReturnNonEmptyDTO) {
    MapTile tile;
    MapTileTrigger trigger1; trigger1.setEvent(MapTileTriggerEvent::None);
    MapTileTrigger trigger2; trigger2.setEvent(MapTileTriggerEvent::SteppedOn);
    MapTileTrigger trigger3; trigger3.setEvent(MapTileTriggerEvent::MoveUpPressed);
    MapTileTrigger trigger4; trigger4.setEvent(MapTileTriggerEvent::MoveDownPressed);
    MapTileTrigger trigger5; trigger5.setEvent(MapTileTriggerEvent::MoveLeftPressed);
    MapTileTrigger trigger6; trigger6.setEvent(MapTileTriggerEvent::MoveRightPressed);
    MapTileTrigger trigger7; trigger7.setEvent(MapTileTriggerEvent::ActionButtonPressed);
    tile.setTextureName("Tex1");
    tile.setTextureIndex(1);
    tile.setObjectTextureName("Obj1");
    tile.setObjectTextureIndex(2);
    tile.setCanPlayerSteppedOn(true);
    tile.setObjectAbovePlayer(true);
    tile.setIsWallToClimb(true);
    tile.setMonsterZoneIndex(3);
    tile.addTrigger(trigger1);
    tile.addTrigger(trigger2);
    tile.addTrigger(trigger3);
    tile.addTrigger(trigger4);
    tile.addTrigger(trigger5);
    tile.addTrigger(trigger6);
    tile.addTrigger(trigger7);
    MapTileDTO expected {
        "Tex1", 1, "Obj1", 2, true, true, true, 3, {
            "None",
            "SteppedOn",
            "MoveUpPressed",
            "MoveDownPressed",
            "MoveLeftPressed",
            "MoveRightPressed",
            "ActionButtonPressed",
        }
    };
    MapTileDTOComparer(expected, MapTileDTOUtils::fromMapTile(tile));
}

TEST(MapTileDTOUtils_ToMapTile, WithEmptyDTO_ReturnEmptyTile) {
    // TODO: 0.3.3 Implement this method
}
