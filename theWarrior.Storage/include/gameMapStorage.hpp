#pragma once

#include "gameMap.hpp"
#include "iBinaryFileStream.hpp"
#include <memory>
#include <string>

class GameMapStorage 
{
public:
    void loadMap(const std::string &fileName, std::shared_ptr<GameMap> map);
    void setFileStream(std::unique_ptr<IBinaryFileStream<GameMap>> bfs);
private:
    std::unique_ptr<IBinaryFileStream<GameMap>> m_bfs;
};
