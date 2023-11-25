#include "tilePropsComponentController.hpp"
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <vector>
#include "mapTile.hpp"
#include "mapTileTrigger.hpp"
#include "mapTileTriggerDTO.hpp"
#include "mapTileTriggerDTOUtils.hpp"

using thewarrior::models::MapTile;
using thewarrior::models::MapTileTrigger;

namespace mapeditor::controllers {

TilePropsComponentController::TilePropsComponentController(GLComponentController *glComponentController)
    : m_glComponentController(glComponentController) {
}

const std::string &TilePropsComponentController::getLastError() const {
    return m_lastError;
}

std::vector<MapTileDTO> TilePropsComponentController::getSelectedTiles() const {
    return m_glComponentController->getSelectedMapTiles();
}
std::set<MapTileTriggerDTO> TilePropsComponentController::getTilesCommonTriggers() const {
    auto tiles = m_glComponentController->getSelectedMapTiles();
    std::set<MapTileTriggerDTO> commonTriggers = {};
    if (tiles.size() == 0) {
        return commonTriggers;
    }
    commonTriggers = std::set<MapTileTriggerDTO>(tiles[0].triggers.begin(),
            tiles[0].triggers.end());
    std::for_each(tiles.begin() + 1, tiles.end(), [&commonTriggers](const auto &tileDTO) {
        std::set<MapTileTriggerDTO> intersectTriggers = {};
        std::set_intersection(commonTriggers.begin(), commonTriggers.end(),
                tileDTO.triggers.begin(), tileDTO.triggers.end(),
                std::inserter(intersectTriggers, intersectTriggers.begin()));
        commonTriggers = intersectTriggers;
    });
    return commonTriggers;
}

boost::optional<MapTileTriggerDTO> TilePropsComponentController::findMapTileTriggerByEvent(const std::string &event) {
    auto commonTriggers = getTilesCommonTriggers();
    const auto iter = std::find_if(commonTriggers.begin(), commonTriggers.end(), [&event](const auto &trigDTO){
            return trigDTO.event == event;
            });
    if (iter != commonTriggers.end()) {
        return *iter;
    }
    return {};
}

void TilePropsComponentController::setTilesTextureName(const std::string &name) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&name](MapTile *tile) {
        tile->setTextureName(name);
    });
}

void TilePropsComponentController::setTilesTextureIndex(int index) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&index](MapTile *tile) {
        tile->setTextureIndex(index);
    });
}

void TilePropsComponentController::setTilesObjectTextureName(const std::string &name) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&name](MapTile *tile) {
        tile->setObjectTextureName(name);
    });
}

void TilePropsComponentController::setTilesObjectTextureIndex(int index) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&index](MapTile *tile) {
        tile->setObjectTextureIndex(index);
    });
}

void TilePropsComponentController::setTilesObjectAbovePlayer(bool value) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&value](MapTile *tile) {
        tile->setObjectAbovePlayer(value);
    });
}

void TilePropsComponentController::setTilesCanSteppedOn(bool value) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&value](MapTile *tile) {
        tile->setCanPlayerSteppedOn(value);
    });
}

void TilePropsComponentController::setTilesIsWallToClimb(bool value) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&value](MapTile *tile) {
        tile->setIsWallToClimb(value);
    });
}

void TilePropsComponentController::setTilesMonsterZoneIndex(int index) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&index](MapTile *tile) {
        tile->setMonsterZoneIndex(index);
    });
}

bool TilePropsComponentController::addTilesTrigger(const MapTileTriggerDTO &triggerDTO) {
    MapTileTrigger trigger;
    try {
        trigger = MapTileTriggerDTOUtils::toMapTileTrigger(triggerDTO);
    } catch(const std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&trigger](MapTile *tile) {
        tile->addTrigger(trigger);
    });
    return true;
}

bool TilePropsComponentController::updateTilesTrigger(const MapTileTriggerDTO &initialTriggerDTO,
        const MapTileTriggerDTO &updatedTriggerDTO) {
    MapTileTrigger initialTrigger;
    MapTileTrigger updatedTrigger;
    try {
        initialTrigger = MapTileTriggerDTOUtils::toMapTileTrigger(initialTriggerDTO);
        updatedTrigger = MapTileTriggerDTOUtils::toMapTileTrigger(updatedTriggerDTO);
    } catch(const std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    auto tiles = m_glComponentController->getCurrentMapTiles();
    bool allTilesResult = true;
    std::for_each(tiles.begin(), tiles.end(), [&allTilesResult, &initialTrigger, &updatedTrigger](MapTile *tile) {
        if (!tile->updateTrigger(initialTrigger, updatedTrigger)) {
            allTilesResult = false;
        }
    });
    return allTilesResult;
}

bool TilePropsComponentController::deleteTilesTrigger(const MapTileTriggerDTO &triggerDTO) {
    MapTileTrigger trigger;
    try {
        trigger = MapTileTriggerDTOUtils::toMapTileTrigger(triggerDTO);
    } catch(const std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    auto tiles = m_glComponentController->getCurrentMapTiles();
    bool allTilesResult = true;
    std::for_each(tiles.begin(), tiles.end(), [&allTilesResult, &trigger](MapTile *tile) {
        if (!tile->deleteTrigger(trigger)) {
            allTilesResult = false;
        }
    });
    return allTilesResult;
}

}  // namespace mapeditor::controllers
