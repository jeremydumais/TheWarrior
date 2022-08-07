#include "glComponentController.hpp"
#include <algorithm>

using namespace std;
using namespace thewarrior::models;

namespace mapeditor::controllers {

GLComponentController::GLComponentController()
    : m_map(nullptr)
{
}

const std::shared_ptr<GameMap> GLComponentController::getMap() const
{
    return this->m_map;
}

void GLComponentController::setCurrentMap(std::shared_ptr<GameMap> map)
{
    this->m_map = map;
}

vector<string> GLComponentController::getAlreadyUsedTextureNames() const
{
	vector<string> alreadyUsedTextureNames;
	if (m_map != nullptr) {
    	transform(m_map->getTextures().begin(), m_map->getTextures().end(), back_inserter(alreadyUsedTextureNames),
                  [](Texture const& x) { return x.getName(); });
	}
	return alreadyUsedTextureNames;
}

bool GLComponentController::isTextureUsedInMap(const std::string &name)
{
	for(const auto &row : m_map->getTiles()) {
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
	return m_map->isShrinkMapImpactAssignedTiles(offsetLeft,
											   offsetTop,
											   offsetRight,
											   offsetBottom);
}

void GLComponentController::resizeMap(int offsetLeft,
                   int offsetTop,
                   int offsetRight,
                   int offsetBottom)
{
	m_map->resizeMap(offsetLeft,
				   offsetTop,
				   offsetRight,
				   offsetBottom);
}

} // namespace mapeditor::controllers
