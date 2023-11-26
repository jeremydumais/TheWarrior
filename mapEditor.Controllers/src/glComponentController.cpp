#include "glComponentController.hpp"
#include <fmt/format.h>
#include <algorithm>
#include "gameMap.hpp"
#include "mapTile.hpp"
#include "mapTileDTOUtils.hpp"
#include "mapTileTrigger.hpp"
#include "mapTileTriggerEventConverter.hpp"
#include "monsterZone.hpp"
#include "monsterZoneDTOUtils.hpp"
#include "texture.hpp"

using thewarrior::models::MonsterZone;
using thewarrior::models::Texture;
using thewarrior::models::GameMap;
using thewarrior::models::MapTile;
using thewarrior::models::MapTileTrigger;
using thewarrior::models::MapTileTriggerAction;
using thewarrior::models::MapTileTriggerCondition;
using thewarrior::models::MapTileTriggerEvent;
using thewarrior::models::MapTileTriggerEventConverter;
using thewarrior::models::MapTileTriggerAction;
using mapeditor::controllers::MonsterZoneDTO;

namespace mapeditor::controllers {

GLComponentController::GLComponentController()
    : m_map(nullptr),
      m_currentMapTiles({}),
      m_lastError(""),
      m_lastSelectedTextureName(""),
      m_lastSelectedObjectName(""),
      m_lastSelectedTextureIndex(-1),
      m_lastSelectedObjectIndex(-1),
      m_lastSelectedMonsterZoneIndex(-1) {
    }

const std::shared_ptr<GameMap> GLComponentController::getMap() const {
    return m_map;
}

const std::string &GLComponentController::getLastError() const {
    return m_lastError;
}

void GLComponentController::setCurrentMap(std::shared_ptr<GameMap> map) {
    m_map = map;
    m_currentMapTiles.clear();
}

void GLComponentController::selectTilesForEditing(const std::set<int> &indices) {
    m_currentMapTiles = m_map->getTilesForEditing(indices);
}

std::vector<thewarrior::models::MapTile *> GLComponentController::getCurrentMapTiles() {
    return m_currentMapTiles;
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

void GLComponentController::setLastSelectedTexture(const std::string &name,
        int index) {
    this->m_lastSelectedTextureName = name;
    this->m_lastSelectedTextureIndex = index;
}

void GLComponentController::setLastSelectedObject(const std::string &name,
        int index) {
    this->m_lastSelectedObjectName = name;
    this->m_lastSelectedObjectIndex = index;
}

void GLComponentController::clearLastSelectedTexture() {
    m_lastSelectedTextureName = "";
    m_lastSelectedTextureIndex = -1;
}

void GLComponentController::clearLastSelectedObject() {
    m_lastSelectedObjectName = "";
    m_lastSelectedObjectIndex = -1;
}

void GLComponentController::setLastSelectedMonsterZone(int index) {
    m_lastSelectedMonsterZoneIndex = index;
}

void GLComponentController::clearLastSelectedMonsterZone() {
    m_lastSelectedMonsterZoneIndex = -1;
}

void GLComponentController::unselectMapTiles() {
    m_currentMapTiles.clear();
}

void GLComponentController::applyTexture() {
    auto tiles = getCurrentMapTiles();
    for (auto *tile : tiles) {
        tile->setTextureName(m_lastSelectedTextureName);
        tile->setTextureIndex(m_lastSelectedTextureIndex);
    }
}

void GLComponentController::applyObject() {
    auto tiles = getCurrentMapTiles();
    for (auto *tile : tiles) {
        tile->setObjectTextureName(m_lastSelectedObjectName);
        tile->setObjectTextureIndex(m_lastSelectedObjectIndex);
    }
}

void GLComponentController::applyCanStep(bool value) {
    auto tiles = getCurrentMapTiles();
    for (auto *tile : tiles) {
        tile->setCanPlayerSteppedOn(value);
    }
}

bool GLComponentController::applyDenyZone(const std::string &eventStr) {
    auto event = MapTileTriggerEventConverter::eventFromString(eventStr);
    if (!event.has_value()) {
        m_lastError = fmt::format("Unrecognized trigger event string {0}", eventStr);
        return false;
    }
    // Check if the event not already used for another purpose on some tiles
    bool alreadyUsedForAnotherPurpose = false;
    auto tiles = getCurrentMapTiles();
    for (const auto *tile : tiles) {
        const auto foundTrigger = tile->findConstTrigger(event.value());
        if (foundTrigger.has_value()) {
            if (foundTrigger->getAction() != MapTileTriggerAction::DenyMove) {
                alreadyUsedForAnotherPurpose = true;
                break;
            }
        }
    }
    if (alreadyUsedForAnotherPurpose) {
        m_lastError = fmt::format("Trigger {0} is already used for another purpose", eventStr);
        return false;
    }
    for (auto *tile : tiles) {
        const auto foundTrigger = tile->findConstTrigger(event.value());
        if (!foundTrigger.has_value()) {
            tile->addTrigger(MapTileTrigger(event.value(),
                        MapTileTriggerCondition::None,
                        MapTileTriggerAction::DenyMove,
                        {}));
        }
    }

    return true;
}

void GLComponentController::clearDenyZones() {
    auto tiles = getCurrentMapTiles();
    for (auto *tile : tiles) {
        std::vector<MapTileTriggerEvent> events = {
            MapTileTriggerEvent::MoveUpPressed,
            MapTileTriggerEvent::MoveDownPressed,
            MapTileTriggerEvent::MoveLeftPressed,
            MapTileTriggerEvent::MoveRightPressed
        };
        for (auto event : events) {
            auto trigger = tile->findTrigger(event);
            if (trigger.has_value() && trigger->getAction() == MapTileTriggerAction::DenyMove) {
                tile->deleteTrigger(trigger.value());
            }
        }
    }
}

void GLComponentController::applyMonsterZone() {
    auto tiles = getCurrentMapTiles();
    for (auto *tile : tiles) {
        tile->setMonsterZoneIndex(m_lastSelectedMonsterZoneIndex);
    }
}

void GLComponentController::clearMonsterZone() {
    auto tiles = getCurrentMapTiles();
    for (auto *tile : tiles) {
        tile->setMonsterZoneIndex(-1);
    }
}

}  // namespace mapeditor::controllers
