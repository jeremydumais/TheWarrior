#pragma once

#include "gameMap.hpp"
#include "point.hpp"
#include <boost/serialization/access.hpp>
#include <memory>
#include <string>
#include <vector>

class MainController
{
public:
    MainController();
    const std::string &getLastError() const;
    std::shared_ptr<GameMap> getMap();
    MapTile &getTileForEditing(int index);
    bool createMap(unsigned int width, unsigned int height);
    Point<> getCoordFromTileIndex(int index);
    const std::vector<Texture> &getTextures() const;
    bool addTexture(const TextureInfo &textureInfo);
    bool replaceTexture(const std::string &name, const TextureInfo &textureInfo);
    bool removeTexture(const std::string &name);
    void replaceTilesTextureName(const std::string &oldName, const std::string &newName);
private:
    friend class boost::serialization::access;
    std::string m_lastError;
	std::shared_ptr<GameMap> m_map;
};