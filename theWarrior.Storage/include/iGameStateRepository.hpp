#pragma once

#include <string>
#include <vector>
#include "gameStateMetadata.hpp"

namespace thewarrior::storage {

struct SaveGameStateRepositoryResult {
    bool success;
    std::vector<std::string> obsoleteFilePaths;
};

class IGameStateRepository {
 public:
     virtual ~IGameStateRepository() = default;
     virtual const std::string &getLastError() const = 0;
     virtual std::vector<GameStateMetadata> listGameStates() = 0;
     virtual SaveGameStateRepositoryResult save(const GameStateMetadata &gameStateMetadata) = 0;
     virtual void remove(int id) = 0;
     virtual bool initSchema() = 0;
};

}  // namespace thewarrior::storage
