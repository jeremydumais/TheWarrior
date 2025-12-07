#pragma once

#include <string>
#include <vector>
#include "gameStateMetadata.hpp"

namespace thewarrior::storage {

class IGameStateRepository {
 public:
     virtual ~IGameStateRepository() = default;
     virtual const std::string &getLastError() const = 0;
     virtual std::vector<GameStateMetadata> listGameStates() = 0;
     virtual bool save(const GameStateMetadata &gameStateMetadata) = 0;
     virtual void remove(int id) = 0;
     virtual bool initSchema() = 0;
};

}  // namespace thewarrior::storage
