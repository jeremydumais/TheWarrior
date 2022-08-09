#include "mainController.hpp"
#include <algorithm>

using namespace std;
using namespace thewarrior::models;

namespace mapeditor::controllers {

MainController::MainController()
    : m_lastError(""),
    m_map(nullptr)
{
}

const string &MainController::getLastError() const
{
    return this->m_lastError;
}

shared_ptr<GameMap> MainController::getMap()
{
    return m_map;
}

MapTile& MainController::getTileForEditing(int index)
{
    return m_map->getTileForEditing(index);
}


bool MainController::createMap(unsigned int width, unsigned int height)
{
    try {
        m_map = make_shared<GameMap>(width, height);
    }
    catch(invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    return true;
}

Point<> MainController::getCoordFromTileIndex(int index)
{
    return m_map->getCoordFromTileIndex(index);
}

const std::vector<Texture>& MainController::getTextures() const
{
    return m_map->getTextures();
}

bool MainController::addTexture(const TextureInfo &textureInfo)
{
    if (!m_map->addTexture(textureInfo)) {
        this->m_lastError = m_map->getLastError();
        return false;
    }
    return true;
}

bool MainController::replaceTexture(const string &name, const TextureInfo &textureInfo)
{
    string oldTextureName { name };
    if (!m_map->replaceTexture(name, textureInfo)) {
        m_lastError = m_map->getLastError();
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
    if (!m_map->removeTexture(name)) {
        m_lastError = m_map->getLastError();
        return false;
    }
    return true;
}

void MainController::replaceTilesTextureName(const string &oldName, const string &newName)
{
    for(int index = 0; index < static_cast<int>(m_map->getWidth() * m_map->getHeight()) - 1; index++) {
        auto &tile = m_map->getTileForEditing(index);
        if (tile.getTextureName() == oldName) {
            tile.setTextureName(newName);
        }
        if (tile.getObjectTextureName() == oldName) {
            tile.setObjectTextureName(newName);
        }
    }
}

} // namespace mapeditor::controllers
