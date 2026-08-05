#include <fmt/core.h>
#include <fmt/format.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include "gameMap.hpp"
#include "monsterZone.hpp"
#include "npc.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

using boost::algorithm::to_upper_copy;

namespace thewarrior::models {

GameMap::GameMap(unsigned int width, unsigned int height)
: m_tiles({}),
m_monsterZones({}),
m_npcs({}),
m_merchantInventories({}) {
    if (width == 0) {
        throw std::invalid_argument("width must be greater than zero.");
    }
    if (height == 0) {
        throw std::invalid_argument("height must be greater than zero.");
    }
    for (unsigned int i = 0; i < height; i++) {
        std::vector<MapTile> row;
        for (unsigned int j = 0; j < width; j++) {
            MapTile newTile;
            row.emplace_back(newTile);
        }
        m_tiles.emplace_back(row);
    }
}

const std::string &GameMap::getLastError() const { return m_lastError; }

const std::vector<std::vector<MapTile>> &GameMap::getTiles() const {
    return m_tiles;
}

MapTile &GameMap::getTileForEditing(int index) {
    if (index < 0) {
        throw std::invalid_argument("index must be a positive number");
    }
    auto indexConverted{static_cast<size_t>(index)};
    return m_tiles.at(indexConverted / getWidth())
        .at(indexConverted % getWidth());
}

MapTile &GameMap::getTileForEditing(Point<> coord) {
    if (coord.x() < 0) {
        throw std::invalid_argument("x must be a positive number");
    }
    if (coord.y() < 0) {
        throw std::invalid_argument("y must be a positive number");
    }
    return m_tiles.at(static_cast<size_t>(coord.y()))
        .at(static_cast<size_t>(coord.x()));
}

std::vector<MapTile *> GameMap::getTilesForEditing(const std::set<int> &indices) {
    std::vector<MapTile *> retval = {};
    std::ranges::for_each(indices,
            [&retval, this](const int index) {
            if (index < 0) {
            return;
            }
            auto indexConverted{static_cast<size_t>(index)};
            if (indexConverted >= getWidth() * getHeight()) {
            return;
            }
            retval.push_back(&m_tiles.at(indexConverted / getWidth())
                    .at(indexConverted % getWidth()));
            });
    return retval;
}

const MapTile &GameMap::getTileFromCoord(Point<> coord) const {
    if (coord.x() < 0) {
        throw std::invalid_argument("x must be a positive number");
    }
    if (coord.y() < 0) {
        throw std::invalid_argument("y must be a positive number");
    }
    return m_tiles.at(static_cast<size_t>(coord.y()))
        .at(static_cast<size_t>(coord.x()));
}

unsigned int GameMap::getWidth() const {
    auto widthSize = m_tiles[0].size();
    if (widthSize > UINT_MAX) {
        return UINT_MAX;
    }
    return static_cast<unsigned int>(widthSize);
}

unsigned int GameMap::getHeight() const {
    auto heightSize = m_tiles.size();
    if (heightSize > UINT_MAX) {
        return UINT_MAX;
    }
    return static_cast<unsigned int>(heightSize);
}

Point<> GameMap::getCoordFromTileIndex(int index) const {
    if (index < 0) {
        throw std::invalid_argument("index must be a positive number");
    }
    auto indexConverted{static_cast<unsigned int>(index)};
    int x = static_cast<int>(indexConverted % getWidth());
    int y = static_cast<int>(indexConverted / getWidth());
    return {x, y};
}

int GameMap::getTileIndexFromCoord(Point<> coord) const {
    if (coord.x() < 0) {
        throw std::invalid_argument("x must be a positive number");
    }
    if (coord.y() < 0) {
        throw std::invalid_argument("y must be a positive number");
    }
    return (coord.y() * static_cast<int>(getWidth())) + coord.x();
}

const std::vector<Texture> &GameMap::getTextures() const {
    return m_textureContainer.getTextures();
}

std::optional<std::reference_wrapper<const Texture>> GameMap::getTextureByName(const std::string &name) const {
    return m_textureContainer.getTextureByName(name);
}
    
void GameMap::setMusicFilename(const std::string &filename) {
    m_musicFilename = filename;
}

bool GameMap::addTexture(const TextureInfo &textureInfo) {
    bool retVal = m_textureContainer.addTexture(textureInfo);
    if (!retVal) {
        m_lastError = m_textureContainer.getLastError();
    }
    return retVal;
}

bool GameMap::replaceTexture(const std::string &name,
        const TextureInfo &textureInfo) {
    bool retVal = m_textureContainer.replaceTexture(name, textureInfo);
    if (!retVal) {
        m_lastError = m_textureContainer.getLastError();
    }
    return retVal;
}

bool GameMap::removeTexture(const std::string &name) {
    bool retVal = m_textureContainer.removeTexture(name);
    if (!retVal) {
        m_lastError = m_textureContainer.getLastError();
    }
    return retVal;
}

const std::vector<MonsterZone> &GameMap::getMonsterZones() const {
    return m_monsterZones;
}

OptMonsterZoneConstRef GameMap::getMonsterZoneByName(const std::string &zoneName) const {
    auto it = std::ranges::find_if(m_monsterZones,
            [zoneName](const MonsterZone &zone) {
            return to_upper_copy(zone.getName()) ==
            to_upper_copy(zoneName);
            });
    if (it != m_monsterZones.end()) {
        return {*it};
    }
    return std::nullopt;
}

const std::string &GameMap::getMusicFilename() const {
    return m_musicFilename;
}

bool GameMap::addMonsterZone(const MonsterZone &zone) {
    // Ensure that the monster zone does not exist
    const auto iter = getMonsterZoneIterator(zone.getName());
    if (iter == m_monsterZones.end()) {
        m_monsterZones.push_back(zone);
        if (m_useOnlyOneMonsterZone) {
            m_useOnlyOneMonsterZone = false;
        }
        return true;
    }

    m_lastError = fmt::format("The zone {0} already exist.", zone.getName());
    return false;
}

bool GameMap::replaceMonsterZone(const std::string &name,
        const MonsterZone &zone) {
    const auto zoneToUpdateIter = getMonsterZoneIterator(name);
    if (zoneToUpdateIter == m_monsterZones.end()) {
        m_lastError = fmt::format("Unable to find the zone {0} to update.", name);
        return false;
    }
    const auto isNameAlreadyExist =
        getMonsterZoneByName(zone.getName()).has_value();
    if (isNameAlreadyExist &&
            to_upper_copy(name) != to_upper_copy(zone.getName())) {
        m_lastError = fmt::format("The zone {0} already exist.", zone.getName());
        return false;
    }
    *zoneToUpdateIter = zone;
    return true;
}

bool GameMap::removeMonsterZone(const std::string &name) {
    const auto zoneToRemoveIter = getMonsterZoneIterator(name);
    if (zoneToRemoveIter == m_monsterZones.end()) {
        m_lastError = fmt::format("Unable to find the zone {0} to delete.", name);
        return false;
    }
    const auto index = std::distance(m_monsterZones.begin(), zoneToRemoveIter);
    // Remove all the monster zone index assignation on tiles
    unassignMonsterZoneOnAllTiles(static_cast<int>(index));
    m_monsterZones.erase(zoneToRemoveIter);
    if (m_useOnlyOneMonsterZone) {
        m_useOnlyOneMonsterZone = false;
    }
    return true;
}

void GameMap::unassignMonsterZoneOnAllTiles(int zoneIndex) {
    for (auto & m_tile : m_tiles) {
        for (auto & tile : m_tile) {
             if (tile.getMonsterZoneIndex() == zoneIndex) {
                tile.setMonsterZoneIndex(-1);
            }
        }
    }
}

bool GameMap::isTileUsedByNPC(const Point<size_t> &coord) const {
    return std::ranges::any_of(m_npcs, [coord](const NPC &npc) {
        bool usedInWanderingZone = std::ranges::any_of(npc.getWanderZone(),
                [coord](const Point<size_t> &zoneCoord) {
                    return zoneCoord == coord;
                });
        return npc.getSpawnPosition() == coord || usedInWanderingZone;
    });
}

bool GameMap::isTilesIndicesUsedByNPC(const std::set<int> &indices) const {
    return std::ranges::any_of(indices, [this](int index) {
        try {
            const auto coord = getCoordFromTileIndex(index);
            return isTileUsedByNPC(Point<size_t>(static_cast<size_t>(coord.x()),
                                                 static_cast<size_t>(coord.y())));
        } catch (const std::invalid_argument &err) { return true; }
    });
}

const std::vector<NPC> &GameMap::getNPCs() const {
    return m_npcs;
}

OptNPCConstRef GameMap::getNPCById(const std::string &id) const {
    const auto iter = std::ranges::find_if(m_npcs, [&id](const auto &npc) {
        return to_upper_copy(npc.getId()) == to_upper_copy(id);
    });
    if (iter == m_npcs.end()) {
        return std::nullopt;
    }
    return {*iter};
}

bool GameMap::addNPC(const NPC &npc) {
    if (std::ranges::any_of(m_npcs, [&npc](const auto &existingNPC) {
                return boost::trim_copy(boost::to_lower_copy(npc.getId())) ==
                boost::trim_copy(boost::to_lower_copy(existingNPC.getId()));
                })) {
        m_lastError = fmt::format("NPC with id {0} already exist", npc.getId());
        return false;
    }

    m_npcs.push_back(npc);
    return true;
}

bool GameMap::replaceNPC(const std::string &npcId, const NPC &npc) {
    auto npcIterator = getNPCIterator(npcId);
    if (npcIterator == m_npcs.end()) {
        m_lastError = fmt::format("NPC with id {0} doesn't exist", npcId);
        return false;
    }
    const auto destinationExist = getNPCIterator(npc.getId()) != m_npcs.end();
    if (destinationExist &&
            boost::to_lower_copy(npcId) != boost::to_lower_copy(npc.getId())) {
        m_lastError = fmt::format("NPC with id {0} already exist", npc.getId());
        return false;
    }
    *npcIterator = npc;
    return true;
}

bool GameMap::removeNPC(const std::string &npcId) {
    const auto npcToRemoveIter = getNPCIterator(npcId);
    if (npcToRemoveIter == m_npcs.end()) {
        m_lastError = fmt::format("Unable to find the NPC {0} to delete.", npcId);
        return false;
    }
    m_npcs.erase(npcToRemoveIter);
    return true;
}

bool GameMap::addNPCWanderingZone(const std::string &npcId, const std::set<int> &selectedTilesIndices) {
    const auto npcIter = getNPCIterator(npcId);
    if (npcIter == m_npcs.end()) {
        m_lastError = fmt::format("Unable to find the NPC {0} to assign wandering zones.", npcId);
        return false;
    }
    // Prepare a list of coordinates that are allowed to be used as wandering zone
    std::vector<Point<size_t>> pointsToAdd;
    for (auto indice : selectedTilesIndices) {
        try {
            const auto coordInt = getCoordFromTileIndex(indice);
            const auto coord = Point<size_t>(static_cast<size_t>(coordInt.x()),
                                             static_cast<size_t>(coordInt.y()));
            const auto &tile = getTileFromCoord(coordInt);
            if (!tile.canPlayerSteppedOn()) {
                m_lastError = fmt::format("Cannot assign the tile at position ({0}, {1}) because it has been marked as 'cannot step on'",
                                          coordInt.x(), coordInt.y());
                return false;
            }
            pointsToAdd.push_back(coord);
        } catch (const std::invalid_argument &err) {
            m_lastError = err.what();
            return false;
        }
    }

    npcIter->addToWanderZone(pointsToAdd);
    return true;
}

bool GameMap::removeNPCWanderingZone(const std::string &npcId, const std::set<int> &selectedTilesIndices) {
    const auto npcIter = getNPCIterator(npcId);
    if (npcIter == m_npcs.end()) {
        m_lastError = fmt::format("Unable to find the NPC {0} to unassign wandering zones.", npcId);
        return false;
    }

    // Prepare a list of coordinates to unassign of the wandering zone
    std::vector<Point<size_t>> pointsToAdd;
    for (auto indice : selectedTilesIndices) {
        try {
            const auto coordInt = getCoordFromTileIndex(indice);
            const auto coord = Point<size_t>(static_cast<size_t>(coordInt.x()),
                                             static_cast<size_t>(coordInt.y()));
            pointsToAdd.push_back(coord);
        } catch (const std::invalid_argument &err) {
            m_lastError = err.what();
            return false;
        }
    }
    npcIter->removeFromWanderZone(pointsToAdd);
    return true;
}

const std::vector<MerchantInventory> &GameMap::getMerchantInventories() const {
    return m_merchantInventories;
}

OptMerchantInventoryConstRef GameMap::getMerchantInventoryByName(const std::string &inventoryName) const {
    auto it = std::ranges::find_if(m_merchantInventories,
            [inventoryName](const MerchantInventory &inventory) {
                return to_upper_copy(inventory.getName()) == to_upper_copy(inventoryName);
            });
    if (it != m_merchantInventories.end()) {
        return {*it};
    }
    return std::nullopt;
}

bool GameMap::addMerchantInventory(const MerchantInventory &inventory) {
    // Ensure that the merchant inventory does not exist
    const auto iter = getMerchantInventoryIterator(inventory.getName());
    if (iter == m_merchantInventories.end()) {
        m_merchantInventories.push_back(inventory);
        return true;
    }

    m_lastError = fmt::format("The merchant inventory {0} already exist.", inventory.getName());
    return false;
}

bool GameMap::replaceMerchantInventory(const std::string &name, const MerchantInventory &inventory) {
    const auto merchantInventoryToUpdateIter = getMerchantInventoryIterator(name);
    if (merchantInventoryToUpdateIter == m_merchantInventories.end()) {
        m_lastError = fmt::format("Unable to find the merchant inventory {0} to update.", name);
        return false;
    }
    const auto isNameAlreadyExist = getMerchantInventoryByName(inventory.getName()).has_value();
    if (isNameAlreadyExist && to_upper_copy(name) != to_upper_copy(inventory.getName())) {
        m_lastError = fmt::format("The merchant inventory {0} already exist.", inventory.getName());
        return false;
    }
    *merchantInventoryToUpdateIter = inventory;
    return true;
}

bool GameMap::removeMerchantInventory(const std::string &name) {
    const auto merchantInventoryToRemoveIter = getMerchantInventoryIterator(name);
    if (merchantInventoryToRemoveIter == m_merchantInventories.end()) {
        m_lastError = fmt::format("Unable to find the merchant inventory {0} to delete.", name);
        return false;
    }
    m_merchantInventories.erase(merchantInventoryToRemoveIter);
    return true;
}

bool GameMap::isShrinkMapImpactAssignedTiles(int offsetLeft, int offsetTop,
        int offsetRight,
        int offsetBottom) const {
    return (_isShrinkMapFromLeftImpactAssignedTiles(offsetLeft) ||
            _isShrinkMapFromTopImpactAssignedTiles(offsetTop) ||
            _isShrinkMapFromRightImpactAssignedTiles(offsetRight) ||
            _isShrinkMapFromBottomImpactAssignedTiles(offsetBottom));
}

bool GameMap::_isShrinkMapFromLeftImpactAssignedTiles(int offset) const {
    if (offset < 0) {
        size_t col = 0;
        for (int index = offset; index < 0; index++) {
            for (size_t rowIndex = 0; rowIndex < getHeight(); rowIndex++) {
                if (m_tiles[rowIndex][col].isAssigned()) {
                    return true;
                }

                if (isTileUsedByNPC({col, rowIndex})) {
                    return true;
                }
            }
            col++;
        }
    }
    return false;
}

bool GameMap::_isShrinkMapFromTopImpactAssignedTiles(int offset) const {
    if (offset < 0) {
        size_t rowIndex{0};
        for (int index = offset; index < 0; index++) {
            // Check all tiles of the column
            for (size_t col = 0; col < getWidth(); col++) {
                if (m_tiles[rowIndex][col].isAssigned()) {
                    return true;
                }

                if (isTileUsedByNPC({col, rowIndex})) {
                    return true;
                }
            }
            rowIndex++;
        }
    }
    return false;
}

bool GameMap::_isShrinkMapFromRightImpactAssignedTiles(int offset) const {
    if (offset < 0) {
        size_t col{m_tiles[0].size() - 1};
        for (int index = offset; index < 0; index++) {
            // Check all tiles of the column
            for (size_t rowIndex = 0; rowIndex < getHeight(); rowIndex++) {
                if (m_tiles[rowIndex][col].isAssigned()) {
                    return true;
                }

                if (isTileUsedByNPC({col, rowIndex})) {
                    return true;
                }
            }
            col--;
        }
    }
    return false;
}

bool GameMap::_isShrinkMapFromBottomImpactAssignedTiles(int offset) const {
    if (offset < 0) {
        size_t rowIndex{m_tiles.size() - 1};
        for (int index = offset; index < 0; index++) {
            // Check all tiles of the column
            for (size_t col = 0; col < getWidth(); col++) {
                if (m_tiles[rowIndex][col].isAssigned()) {
                    return true;
                }

                if (isTileUsedByNPC({col, rowIndex})) {
                    return true;
                }
            }
            rowIndex--;
        }
    }
    return false;
}

void GameMap::resizeMap(int offsetLeft, int offsetTop, int offsetRight,
        int offsetBottom) {
    if (offsetLeft < 0 && abs(offsetLeft) >= static_cast<int64_t>(getWidth())) {
        throw std::invalid_argument(
                "This left offset would remove all the remaining tiles.");
    }
    if (offsetTop < 0 && abs(offsetTop) >= static_cast<int64_t>(getHeight())) {
        throw std::invalid_argument(
                "This top offset would remove all the remaining tiles.");
    }
    if (offsetRight < 0 && abs(offsetRight) >= static_cast<int64_t>(getWidth())) {
        throw std::invalid_argument(
                "This right offset would remove all the remaining tiles.");
    }
    if (offsetBottom < 0 &&
            abs(offsetBottom) >= static_cast<int64_t>(getHeight())) {
        throw std::invalid_argument(
                "This bottom offset would remove all the remaining tiles.");
    }
    _resizeMapFromLeft(offsetLeft);
    _resizeMapFromTop(offsetTop);
    _resizeMapFromRight(offsetRight);
    _resizeMapFromBottom(offsetBottom);
}

void GameMap::_resizeMapFromLeft(int offset) {
    if (offset < 0) {
        const auto k = static_cast<size_t>(-offset);
        for (auto &row : m_tiles) {
            const size_t eraseCount = std::min(k, row.size());
            row.erase(row.begin(),
                    row.begin() + static_cast<std::ptrdiff_t>(eraseCount));
        }
    } else if (offset > 0) {
        for (auto &row : m_tiles) {
            row.insert(row.begin(), static_cast<size_t>(offset), MapTile{});
        }
    }

    // Add the offset to the NPCs Spawn location and Wandering zone
    for (auto &npc : m_npcs) {
        npc.applyCoordinateOffset(offset, 0);
    }
}

void GameMap::_resizeMapFromTop(int offset) {
    if (offset < 0) {
        for (int index = offset; index < 0; index++) {
            m_tiles.erase(m_tiles.begin());
        }
    } else if (offset > 0) {
        for (int index = 0; index < offset; index++) {
            std::vector<MapTile> newRow(getWidth());
            m_tiles.insert(m_tiles.begin(), newRow);
        }
    }
    // Add the offset to the NPCs Spawn location and Wandering zone
    for (auto &npc : m_npcs) {
        npc.applyCoordinateOffset(0, offset);
    }
}

void GameMap::_resizeMapFromRight(int offset) {
    if (offset < 0) {
        for (auto &row : m_tiles) {
            for (int index = offset; index < 0; index++) {
                row.erase(row.end() - 1);
            }
        }
    } else if (offset > 0) {
        for (auto &row : m_tiles) {
            for (int index = 0; index < offset; index++) {
                row.insert(row.end(), MapTile());
            }
        }
    }
}

void GameMap::_resizeMapFromBottom(int offset) {
    if (offset < 0) {
        for (int index = offset; index < 0; index++) {
            m_tiles.erase(m_tiles.end() - 1);
        }
    } else if (offset > 0) {
        for (int index = 0; index < offset; index++) {
            std::vector<MapTile> newRow(getWidth());
            m_tiles.insert(m_tiles.end(), newRow);
        }
    }
}

bool GameMap::canSteppedOnTile(Point<> playerCoord) const {
    return (playerCoord.x() >= 0 &&
            static_cast<unsigned int>(playerCoord.x()) < getWidth() &&
            playerCoord.y() >= 0 &&
            static_cast<unsigned int>(playerCoord.y()) < getHeight() &&
            getTileFromCoord(playerCoord).canPlayerSteppedOn());
}

std::vector<MonsterZone>::iterator GameMap::getMonsterZoneIterator(const std::string &name) {
    return std::ranges::find_if(m_monsterZones, [&name](const auto &zone) {
            return to_upper_copy(zone.getName()) == to_upper_copy(name);
            });
}

std::vector<NPC>::iterator GameMap::getNPCIterator(const std::string &npcId) {
    return std::ranges::find_if(m_npcs, [&npcId](const auto &npc) {
            return to_upper_copy(npc.getId()) == to_upper_copy(npcId);
            });
}

std::vector<MerchantInventory>::iterator GameMap::getMerchantInventoryIterator(const std::string &name) {
    return std::ranges::find_if(m_merchantInventories, [&name](const auto &merchantInventory) {
            return to_upper_copy(merchantInventory.getName()) == to_upper_copy(name);
            });
}

bool GameMap::useOnlyOneMonsterZone() const { return m_useOnlyOneMonsterZone; }

bool GameMap::setUseOnlyOneMonsterZone(bool value) {
    if (value) {
        if (m_monsterZones.size() != 1) {
            m_lastError = "You must have exactly one monster zone.";
            return false;
        }
    }
    m_useOnlyOneMonsterZone = value;
    return true;
}

}  // namespace thewarrior::models
