#pragma once

#include <map>
#include <vector>
#include "inventory.hpp"
#include "player.hpp"
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

namespace thewarrior::models {

struct SaveGameCreationInfo {
    Player &player;
    //TODO: Player location, map and facing
    //TODO: std::map<std::string, std::vector<int>> mapTileIndexActionAlreadyProcessed;
    //TODO: Already processed tiles
};

class SaveGame {
 public:
     explicit SaveGame(SaveGameCreationInfo &info);
 private:
    friend class boost::serialization::access;
    SaveGame() = default;  // Needed for deserialization
    SaveGameCreationInfo &m_info;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & m_info.player;
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::SaveGame, 0)
