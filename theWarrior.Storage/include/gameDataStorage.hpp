#pragma once

#include <memory>
#include <string>
#include "gameData.hpp"
#include "iBinaryFileStream.hpp"

namespace thewarrior::storage {

class GameDataStorage {
 public:
    void loadGameData(const std::string &fileName, std::shared_ptr<thewarrior::models::GameData> gameData);
    void saveGameData(const std::string &fileName, std::shared_ptr<thewarrior::models::GameData> gameData);
    void setFileStream(std::unique_ptr<IBinaryFileStream<thewarrior::models::GameData>> bfs);

 private:
    std::unique_ptr<IBinaryFileStream<thewarrior::models::GameData>> m_bfs = nullptr;
};

}  // namespace thewarrior::storage
