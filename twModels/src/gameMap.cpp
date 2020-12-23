#include "gameMap.hpp"
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
    int texIndex { 0 };
    for(unsigned int i = 0; i < width; i++) {
        vector<MapTile> col;
        for(unsigned int j = 0; j < height; j++) {
            MapTile newTile;
            if (i == 0 && j == 0) {
                newTile.setTextureName("terrain1");
                newTile.setTextureIndex(258);
                newTile.setObjectTextureName("terrain1");
                newTile.setObjectTextureIndex(7);
            }
            else {
            //if (j < 5) {
                newTile.setTextureName("terrain1");
                newTile.setTextureIndex(texIndex);
                texIndex++;
            }
            //}
            /*}
            else {
                newTile.setTextureIndex(1);
            }*/
            col.emplace_back(newTile);
        }
        tiles.emplace_back(col);
    }
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
    if (tiles.size() == 0) {
        return 0;
    } 
    else {
        return tiles[0].size();
    }
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
    textures.emplace_back(Texture(textureInfo));
    return true;
}
