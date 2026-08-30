#include <set>
#include <string>
#include "player.hpp"
#include "gameState.hpp"
#include "story.hpp"

namespace thewarrior::models {

GameState::GameState(Player &player, WorldState &worldState, std::set<StoryId> &completedStoryIds)
: m_player(player),
m_worldState(worldState),
m_completedStoryIds(completedStoryIds) {}

const Player &GameState::getPlayer() const {
    return m_player;
}

const WorldState &GameState::getWorldState() const {
    return m_worldState;
}

const std::set<std::string> &GameState::getCompletedStoryIds() const {
    return m_completedStoryIds;
}


}  // namespace thewarrior::models
