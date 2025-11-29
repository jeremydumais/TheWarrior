#include <algorithm>
#include <string>
#include <vector>
#include "worldState.hpp"

namespace thewarrior::models {

WorldState::WorldState()
: m_currentMapName(""),
m_playerPosition(0, 0) {}

const std::string &WorldState::getCurrentMapName() const {
    return m_currentMapName;
}

const Point<> &WorldState::getPlayerPosition() const {
    return m_playerPosition;
}

bool WorldState::isTileActionAlreadyProcessed(const std::string &mapName,
                                              int tileIndex) const {
    if (const auto tilesProcessed = m_mapTileIndexActionAlreadyProcessed.find(mapName);
            tilesProcessed != m_mapTileIndexActionAlreadyProcessed.end()) {
        const auto &tileIndices = tilesProcessed->second;
        return std::find(begin(tileIndices), end(tileIndices), tileIndex) != tileIndices.end();
    }
    return false;
}

void WorldState::setCurrentMapName(const std::string &name) {
    m_currentMapName = name;
}

void WorldState::setPlayerPosition(const Point<> &position) {
    m_playerPosition = position;
}

void WorldState::addTileActionProcessed(const std::string &mapName,
                                        int tileIndex) {
    if (m_mapTileIndexActionAlreadyProcessed.find(mapName) == m_mapTileIndexActionAlreadyProcessed.end()) {
        m_mapTileIndexActionAlreadyProcessed.insert({mapName, std::vector<int>()});
    }
    auto &tileIndices = m_mapTileIndexActionAlreadyProcessed.at(mapName);
    if (std::find(begin(tileIndices), end(tileIndices), tileIndex) == tileIndices.end()) {
        tileIndices.push_back(tileIndex);
    }
}

void WorldState::movePlayerLeft() {
    m_playerPosition.setX(m_playerPosition.x() - 1);
}

void WorldState::movePlayerUp() {
    m_playerPosition.setY(m_playerPosition.y() - 1);
}

void WorldState::movePlayerDown() {
    m_playerPosition.setY(m_playerPosition.y() + 1);
}

void WorldState::movePlayerRight() {
    m_playerPosition.setX(m_playerPosition.x() + 1);
}

}  // namespace thewarrior::models
