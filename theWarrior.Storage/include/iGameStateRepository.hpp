#pragma once

#include <string>
#include <vector>
#include "gameStateMetadata.hpp"

namespace thewarrior::storage {

struct GameStateRepositorySaveResult {
    bool success;
    std::vector<std::string> obsoleteFilePaths;
};

struct GameStateRepositoryListResult {
    bool success;
    std::vector<GameStateMetadata> gameStateList;
};

class IGameStateRepository {
 public:
     virtual ~IGameStateRepository() = default;
     virtual const std::string &getLastError() const = 0;
     virtual GameStateRepositoryListResult getAllGameStates() = 0;
     virtual GameStateRepositorySaveResult save(const GameStateMetadata &gameStateMetadata) = 0;
     virtual void remove(int id) = 0;
     virtual bool initSchema() = 0;
};

}  // namespace thewarrior::storage
