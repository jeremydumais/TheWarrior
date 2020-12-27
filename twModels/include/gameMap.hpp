#pragma once

#include "mapTile.hpp"
#include "texture.hpp"
#include <string>
#include <vector>
#include <boost/optional.hpp>

class GameMap
{
public:
    GameMap(unsigned int width, unsigned int height);
    const std::string getLastError() const;
    const std::vector<std::vector<MapTile>> &getTiles() const;
    MapTile &getTileForEditing(int index);
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    const std::vector<Texture> &getTextures() const;
    boost::optional<const Texture &> getTextureByName(const std::string &name) const;
    bool addTexture(const TextureInfo &textureInfo);
    bool replaceTexture(const std::string &name, const TextureInfo &textureInfo);
    bool removeTexture(const std::string &name);
private:
    std::string lastError;
    std::vector<std::vector<MapTile>> tiles;
    std::vector<Texture> textures;
};