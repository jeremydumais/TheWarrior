#pragma once

#include "point.hpp"
#include <map>
#include <string>
#include <vector>

namespace thewarrior::models {

class WorldState {
 public:
    WorldState();
    const std::string &getCurrentMapName() const;
    const Point<> &getPlayerPosition() const;
    bool isTileActionAlreadyProcessed(const std::string &mapName, int tileIndex) const;
    void setCurrentMapName(const std::string &name);
    void setPlayerPosition(const Point<> &position);
    void addTileActionProcessed(const std::string &mapName, int tileIndex);
    void movePlayerLeft();
    void movePlayerUp();
    void movePlayerDown();
    void movePlayerRight();
 private:
    std::string m_currentMapName;
    Point<> m_playerPosition;
    std::map<std::string, std::vector<int>> m_mapTileIndexActionAlreadyProcessed;
};

}  // namespace thewarrior::models
