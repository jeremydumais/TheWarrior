#include "gameMap.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <stdexcept>

using namespace std;

GameMap::GameMap(unsigned int width, unsigned int height)
    : lastError("")
{
    if (width == 0) {
        throw invalid_argument("width must be greater than zero.");
    }
    if (height == 0) {
        throw invalid_argument("height must be greater than zero.");
    }
    for(unsigned int i = 0; i < height; i++) {
        vector<MapTile> row;
        for(unsigned int j = 0; j < width; j++) {
            MapTile newTile;
            row.emplace_back(newTile);
        }
        tiles.emplace_back(row);
    }
}

const std::string &GameMap::getLastError() const
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

MapTile& GameMap::getTileForEditing(Point coord) 
{
    return tiles.at(coord.y()).at(coord.x());
}

const MapTile& GameMap::getTileFromCoord(Point coord) const
{
    return tiles.at(coord.y()).at(coord.x());
}

unsigned int GameMap::getWidth() const
{
    return tiles[0].size();    
}

unsigned int GameMap::getHeight() const
{
    return tiles.size();
}

Point GameMap::getCoordFromTileIndex(int index) 
{
    int x = index % getWidth();
    int y = index / getWidth();
    return Point(x, y);
}

const vector<Texture> &GameMap::getTextures() const
{
    return textureContainer.getTextures();
}
    
boost::optional<const Texture &> GameMap::getTextureByName(const string &name) const
{
    return textureContainer.getTextureByName(name);
}

bool GameMap::addTexture(const TextureInfo &textureInfo)
{
    bool retVal = textureContainer.addTexture(textureInfo);
    if (!retVal) {
        lastError = textureContainer.getLastError();
    }
    return retVal;
}

bool GameMap::replaceTexture(const std::string &name, const TextureInfo &textureInfo)
{
    bool retVal = textureContainer.replaceTexture(name, textureInfo);
    if (!retVal) {
        lastError = textureContainer.getLastError();
    }
    return retVal;
}

bool GameMap::removeTexture(const std::string &name)
{
    bool retVal = textureContainer.removeTexture(name);
    if (!retVal) {
        lastError = textureContainer.getLastError();
    }
    return retVal;
}

bool GameMap::isShrinkMapImpactAssignedTiles(int offsetLeft, 
                                             int offsetTop, 
                                             int offsetRight, 
                                             int offsetBottom) const
{
    return (_isShrinkMapFromLeftImpactAssignedTiles(offsetLeft) ||
            _isShrinkMapFromTopImpactAssignedTiles(offsetTop) ||
            _isShrinkMapFromRightImpactAssignedTiles(offsetRight) ||
            _isShrinkMapFromBottomImpactAssignedTiles(offsetBottom));
}

bool GameMap::_isShrinkMapFromLeftImpactAssignedTiles(int offset) const 
{
    if (offset < 0) {
		size_t col { 0 };
		for(int index = offset; index < 0; index++) {
			//Check all tiles of the column
			for(const auto &row : tiles) {
				if (row[col].isAssigned()) {
					return true;
				}
			}
			col++;
		}
	}
    return false;
}

bool GameMap::_isShrinkMapFromTopImpactAssignedTiles(int offset) const
{
    if (offset < 0 ) {
		size_t rowIndex { 0 };
		for(int index = offset; index < 0; index++) {
			//Check all tiles of the column
			for(const auto &row : tiles[rowIndex]) {
				if (row.isAssigned()) {
					return true;
				}
			}
			rowIndex++;
		}
	}
    return false;
}

bool GameMap::_isShrinkMapFromRightImpactAssignedTiles(int offset) const
{
    if (offset < 0) {
		size_t col { tiles[0].size() - 1 };
		for(int index = offset; index < 0; index++) {
			//Check all tiles of the column
			for(const auto &row : tiles) {
				if (row[col].isAssigned()) {
					return true;
				}
			}
			col--;
		}
	}
    return false;
}

bool GameMap::_isShrinkMapFromBottomImpactAssignedTiles(int offset) const
{
    if (offset < 0 ) {
		size_t rowIndex { tiles.size() - 1 };
		for(int index = offset; index < 0; index++) {
			//Check all tiles of the column
			for(const auto &row : tiles[rowIndex]) {
				if (row.isAssigned()) {
					return true;
				}
			}
			rowIndex--;
		}
	}
    return false;
}

void GameMap::resizeMap(int offsetLeft, 
                   int offsetTop, 
                   int offsetRight, 
                   int offsetBottom) 
{
    if (offsetLeft < 0 && abs(offsetLeft) >= static_cast<long long>(getWidth())) {
        throw invalid_argument("This left offset would remove all the remaining tiles.");
    }
    if (offsetTop < 0 && abs(offsetTop) >= static_cast<long long>(getHeight())) {
        throw invalid_argument("This top offset would remove all the remaining tiles.");
    }
    if (offsetRight < 0 && abs(offsetRight) >= static_cast<long long>(getWidth())) {
        throw invalid_argument("This right offset would remove all the remaining tiles.");
    }
    if (offsetBottom < 0 && abs(offsetBottom) >= static_cast<long long>(getHeight())) {
        throw invalid_argument("This bottom offset would remove all the remaining tiles.");
    }
    _resizeMapFromLeft(offsetLeft);
    _resizeMapFromTop(offsetTop);
    _resizeMapFromRight(offsetRight);
    _resizeMapFromBottom(offsetBottom);
}

void GameMap::_resizeMapFromLeft(int offset) 
{
    if (offset < 0) {
        for(auto &row : tiles) {
            for(int index=offset; index<0; index++) {
                row.erase(row.begin());
            }
        }
    }
    else if (offset > 0) {
        for(auto &row : tiles) {
            for(int index=0; index<offset; index++) {
                row.insert(row.begin(), MapTile());
            }
        }
    }
}

void GameMap::_resizeMapFromTop(int offset) 
{
    if (offset < 0) {
        for(int index=offset; index<0; index++) {
            tiles.erase(tiles.begin());
        }
    }
    else if (offset > 0) {
        for(int index=0; index<offset; index++) {
            vector<MapTile> newRow(getWidth());
            tiles.insert(tiles.begin(), newRow);
        }
    }
}

void GameMap::_resizeMapFromRight(int offset) 
{
    if (offset < 0) {
        for(auto &row : tiles) {
            for(int index=offset; index<0; index++) {
                row.erase(row.end() - 1);
            }
        }
    }
    else if (offset > 0) {
        for(auto &row : tiles) {
            for(int index=0; index<offset; index++) {
                row.insert(row.end(), MapTile());
            }
        }
    }
}

void GameMap::_resizeMapFromBottom(int offset) 
{
    if (offset < 0) {
        for(int index=offset; index<0; index++) {
            tiles.erase(tiles.end() - 1);
        }
    }
    else if (offset > 0) {
        for(int index=0; index<offset; index++) {
            vector<MapTile> newRow(getWidth());
            tiles.insert(tiles.end(), newRow);
        }
    }
}

/*std::vector<Texture>::iterator GameMap::_getTextureIterator(const std::string &name) 
{
    return find_if(textures.begin(), textures.end(), [&name](const Texture &x) {
        return x.getName() == name;
    });
}*/

bool GameMap::canSteppedOnTile(Point playerCoord) 
{
    return (playerCoord.x() < static_cast<int>(getWidth()) &&
            playerCoord.x() >= 0 &&
            playerCoord.y() < static_cast<int>(getHeight()) &&
            playerCoord.y() >= 0 &&
            getTileFromCoord(playerCoord).canPlayerSteppedOn());
}