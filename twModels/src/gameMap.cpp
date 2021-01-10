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
    for(unsigned int i = 0; i < height; i++) {
        vector<MapTile> row;
        for(unsigned int j = 0; j < width; j++) {
            MapTile newTile;
            row.emplace_back(newTile);
        }
        tiles.emplace_back(row);
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
    return tiles[0].size();    
}

unsigned int GameMap::getHeight() const
{
    return tiles.size();
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
    auto iter { _getTextureIterator(name) };
    if (iter == textures.end()) {
        lastError = fmt::format("Unable to find the texture {0} in the texture list.", name);
        return false;
    }
    //Ensure the new name doesn't exist
    if (name != textureInfo.name && _getTextureIterator(textureInfo.name) != textures.end()) {
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
    auto iter { _getTextureIterator(name) };
    if (iter == textures.end()) {
        lastError = fmt::format("Unable to find the texture {0} in the texture list.", name);
        return false;
    }
    textures.erase(iter);
    return true;
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
    if (offsetLeft < 0 && abs(offsetLeft) >= getWidth()) {
        throw invalid_argument("This left offset would remove all the remaining tiles.");
    }
    if (offsetTop < 0 && abs(offsetTop) >= getHeight()) {
        throw invalid_argument("This top offset would remove all the remaining tiles.");
    }
    if (offsetRight < 0 && abs(offsetRight) >= getWidth()) {
        throw invalid_argument("This right offset would remove all the remaining tiles.");
    }
    if (offsetBottom < 0 && abs(offsetBottom) >= getHeight()) {
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

std::vector<Texture>::iterator GameMap::_getTextureIterator(const std::string &name) 
{
    return find_if(textures.begin(), textures.end(), [&name](const Texture &x) {
        return x.getName() == name;
    });
}

bool GameMap::canSteppedOnTile(int playerX, int playerY) 
{
    return (playerX < getWidth() &&
            playerX >= 0 &&
            playerY < getHeight() &&
            playerY >= 0 &&
            tiles[playerY][playerX].canPlayerSteppedOn());
}