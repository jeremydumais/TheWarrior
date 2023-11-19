#include "tilePropsComponentController.hpp"
#include <algorithm>
#include <iterator>
#include <vector>
#include "mapTile.hpp"
#include "mapTileTriggerDTO.hpp"

using thewarrior::models::MapTile;

namespace mapeditor::controllers {

TilePropsComponentController::TilePropsComponentController(GLComponentController *glComponentController)
    : m_glComponentController(glComponentController) {
}

std::vector<MapTileDTO> TilePropsComponentController::getSelectedTiles() const {
    return m_glComponentController->getSelectedMapTiles();
}
std::set<MapTileTriggerDTO> TilePropsComponentController::getSelectedTilesCommonTriggers() const {
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

void TilePropsComponentController::setSelectedTilesTextureName(const std::string &name) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&name](MapTile *tile) {
        tile->setTextureName(name);
    });
}

void TilePropsComponentController::setSelectedTilesTextureIndex(int index) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&index](MapTile *tile) {
        tile->setTextureIndex(index);
    });
}

void TilePropsComponentController::setSelectedTilesObjectTextureName(const std::string &name) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&name](MapTile *tile) {
        tile->setObjectTextureName(name);
    });
}

void TilePropsComponentController::setSelectedTilesObjectTextureIndex(int index) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&index](MapTile *tile) {
        tile->setObjectTextureIndex(index);
    });
}

void TilePropsComponentController::setSelectedTilesObjectAbovePlayer(bool value) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&value](MapTile *tile) {
        tile->setObjectAbovePlayer(value);
    });
}

void TilePropsComponentController::setSelectedTilesCanSteppedOn(bool value) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&value](MapTile *tile) {
        tile->setCanPlayerSteppedOn(value);
    });
}

void TilePropsComponentController::setSelectedTilesIsWallToClimb(bool value) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&value](MapTile *tile) {
        tile->setIsWallToClimb(value);
    });
}

void TilePropsComponentController::setSelectedTilesMonsterZoneIndex(int index) {
    auto tiles = m_glComponentController->getCurrentMapTiles();
    std::for_each(tiles.begin(), tiles.end(), [&index](MapTile *tile) {
        tile->setMonsterZoneIndex(index);
    });
}

void addSelectedTilesTrigger(const MapTileTriggerDTO &trigger) {
    // TODO: 0.3.3 To Complete
    //auto tiles = m_glComponentController->getCurrentMapTiles();
    //std::for_each(tiles.begin(), tiles.end(), [&index](MapTile *tile) {
        //tile->addTrigger(index);
    //});
}

}  // namespace mapeditor::controllers
