#include "gameMap.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <limits>
#include <stdexcept>

using namespace std;

GameMap::GameMap(unsigned int width, unsigned int height)
    : m_lastError("")
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
        m_tiles.emplace_back(row);
    }
}

const std::string &GameMap::getLastError() const
{
    return m_lastError;
}

const std::vector<std::vector<MapTile>>& GameMap::getTiles() const
{
    return m_tiles;
}

MapTile& GameMap::getTileForEditing(int index)
{
    if (index < 0) {
        throw invalid_argument("index must be a positive number");
    }
    auto indexConverted { static_cast<size_t>(index) };
    return m_tiles.at(indexConverted / getWidth()).at(indexConverted % getWidth());
}

MapTile& GameMap::getTileForEditing(Point<> coord) 
{
    if (coord.x() < 0) {
        throw invalid_argument("x must be a positive number");
    }
    if (coord.y() < 0) {
        throw invalid_argument("y must be a positive number");
    }
    return m_tiles.at(static_cast<size_t>(coord.y()))
                .at(static_cast<size_t>(coord.x()));
}

const MapTile& GameMap::getTileFromCoord(Point<> coord) const
{
    if (coord.x() < 0) {
        throw invalid_argument("x must be a positive number");
    }
    if (coord.y() < 0) {
        throw invalid_argument("y must be a positive number");
    }
    return m_tiles.at(static_cast<size_t>(coord.y()))
                .at(static_cast<size_t>(coord.x()));
}

unsigned int GameMap::getWidth() const
{
    auto widthSize = m_tiles[0].size();
    if (widthSize > UINT_MAX) {
        return UINT_MAX;
    }
    return static_cast<unsigned int>(widthSize);    
}

unsigned int GameMap::getHeight() const
{
    auto heightSize = m_tiles.size();
    if (heightSize > UINT_MAX) {
        return UINT_MAX;
    }
    return static_cast<unsigned int>(heightSize);
}

Point<> GameMap::getCoordFromTileIndex(int index) 
{
    if (index < 0) {
        throw invalid_argument("index must be a positive number");
    }
    auto indexConverted { static_cast<unsigned int>(index) };
    int x = static_cast<int>(indexConverted % getWidth());
    int y = static_cast<int>(indexConverted / getWidth());
    return Point(x, y);
}

const vector<Texture> &GameMap::getTextures() const
{
    return m_textureContainer.getTextures();
}
    
optional<reference_wrapper<const Texture>> GameMap::getTextureByName(const string &name) const
{
    return m_textureContainer.getTextureByName(name);
}

bool GameMap::addTexture(const TextureInfo &textureInfo)
{
    bool retVal = m_textureContainer.addTexture(textureInfo);
    if (!retVal) {
        m_lastError = m_textureContainer.getLastError();
    }
    return retVal;
}

bool GameMap::replaceTexture(const std::string &name, const TextureInfo &textureInfo)
{
    bool retVal = m_textureContainer.replaceTexture(name, textureInfo);
    if (!retVal) {
        m_lastError = m_textureContainer.getLastError();
    }
    return retVal;
}

bool GameMap::removeTexture(const std::string &name)
{
    bool retVal = m_textureContainer.removeTexture(name);
    if (!retVal) {
        m_lastError = m_textureContainer.getLastError();
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
			for(const auto &row : m_tiles) {
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
			for(const auto &row : m_tiles[rowIndex]) {
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
		size_t col { m_tiles[0].size() - 1 };
		for(int index = offset; index < 0; index++) {
			//Check all tiles of the column
			for(const auto &row : m_tiles) {
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
		size_t rowIndex { m_tiles.size() - 1 };
		for(int index = offset; index < 0; index++) {
			//Check all tiles of the column
			for(const auto &row : m_tiles[rowIndex]) {
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
        for(auto &row : m_tiles) {
            for(int index=offset; index<0; index++) {
                row.erase(row.begin());
            }
        }
    }
    else if (offset > 0) {
        for(auto &row : m_tiles) {
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
            m_tiles.erase(m_tiles.begin());
        }
    }
    else if (offset > 0) {
        for(int index=0; index<offset; index++) {
            vector<MapTile> newRow(getWidth());
            m_tiles.insert(m_tiles.begin(), newRow);
        }
    }
}

void GameMap::_resizeMapFromRight(int offset) 
{
    if (offset < 0) {
        for(auto &row : m_tiles) {
            for(int index=offset; index<0; index++) {
                row.erase(row.end() - 1);
            }
        }
    }
    else if (offset > 0) {
        for(auto &row : m_tiles) {
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
            m_tiles.erase(m_tiles.end() - 1);
        }
    }
    else if (offset > 0) {
        for(int index=0; index<offset; index++) {
            vector<MapTile> newRow(getWidth());
            m_tiles.insert(m_tiles.end(), newRow);
        }
    }
}

bool GameMap::canSteppedOnTile(Point<> playerCoord) 
{
    return (playerCoord.x() >= 0 && 
            static_cast<unsigned int>(playerCoord.x()) < getWidth() &&
            playerCoord.y() >= 0 &&
            static_cast<unsigned int>(playerCoord.y()) < getHeight() &&
            getTileFromCoord(playerCoord).canPlayerSteppedOn());
}