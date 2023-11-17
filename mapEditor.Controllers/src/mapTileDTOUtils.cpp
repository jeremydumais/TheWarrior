#include "mapTileDTOUtils.hpp"
#include <fmt/format.h>
#include <algorithm>
#include <stdexcept>
#include "mapTile.hpp"
#include "mapTileTrigger.hpp"
#include "mapTileTriggerDTO.hpp"

using thewarrior::models::MapTile;
using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerEvent;
using thewarrior::models::MapTileTriggerCondition;
using thewarrior::models::MapTileTriggerAction;

namespace mapeditor::controllers {

MapTileTriggerDTO convertTriggerToDTO(const MapTileTrigger &trigger) {
    MapTileTriggerDTO dto;
    dto.event = [&trigger]() {
    switch (trigger.getEvent()) {
        case MapTileTriggerEvent::None:
            return "None";
        case MapTileTriggerEvent::SteppedOn:
            return "SteppedOn";
        case MapTileTriggerEvent::MoveUpPressed:
            return "MoveUpPressed";
        case MapTileTriggerEvent::MoveDownPressed:
            return "MoveDownPressed";
        case MapTileTriggerEvent::MoveLeftPressed:
            return "MoveLeftPressed";
        case MapTileTriggerEvent::MoveRightPressed:
            return "MoveRightPressed";
        case MapTileTriggerEvent::ActionButtonPressed:
            return "ActionButtonPressed";
    }
    throw std::invalid_argument("Unmanaged trigger event");
    }();
    return dto;
}

MapTileDTO MapTileDTOUtils::fromMapTile(const MapTile &tile) {
    std::vector<MapTileTriggerDTO> triggersDTO = {};
    const auto &triggers = tile.getTriggers();
    std::for_each(triggers.begin(),
        triggers.end(),
        [&triggersDTO](const MapTileTrigger &trigger) {
            triggersDTO.push_back(convertTriggerToDTO(trigger));
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

MapTileTrigger convertDTOToTrigger(const MapTileTriggerDTO &dto) {
    MapTileTrigger trigger;
    trigger.setEvent([&dto]() {
        if (dto.event == "None") {
            return MapTileTriggerEvent::None;
        } else if (dto.event == "SteppedOn") {
            return MapTileTriggerEvent::SteppedOn;
        } else if (dto.event == "MoveUpPressed") {
            return MapTileTriggerEvent::MoveUpPressed;
        } else if (dto.event == "MoveDownPressed") {
            return MapTileTriggerEvent::MoveDownPressed;
        } else if (dto.event == "MoveLeftPressed") {
            return MapTileTriggerEvent::MoveLeftPressed;
        } else if (dto.event == "MoveRightPressed") {
            return MapTileTriggerEvent::MoveRightPressed;
        } else if (dto.event == "ActionButtonPressed") {
            return MapTileTriggerEvent::ActionButtonPressed;
        }
        throw std::invalid_argument(fmt::format("Unknown trigger event value : {0}", dto.event));
    }());
    trigger.setCondition([&dto]() {
        if (dto.condition == "None") {
            return MapTileTriggerCondition::None;
        } else if (dto.condition == "MustHaveItem") {
            return MapTileTriggerCondition::MustHaveItem;
        } else if (dto.condition == "MustBeFacing") {
            return MapTileTriggerCondition::MustBeFacing;
        }
        throw std::invalid_argument(fmt::format("Unknown trigger condition value : {0}", dto.condition));
    }());
    trigger.setAction([&dto]() {
        if (dto.action == "None") {
            return MapTileTriggerAction::None;
        } else if (dto.action == "OpenChest") {
            return MapTileTriggerAction::OpenChest;
        } else if (dto.action == "ChangeMap") {
            return MapTileTriggerAction::ChangeMap;
        } else if (dto.action == "DenyMove") {
            return MapTileTriggerAction::DenyMove;
        }
        throw std::invalid_argument(fmt::format("Unknown trigger action value : {0}", dto.action));
    }());
    trigger.setActionProperties(dto.actionProperties);
    return trigger;
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
            tile.addTrigger(convertDTOToTrigger(triggerDTO));
        });
    return tile;
}

}  // namespace mapeditor::controllers
