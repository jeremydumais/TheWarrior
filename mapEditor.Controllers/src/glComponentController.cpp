#include "glComponentController.hpp"
#include <algorithm>
#include "gameMap.hpp"
#include "mapTile.hpp"
#include "mapTileDTOUtils.hpp"
#include "monsterZone.hpp"
#include "monsterZoneDTOUtils.hpp"
#include "texture.hpp"

using thewarrior::models::MonsterZone;
using thewarrior::models::Texture;
using thewarrior::models::GameMap;
using thewarrior::models::MapTile;
using mapeditor::controllers::MonsterZoneDTO;


namespace mapeditor::controllers {

GLComponentController::GLComponentController()
    : m_map(nullptr),
      m_currentMapTiles({}) {
    }

const std::shared_ptr<GameMap> GLComponentController::getMap() const {
    return m_map;
}

void GLComponentController::setCurrentMap(std::shared_ptr<GameMap> map) {
    m_map = map;
    m_currentMapTiles.clear();
}

void GLComponentController::selectTilesForEditing(const std::set<int> &indices) {
    m_currentMapTiles = m_map->getTilesForEditing(indices);
}

std::vector<MapTileDTO> GLComponentController::getSelectedMapTiles() const {
    std::vector<MapTileDTO> retval = {};
    std::for_each(m_currentMapTiles.begin(),
            m_currentMapTiles.end(),
            [&retval](const MapTile *tile) {
        retval.push_back(MapTileDTOUtils::fromMapTile(*tile));
    });
    return retval;
}

std::vector<std::string> GLComponentController::getAlreadyUsedTextureNames() const {
    std::vector<std::string> alreadyUsedTextureNames;
    if (m_map != nullptr) {
        std::transform(m_map->getTextures().begin(),
                       m_map->getTextures().end(),
                       back_inserter(alreadyUsedTextureNames),
                       [](Texture const& x) { return x.getName(); });
    }
    return alreadyUsedTextureNames;
}

std::vector<std::string> GLComponentController::getAlreadyUsedMonsterZoneNames() const {
    std::vector<std::string> alreadyUsedMonsterZoneNames;
    if (m_map != nullptr) {
        std::transform(m_map->getMonsterZones().begin(),
                       m_map->getMonsterZones().end(),
                       back_inserter(alreadyUsedMonsterZoneNames),
                       [](MonsterZone const& x) { return x.getName(); });
    }
    return alreadyUsedMonsterZoneNames;
}

bool isTextureNameUsedInTile(const std::string &name, const MapTile &tile) {
    return tile.getTextureName() == name ||
           tile.getObjectTextureName() == name;
}

bool GLComponentController::isTextureUsedInMap(const std::string &name) {
    for (const auto &row : m_map->getTiles()) {
        if (std::any_of(row.begin(), row.end(), [&name](const auto &tile) {
                return isTextureNameUsedInTile(name, tile); })) {
            return true;
        }
    }
    return false;
}

bool GLComponentController::isShrinkMapImpactAssignedTiles(int offsetLeft,
        int offsetTop,
        int offsetRight,
        int offsetBottom) const {
    return m_map->isShrinkMapImpactAssignedTiles(offsetLeft,
            offsetTop,
            offsetRight,
            offsetBottom);
}

void GLComponentController::resizeMap(int offsetLeft,
        int offsetTop,
        int offsetRight,
        int offsetBottom) {
    m_map->resizeMap(offsetLeft,
            offsetTop,
            offsetRight,
            offsetBottom);
}

std::vector<mapeditor::controllers::MonsterZoneDTO> GLComponentController::getMonsterZones() const {
    std::vector<MonsterZoneDTO> retval = {};
    std::ranges::transform(m_map->getMonsterZones(),
                           std::back_inserter(retval),
                           MonsterZoneDTOUtils::fromMonsterZone);
    return retval;
}

OptMonsterZoneDTOConst GLComponentController::getMonsterZoneByName(const std::string &name) const {
    const auto zoneOpt = m_map->getMonsterZoneByName(name);
    if (zoneOpt.has_value()) {
        const auto zoneDTO = MonsterZoneDTOUtils::fromMonsterZone(zoneOpt->get());
        return OptMonsterZoneDTOConst { zoneDTO };
    }
    return std::nullopt;
}

void GLComponentController::unselectMapTiles() {
    m_currentMapTiles.clear();
}

}  // namespace mapeditor::controllers
