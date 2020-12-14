#include "map.hpp"

using namespace std;

GameMap::GameMap(unsigned int width, unsigned int height) 
{

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
