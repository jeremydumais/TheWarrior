#pragma once

#include <memory>
#include <string>
#include <vector>
#include "gameState.hpp"
#include "iBinaryFileStream.hpp"

namespace thewarrior::storage {

class GameStateStorage {
 public:
    const std::string &getLastError() const;
    void loadGameState(const std::string &filename, thewarrior::models::GameState &gameState);
    void saveGameState(const std::string &filename, thewarrior::models::GameState &gameState);
    bool deleteGameStates(const std::vector<std::string> &fileNames);
    void setFileStream(std::unique_ptr<IBinaryFileStream<thewarrior::models::GameState>> bfs);

 private:
    std::unique_ptr<IBinaryFileStream<thewarrior::models::GameState>> m_bfs = nullptr;
    std::string m_lastError = "";
};

}  // namespace thewarrior::storage
