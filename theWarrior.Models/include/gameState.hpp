#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include "player.hpp"
#include "worldState.hpp"

namespace thewarrior::models {

class GameState {
 public:
    GameState(Player &player, WorldState &worldState);

 private:
    friend class boost::serialization::access;
    Player &m_player;
    WorldState &m_worldState;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & m_player;
        ar & m_worldState;
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::GameState, 0)
