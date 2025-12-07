#pragma once

#include <memory>
#include "gameState.hpp"
#include "iBinaryFileStream.hpp"

namespace thewarrior::storage {

class GameStateStorage {
 public:
    void loadGameState(thewarrior::models::GameState &gameState);
    void saveGameState(const std::string &filename, thewarrior::models::GameState &gameState);
    void setFileStream(std::unique_ptr<IBinaryFileStream<thewarrior::models::GameState>> bfs);

 private:
    std::unique_ptr<IBinaryFileStream<thewarrior::models::GameState>> m_bfs = nullptr;
};

}  // namespace thewarrior::storage
