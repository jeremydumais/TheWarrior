#pragma once

#include <memory>
#include <string>
#include <vector>
#include <boost/serialization/access.hpp>
#include "gameMap.hpp"
#include "monsterStore.hpp"
#include "monsterZoneDTO.hpp"
#include "point.hpp"
#include "textureDTO.hpp"
#include "types.hpp"

namespace mapeditor::controllers {

class MainController {
 public:
    MainController();
    const std::string &getLastError() const;
    std::shared_ptr<thewarrior::models::GameMap> getMap();
    thewarrior::models::MapTile &getTileForEditing(int index);
    thewarrior::models::Point<> getCoordFromTileIndex(int index);
    const std::vector<thewarrior::models::Texture> &getTextures() const;
    const std::string &getExecutablePath() const;
    const std::string &getResourcesPath() const;
    const std::string &getUserConfigFolder() const;
    const std::shared_ptr<ContainerOfMonsterStore> &getMonsterStores() const;
    bool createMap(unsigned int width, unsigned int height);
    void initializeExecutablePath();
    void initializeResourcesPath();
    void initializeUserConfigFolder();
    bool addTexture(const commoneditor::ui::TextureDTO &textureDTO);
    bool replaceTexture(const std::string &name, const commoneditor::ui::TextureDTO &textureInfo);
    bool removeTexture(const std::string &name);
    void replaceTilesTextureName(const std::string &oldName, const std::string &newName);
    bool addMonsterZone(const MonsterZoneDTO &monsterZoneDTO);
    bool replaceMonsterZone(const std::string &name, const MonsterZoneDTO &monsterZoneDTO);
    bool removeMonsterZone(const std::string &name);
    bool loadConfiguredMonsterStores();

 private:
    friend class boost::serialization::access;
    std::string m_lastError = "";
    std::string m_executablePath = "";
    std::string m_resourcesPath = "";
    std::string m_userConfigFolder = "";
    std::shared_ptr<thewarrior::models::GameMap> m_map = nullptr;
    std::shared_ptr<ContainerOfMonsterStore> m_monsterStores = nullptr;
};

}  // namespace mapeditor::controllers
