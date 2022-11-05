#include "mainController.hpp"
#include "manageMonsterStoreController.hpp"
#include "monsterStore.hpp"
#include "monsterStoreStorage.hpp"
#include "specialFolders.hpp"
#include "types.hpp"
#include <exception>
#include <fmt/format.h>
#include <algorithm>
#include <libgen.h>         // dirname
#include <linux/limits.h>   // PATH_MAX
#include <memory>
#include <unistd.h>         // readlink

using namespace std;
using namespace thewarrior::models;
using namespace thewarrior::storage;

namespace mapeditor::controllers {

MainController::MainController()
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

Point<> MainController::getCoordFromTileIndex(int index)
{
    return m_map->getCoordFromTileIndex(index);
}

const std::vector<Texture>& MainController::getTextures() const
{
    return m_map->getTextures();
}

const std::string &MainController::getExecutablePath() const
{
    return m_executablePath;
}

const std::string &MainController::getResourcesPath() const
{
    return m_resourcesPath;
}

const std::string &MainController::getUserConfigFolder() const
{
    return m_userConfigFolder;
}

const std::shared_ptr<ContainerOfMonsterStore> &MainController::getMonsterStores() const
{
    return m_monsterStores;
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

void MainController::initializeExecutablePath()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        m_executablePath = dirname(result);
    }
    else {
        m_executablePath = "";
    }
}

void MainController::initializeResourcesPath()
{
    m_resourcesPath = fmt::format("{0}/resources/", m_executablePath);
}

void MainController::initializeUserConfigFolder()
{
    m_userConfigFolder = SpecialFolders::getAppConfigDirectory("TheWarrior_MapEditor");
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

bool MainController::loadConfiguredMonsterStores()
{
    if (m_monsterStores == nullptr) {
        m_monsterStores = make_shared<ContainerOfMonsterStore>();
    }
    ManageMonsterStoreController manageMonsterStoreController(m_resourcesPath, m_userConfigFolder);
    if (!manageMonsterStoreController.loadMonsterStore()) {
        m_lastError = fmt::format("Unable to load configured monster store list. {0}",
                                  manageMonsterStoreController.getLastError());
        return false;
    }
    m_monsterStores->clear();
    for(const auto &monsterStoreInfo : manageMonsterStoreController.getMonsterStores()) {
        MonsterStoreStorage storage;
        std::shared_ptr<MonsterStore> store = std::make_shared<MonsterStore>();
        try {
            storage.loadMonsterStore(fmt::format("{0}/monsters/{1}", m_resourcesPath, monsterStoreInfo.filename), store);
            m_monsterStores->insert({ monsterStoreInfo.name, store });
        }
        catch(const std::exception &err) {
            m_lastError = err.what();
            return false;
        }
    }
    return true;
}

} // namespace mapeditor::controllers
