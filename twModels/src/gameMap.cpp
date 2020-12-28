#include "gameMap.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <stdexcept>

using namespace std;

GameMap::GameMap(unsigned int width, unsigned int height)
    : lastError(""),
      textures(vector<Texture>())
{
    if (width == 0) {
        throw invalid_argument("width must be greater than zero.");
    }
    if (height == 0) {
        throw invalid_argument("height must be greater than zero.");
    }
    for(unsigned int i = 0; i < width; i++) {
        vector<MapTile> col;
        for(unsigned int j = 0; j < height; j++) {
            MapTile newTile;
            col.emplace_back(newTile);
        }
        tiles.emplace_back(col);
    }
}

const std::string GameMap::getLastError() const
{
    return lastError;
}

const std::vector<std::vector<MapTile>>& GameMap::getTiles() const
{
    return tiles;
}

MapTile& GameMap::getTileForEditing(int index)
{
    return tiles.at(index / getWidth()).at(index % getWidth());
}

unsigned int GameMap::getWidth() const
{
    return tiles.size();    
}

unsigned int GameMap::getHeight() const
{
    return tiles[0].size();
}

const vector<Texture>& GameMap::getTextures() const
{
    return textures;
}

boost::optional<const Texture &> GameMap::getTextureByName(const std::string &name) const
{
    for(const auto &texture : textures) {
        if (texture.getName() == name) {
            return texture;
        }
    }
    return {};
}

bool GameMap::addTexture(const TextureInfo &textureInfo) 
{
    //Check that name doesn't already exist in the list
    if (getTextureByName(textureInfo.name).has_value()) {
        lastError = fmt::format("The texture name {0} already exist in the list", textureInfo.name);
        return false;
    }
    try {
        textures.emplace_back(textureInfo);
    }
    catch(invalid_argument &err) {
        lastError = err.what();
        return false;
    }
    return true;
}

bool GameMap::replaceTexture(const std::string &name, const TextureInfo &textureInfo) 
{
    //Find the texture to replace
    auto iter { getTextureIterator(name) };
    if (iter == textures.end()) {
        lastError = fmt::format("Unable to find the texture {0} in the texture list.", name);
        return false;
    }
    //Ensure the new name doesn't exist
    if (name != textureInfo.name && getTextureIterator(textureInfo.name) != textures.end()) {
        lastError = fmt::format("The texture {0} already exist in the texture list.", textureInfo.name);
        return false;
    }
    //Try to construct the new texture
    try {
        Texture newTexture(textureInfo);
        swap(*iter, newTexture);
    }
    catch(invalid_argument &err) {
        lastError = err.what();
        return false;
    }
    return true;
}

bool GameMap::removeTexture(const std::string &name) 
{
    //Find the texture to delete
    auto iter { getTextureIterator(name) };
    if (iter == textures.end()) {
        lastError = fmt::format("Unable to find the texture {0} in the texture list.", name);
        return false;
    }
    textures.erase(iter);
    return true;
}

std::vector<Texture>::iterator GameMap::getTextureIterator(const std::string &name) 
{
    return find_if(textures.begin(), textures.end(), [&name](const Texture &x) {
        return x.getName() == name;
    });
}
