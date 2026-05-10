#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include "npc.hpp"
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
    std::vector<Point<size_t>> getAllNPCPositions() const;
    const Point<size_t> &getNPCPosition(const std::string &npcId) const;
    NPCFacing getNPCFacing(const std::string &npcId) const;
    void clearNPCsState();
    void setNPCPosition(const std::string &npcId, const Point<size_t> &position);
    void setNPCFacing(const std::string &npcId, const NPCFacing &facing);

 private:
    friend class boost::serialization::access;
    std::string m_currentMapName;
    Point<> m_playerPosition;
    std::map<std::string, Point<size_t>> m_npcsPositionByName;
    std::map<std::string, NPCFacing> m_npcsFacingByName;
    std::map<std::string, std::vector<int>> m_mapTileIndexActionAlreadyProcessed;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & m_currentMapName;
        ar & m_playerPosition;
        ar & m_mapTileIndexActionAlreadyProcessed;
        if (version >= 1) {
            ar & m_npcsPositionByName;
            ar & m_npcsFacingByName;
        }
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::WorldState, 1)
