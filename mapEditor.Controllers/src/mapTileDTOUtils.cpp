#include "mapTileDTOUtils.hpp"
#include <fmt/format.h>
#include <algorithm>
#include <stdexcept>
#include "mapTile.hpp"
#include "mapTileTrigger.hpp"
#include "mapTileTriggerDTO.hpp"
#include "mapTileTriggerDTOUtils.hpp"

using thewarrior::models::MapTile;
using thewarrior::models::MapTileTrigger;

namespace mapeditor::controllers {

MapTileDTO MapTileDTOUtils::fromMapTile(const MapTile &tile) {
    std::vector<MapTileTriggerDTO> triggersDTO = {};
    const auto &triggers = tile.getTriggers();
    std::for_each(triggers.begin(),
        triggers.end(),
        [&triggersDTO](const MapTileTrigger &trigger) {
            triggersDTO.push_back(MapTileTriggerDTOUtils::fromMapTileTrigger(trigger));
        });
    return {
        tile.getTextureName(),
        tile.getTextureIndex(),
        tile.getObjectTextureName(),
        tile.getObjectTextureIndex(),
        tile.canPlayerSteppedOn(),
        tile.getObjectAbovePlayer(),
        tile.getIsWallToClimb(),
        tile.getMonsterZoneIndex(),
        triggersDTO
    };
}

MapTile MapTileDTOUtils::toMapTile(const MapTileDTO &dto) {
    MapTile tile;
    tile.setTextureName(dto.textureName);
    tile.setTextureIndex(dto.textureIndex);
    tile.setObjectTextureName(dto.objectTextureName);
    tile.setObjectTextureIndex(dto.objectTextureIndex);
    tile.setCanPlayerSteppedOn(dto.canSteppedOn);
    tile.setObjectAbovePlayer(dto.objectAbovePlayer);
    tile.setIsWallToClimb(dto.isWallToClimb);
    tile.setMonsterZoneIndex(dto.monsterZoneIndex);
    std::vector<MapTileTrigger> triggers = {};
    const auto &triggersDTO = dto.triggers;
    std::for_each(triggersDTO.begin(),
        triggersDTO.end(),
        [&tile](const MapTileTriggerDTO &triggerDTO) {
            tile.addTrigger(MapTileTriggerDTOUtils::toMapTileTrigger(triggerDTO));
        });
    return tile;
}

}  // namespace mapeditor::controllers
