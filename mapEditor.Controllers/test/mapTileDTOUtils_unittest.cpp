#include <gtest/gtest.h>
#include "mapTile.hpp"
#include "mapTileDTOUtils.hpp"
#include "mapTileTrigger.hpp"
#include "mapTileTriggerDTO.hpp"

using mapeditor::controllers::MapTileDTO;
using mapeditor::controllers::MapTileDTOUtils;
using mapeditor::controllers::MapTileTriggerDTO;
using thewarrior::models::MapTile;
using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerEvent;
using thewarrior::models::MapTileTriggerCondition;
using thewarrior::models::MapTileTriggerAction;

void MapTileDTOComparer(MapTileDTO val1, MapTileDTO val2) {
    ASSERT_EQ(val1.textureName, val2.textureName);
    ASSERT_EQ(val1.textureIndex, val2.textureIndex);
    ASSERT_EQ(val1.objectTextureName, val2.objectTextureName);
    ASSERT_EQ(val1.objectTextureIndex, val2.objectTextureIndex);
    ASSERT_EQ(val1.canSteppedOn, val2.canSteppedOn);
    ASSERT_EQ(val1.objectAbovePlayer, val2.objectAbovePlayer);
    ASSERT_EQ(val1.isWallToClimb, val2.isWallToClimb);
    ASSERT_EQ(val1.monsterZoneIndex, val2.monsterZoneIndex);
    ASSERT_EQ(val1.triggers.size(), val2.triggers.size());
    for (size_t i = 0; i < val1.triggers.size(); i++) {
        const auto val1trig = val1.triggers[i];
        const auto val2trig = val2.triggers[i];
        ASSERT_EQ(val1trig.event, val2trig.event);
        ASSERT_EQ(val1trig.condition, val2trig.condition);
        ASSERT_EQ(val1trig.action, val2trig.action);
        ASSERT_EQ(val1trig.actionProperties, val2trig.actionProperties);
    }
}

TEST(MapTileDTOUtils_FromMapTile, WithEmptyTile_ReturnEmptyDTO) {
    MapTile tile;
    MapTileDTO expected;
    MapTileDTOComparer(expected, MapTileDTOUtils::fromMapTile(tile));
}

TEST(MapTileDTOUtils_FromMapTile, WithFilledTile_ReturnFilledDTO) {
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
            MapTileTriggerDTO { "None", "None", "None", {}},
            MapTileTriggerDTO { "SteppedOn", "None", "None", {}},
            MapTileTriggerDTO { "MoveUpPressed", "None", "None", {}},
            MapTileTriggerDTO { "MoveDownPressed", "None", "None", {}},
            MapTileTriggerDTO { "MoveLeftPressed", "None", "None", {}},
            MapTileTriggerDTO { "MoveRightPressed", "None", "None", {}},
            MapTileTriggerDTO { "ActionButtonPressed", "None", "None", {}}
        }
    };
    MapTileDTOComparer(expected, MapTileDTOUtils::fromMapTile(tile));
}

TEST(MapTileDTOUtils_ToMapTile, WithEmptyDTO_ReturnEmptyTile) {
    ASSERT_EQ(MapTile(), MapTileDTOUtils::toMapTile(MapTileDTO {}));
}

TEST(MapTileDTOUtils_ToMapTile, WithFilledDTO_ReturnFilledTile) {
    MapTileDTO dto {
        "Tex1", 1, "Obj1", 2, true, true, true, 3, {
            MapTileTriggerDTO { "None", "MustBeFacing", "DenyMove", {{"Test", "Test1"}}},
            MapTileTriggerDTO { "SteppedOn", "MustHaveItem", "ChangeMap", {}},
            MapTileTriggerDTO { "MoveUpPressed", "None", "OpenChest", {{"Test", "Test1"}, {"Test2", "Test3"}}},
            MapTileTriggerDTO { "MoveDownPressed", "None", "None", {}},
            MapTileTriggerDTO { "MoveLeftPressed", "None", "None", {}},
            MapTileTriggerDTO { "MoveRightPressed", "None", "None", {}},
            MapTileTriggerDTO { "ActionButtonPressed", "None", "None", {}}
        }
    };
    MapTile expected;
    expected.setTextureName("Tex1");
    expected.setTextureIndex(1);
    expected.setObjectTextureName("Obj1");
    expected.setObjectTextureIndex(2);
    expected.setCanPlayerSteppedOn(true);
    expected.setObjectAbovePlayer(true);
    expected.setIsWallToClimb(true);
    expected.setMonsterZoneIndex(3);
    expected.addTrigger(MapTileTrigger(MapTileTriggerEvent::None,
                MapTileTriggerCondition::MustBeFacing, MapTileTriggerAction::DenyMove, {{"Test", "Test1"}}));
    expected.addTrigger(MapTileTrigger(MapTileTriggerEvent::SteppedOn,
                MapTileTriggerCondition::MustHaveItem, MapTileTriggerAction::ChangeMap, {}));
    expected.addTrigger(MapTileTrigger(MapTileTriggerEvent::MoveUpPressed,
                MapTileTriggerCondition::None, MapTileTriggerAction::OpenChest, {{"Test", "Test1"}, {"Test2", "Test3"}}));
    expected.addTrigger(MapTileTrigger(MapTileTriggerEvent::MoveDownPressed,
                MapTileTriggerCondition::None, MapTileTriggerAction::None, {}));
    expected.addTrigger(MapTileTrigger(MapTileTriggerEvent::MoveLeftPressed,
                MapTileTriggerCondition::None, MapTileTriggerAction::None, {}));
    expected.addTrigger(MapTileTrigger(MapTileTriggerEvent::MoveRightPressed,
                MapTileTriggerCondition::None, MapTileTriggerAction::None, {}));
    expected.addTrigger(MapTileTrigger(MapTileTriggerEvent::ActionButtonPressed,
                MapTileTriggerCondition::None, MapTileTriggerAction::None, {}));
    auto t =  MapTileDTOUtils::toMapTile(dto);
    ASSERT_EQ(expected, MapTileDTOUtils::toMapTile(dto));
}
