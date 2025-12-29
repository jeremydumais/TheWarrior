#include "gameState.hpp"

namespace thewarrior::models {

GameState::GameState(Player &player, WorldState &worldState)
: m_player(player),
m_worldState(worldState) {}

const Player &GameState::getPlayer() const {
    return m_player;
}

const WorldState &GameState::getWorldState() const {
    return m_worldState;
}

}  // namespace thewarrior::models
