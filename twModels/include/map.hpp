#pragma once

#include "mapTile.hpp"
#include <vector>

class GameMap
{
public:
    GameMap(unsigned int width, unsigned int height);
    const std::vector<std::vector<MapTile>> &getTiles() const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;
private:
    std::vector<std::vector<MapTile>> tiles;
};