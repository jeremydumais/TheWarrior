#include "mainController.hpp"
#include <algorithm>

using namespace std;

MainController::MainController() 
    : lastError(""),
	  map(nullptr)
{
}

const string& MainController::getLastError() const
{
	return lastError;
}

shared_ptr<GameMap> MainController::getMap() 
{
    return map;
}

vector<string> MainController::getAlreadyUsedTextureNames() const
{
	vector<string> alreadyUsedTextureNames;
	if (map != nullptr) {
    	transform(map->getTextures().begin(), map->getTextures().end(), back_inserter(alreadyUsedTextureNames),
                  [](Texture const& x) { return x.getName(); });
	}
	return alreadyUsedTextureNames;
}

int MainController::getTextureIndexFromPosition(const Point &pos, const Texture &texture) 
{
	int realY { texture.getHeight() - pos.y()};
	int indexX = pos.x() / texture.getTileWidth();
	int indexY = realY / texture.getTileHeight();
	int tileIndex { indexX + (indexY * (texture.getWidth()/texture.getTileWidth())) };
	return tileIndex;
}

bool MainController::createMap(unsigned int width, unsigned int height) 
{
	try {
		map = make_shared<GameMap>(width, height);
	}
	catch(invalid_argument &err) {
		lastError = err.what();
		return false;
	}
	return true;
}

const std::vector<Texture>& MainController::getTextures() const
{
	return map->getTextures();
}

bool MainController::addTexture(const TextureInfo &textureInfo) 
{
	if (!map->addTexture(textureInfo)) {
		lastError = map->getLastError();
		return false;
	}
	return true;
}

bool MainController::replaceTexture(const string &name, const TextureInfo &textureInfo) 
{
    string oldTextureName { name };
	if (!map->replaceTexture(name, textureInfo)) {
		lastError = map->getLastError();
		return false;
	}
	//If the texture name has changed, update all tiles that was using the old texture name	
    if (oldTextureName != textureInfo.name) {
        replaceTilesTextureName(oldTextureName, textureInfo.name);
    }
	return true;
}

bool MainController::removeTexture(const string &name) 
{
	if (!map->removeTexture(name)) {
		lastError = map->getLastError();
		return false;
	}
	return true;
}

bool MainController::isTextureUsedInMap(const string &name) 
{
	for(const auto &row : map->getTiles()) {
		for (const auto &tile : row) {
			if (tile.getTextureName() == name || tile.getObjectTextureName() == name) {
				return true;
			}
		}
	}
	return false;
}

bool MainController::isShrinkMapImpactAssignedTiles(int offsetLeft, 
													int offsetTop, 
													int offsetRight, 
													int offsetBottom) const
{
	return map->isShrinkMapImpactAssignedTiles(offsetLeft,
											   offsetTop,
											   offsetRight,
											   offsetBottom);
}

void MainController::resizeMap(int offsetLeft, 
                   int offsetTop, 
                   int offsetRight, 
                   int offsetBottom) 
{
	map->resizeMap(offsetLeft,
				   offsetTop,
				   offsetRight,
				   offsetBottom);
}

void MainController::replaceTilesTextureName(const string &oldName, const string &newName) 
{
	for(int index = 0; index < static_cast<int>(map->getWidth() * map->getHeight()) - 1; index++) {
		auto &tile = map->getTileForEditing(index);
		if (tile.getTextureName() == oldName) {
			tile.setTextureName(newName);
		}
		if (tile.getObjectTextureName() == oldName) {
			tile.setObjectTextureName(newName);
		}
	}
}