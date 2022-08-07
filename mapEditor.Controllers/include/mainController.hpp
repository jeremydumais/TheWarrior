#pragma once

#include "gameMap.hpp"
#include "point.hpp"
#include <boost/serialization/access.hpp>
#include <memory>
#include <string>
#include <vector>

namespace mapeditor::controllers {

class MainController
{
    public:
        MainController();
        const std::string &getLastError() const;
        std::shared_ptr<thewarrior::models::GameMap> getMap();
        thewarrior::models::MapTile &getTileForEditing(int index);
        bool createMap(unsigned int width, unsigned int height);
        thewarrior::models::Point<> getCoordFromTileIndex(int index);
        const std::vector<thewarrior::models::Texture> &getTextures() const;
        bool addTexture(const thewarrior::models::TextureInfo &textureInfo);
        bool replaceTexture(const std::string &name, const thewarrior::models::TextureInfo &textureInfo);
        bool removeTexture(const std::string &name);
        void replaceTilesTextureName(const std::string &oldName, const std::string &newName);
    private:
        friend class boost::serialization::access;
        std::string m_lastError;
        std::shared_ptr<thewarrior::models::GameMap> m_map;
};

} // namespace mapeditor::controllers
