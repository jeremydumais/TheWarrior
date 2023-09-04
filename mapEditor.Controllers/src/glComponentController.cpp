#include "glComponentController.hpp"
#include <algorithm>
#include "gameMap.hpp"
#include "mapTile.hpp"
#include "texture.hpp"

using thewarrior::models::Texture;
using thewarrior::models::GameMap;
using thewarrior::models::MapTile;

namespace mapeditor::controllers {

GLComponentController::GLComponentController()
    : m_map(nullptr) {
    }

const std::shared_ptr<GameMap> GLComponentController::getMap() const {
    return this->m_map;
}

void GLComponentController::setCurrentMap(std::shared_ptr<GameMap> map) {
    this->m_map = map;
}

std::vector<std::string> GLComponentController::getAlreadyUsedTextureNames() const {
    std::vector<std::string> alreadyUsedTextureNames;
    if (m_map != nullptr) {
        std::transform(m_map->getTextures().begin(), m_map->getTextures().end(), back_inserter(alreadyUsedTextureNames),
                       [](Texture const& x) { return x.getName(); });
    }
    return alreadyUsedTextureNames;
}

std::vector<std::string> GLComponentController::getAlreadyUsedMonsterZoneNames() const {
    return {};
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

}  // namespace mapeditor::controllers
