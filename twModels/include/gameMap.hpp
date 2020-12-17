#pragma once

#include "mapTile.hpp"
#include "texture.hpp"
#include <string>
#include <vector>

class GameMap
{
public:
    GameMap(unsigned int width, unsigned int height);
    const std::vector<std::vector<MapTile>> &getTiles() const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    void addTexture(const std::string &name, const std::string &filename);
private:
    std::vector<std::vector<MapTile>> tiles;
    std::vector<Texture> textures;
};