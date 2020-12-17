#include "gameMap.hpp"
#include <stdexcept>

using namespace std;

GameMap::GameMap(unsigned int width, unsigned int height) 
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
            col.emplace_back(MapTile());
        }
        tiles.emplace_back(col);
    }
}

const std::vector<std::vector<MapTile>>& GameMap::getTiles() const
{
    return tiles;
}

unsigned int GameMap::getWidth() const
{
    if (tiles.size() == 0) {
        return 0;
    } 
    else {
        return tiles[0].size();
    }
    
}

unsigned int GameMap::getHeight() const
{
    return tiles.size();
}

void GameMap::addTexture(const std::string &name, const string &filename) 
{
    //Check that name doesn't already exist in the list
    textures.emplace_back(Texture(name, filename));
}
