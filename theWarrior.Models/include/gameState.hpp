#pragma once

#include <set>
#include <string>
#include <boost/serialization/array_wrapper.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include "player.hpp"
#include "story.hpp"
#include "worldState.hpp"

namespace thewarrior::models {

class GameState {
 public:
    GameState(Player &player, WorldState &worldState, std::set<StoryId> &completedStoryIds);
    const Player &getPlayer() const;
    const WorldState &getWorldState() const;
    const std::set<StoryId> &getCompletedStoryIds() const;

 private:
    friend class boost::serialization::access;
    Player &m_player;
    WorldState &m_worldState;
    std::set<StoryId> &m_completedStoryIds;
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & m_player;
        ar & m_worldState;
        if (version >= 1) {
            ar & m_completedStoryIds;
        }
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::GameState, 1)
