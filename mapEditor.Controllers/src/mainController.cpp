#include "mainController.hpp"
#include <algorithm>

using namespace std;

MainController::MainController() 
    : map(nullptr)
{
}


void MainController::createMap(unsigned int width, unsigned int height) 
{
	map = make_shared<GameMap>(width, height);
}

std::shared_ptr<GameMap> MainController::getMap() 
{
    return map;
}

std::vector<std::string> MainController::getAlreadyUsedTextureNames() const
{
	vector<string> alreadyUsedTextureNames;
	if (map != nullptr) {
    	transform(map->getTextures().begin(), map->getTextures().end(), std::back_inserter(alreadyUsedTextureNames),
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

void MainController::generateTestMap() 
{
    map = make_shared<GameMap>(20, 20);
	TextureInfo textureInfo {
		"terrain1", "tile.png",
		256, 4256,
		32, 32
	};
	map->addTexture(textureInfo);
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