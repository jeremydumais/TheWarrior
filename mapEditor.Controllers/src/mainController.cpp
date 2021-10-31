#include "mainController.hpp"
#include <algorithm>

using namespace std;

MainController::MainController() 
    : lastError(""),
	  map(nullptr)
{
}

const string &MainController::getLastError() const
{
	return this->lastError;
}

shared_ptr<GameMap> MainController::getMap() 
{
    return map;
}

MapTile& MainController::getTileForEditing(int index) 
{
	return map->getTileForEditing(index);
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

Point MainController::getCoordFromTileIndex(int index) 
{
	return map->getCoordFromTileIndex(index);
}

const std::vector<Texture>& MainController::getTextures() const
{
	return map->getTextures();
}

bool MainController::addTexture(const TextureInfo &textureInfo) 
{
	if (!map->addTexture(textureInfo)) {
		this->lastError = map->getLastError();
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