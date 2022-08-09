#pragma once

#include "gameMap.hpp"
#include "iBinaryFileStream.hpp"
#include <memory>
#include <string>

namespace thewarrior::storage {

class GameMapStorage
{
public:
    void loadMap(const std::string &fileName, std::shared_ptr<thewarrior::models::GameMap> map);
    void setFileStream(std::unique_ptr<IBinaryFileStream<thewarrior::models::GameMap>> bfs);
private:
    std::unique_ptr<IBinaryFileStream<thewarrior::models::GameMap>> m_bfs;
};

} // namespace thewarrior::storage
