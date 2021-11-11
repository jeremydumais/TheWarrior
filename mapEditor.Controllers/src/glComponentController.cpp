#include "glComponentController.hpp"
#include <algorithm>

using namespace std;

GLComponentController::GLComponentController() 
    : map(nullptr)
{
}

const std::shared_ptr<GameMap> GLComponentController::getMap() const
{
    return this->map;
}

void GLComponentController::setCurrentMap(std::shared_ptr<GameMap> map) 
{
    this->map = map;
}

vector<string> GLComponentController::getAlreadyUsedTextureNames() const
{
	vector<string> alreadyUsedTextureNames;
	if (map != nullptr) {
    	transform(map->getTextures().begin(), map->getTextures().end(), back_inserter(alreadyUsedTextureNames),
                  [](Texture const& x) { return x.getName(); });
	}
	return alreadyUsedTextureNames;
}

bool GLComponentController::isTextureUsedInMap(const std::string &name) 
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

bool GLComponentController::isShrinkMapImpactAssignedTiles(int offsetLeft, 
													      int offsetTop, 
														  int offsetRight, 
														  int offsetBottom) const
{
	return map->isShrinkMapImpactAssignedTiles(offsetLeft,
											   offsetTop,
											   offsetRight,
											   offsetBottom);
}

void GLComponentController::resizeMap(int offsetLeft, 
                   int offsetTop, 
                   int offsetRight, 
                   int offsetBottom) 
{
	map->resizeMap(offsetLeft,
				   offsetTop,
				   offsetRight,
				   offsetBottom);
}

