#pragma once

#include <map>
#include <string>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include "point.hpp"

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
    friend class boost::serialization::access;
    std::string m_currentMapName;
    Point<> m_playerPosition;
    std::map<std::string, std::vector<int>> m_mapTileIndexActionAlreadyProcessed;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & m_currentMapName;
        ar & m_playerPosition;
        ar & m_mapTileIndexActionAlreadyProcessed;
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::WorldState, 0)
