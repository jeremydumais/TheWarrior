#include "gameState.hpp"

namespace thewarrior::models {

GameState::GameState(Player &player, WorldState &worldState)
: m_player(player),
m_worldState(worldState) {}

}  // namespace thewarrior::models
