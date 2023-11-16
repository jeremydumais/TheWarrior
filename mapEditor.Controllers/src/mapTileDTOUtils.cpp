#include "mapTileDTOUtils.hpp"
#include <algorithm>
#include <stdexcept>
#include "mapTile.hpp"
#include "mapTileTrigger.hpp"

using thewarrior::models::MapTile;
using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerEvent;

namespace mapeditor::controllers {

std::string getTriggerEventStr(const MapTileTrigger &trigger) {
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
}

MapTileDTO MapTileDTOUtils::fromMapTile(const MapTile &tile) {
    std::vector<std::string> triggerNames = {};
    const auto &triggers = tile.getTriggers();
    std::for_each(triggers.begin(),
        triggers.end(),
        [&triggerNames](const MapTileTrigger &trigger) {
            triggerNames.push_back(getTriggerEventStr(trigger));
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
        triggerNames
    };
}

MapTile MapTileDTOUtils::toMapTile(const MapTileDTO &dto) {
    return MapTile();
}

}  // namespace mapeditor::controllers
