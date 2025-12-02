#pragma once

#include <vector>
#include "gameState.hpp"
#include "gameStateMetadata.hpp"

namespace thewarrior::storage {

class IGameStateRepository {
 public:
     virtual ~IGameStateRepository() = default;
     virtual std::vector<GameStateMetadata> listGameStates() = 0;
     virtual void save(const models::GameState &gameState) = 0;
     virtual void remove(int id) = 0;
};

}  // namespace thewarrior::storage
