#include "mainController.hpp"
#include <fmt/format.h>
#include <linux/limits.h>   // PATH_MAX
#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <algorithm>
#include <exception>
#include <memory>
#include "manageMonsterStoreController.hpp"
#include "monsterStore.hpp"
#include "monsterStoreStorage.hpp"
#include "monsterZoneDTOUtils.hpp"
#include "specialFolders.hpp"
#include "textureDTO.hpp"
#include "textureUtils.hpp"
#include "types.hpp"

using commoneditor::ui::TextureDTO;
using commoneditor::ui::TextureUtils;
using thewarrior::models::GameMap;
using thewarrior::models::MapTile;
using thewarrior::models::MonsterStore;
using thewarrior::models::Point;
using thewarrior::models::Texture;
using thewarrior::storage::MonsterStoreStorage;
using thewarrior::storage::SpecialFolders;

namespace mapeditor::controllers {

MainController::MainController() {
}

const std::string &MainController::getLastError() const {
    return this->m_lastError;
}

std::shared_ptr<GameMap> MainController::getMap() {
    return m_map;
}

MapTile& MainController::getTileForEditing(int index) {
    return m_map->getTileForEditing(index);
}

Point<> MainController::getCoordFromTileIndex(int index) {
    return m_map->getCoordFromTileIndex(index);
}

const std::vector<Texture>& MainController::getTextures() const {
    return m_map->getTextures();
}

const std::string &MainController::getExecutablePath() const {
    return m_executablePath;
}

const std::string &MainController::getResourcesPath() const {
    return m_resourcesPath;
}

const std::string &MainController::getUserConfigFolder() const {
    return m_userConfigFolder;
}

const std::shared_ptr<ContainerOfMonsterStore> &MainController::getMonsterStores() const {
    return m_monsterStores;
}

bool MainController::createMap(unsigned int width, unsigned int height) {
    try {
        m_map = std::make_shared<GameMap>(width, height);
    }
    catch(std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    return true;
}

void MainController::initializeExecutablePath() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        m_executablePath = dirname(result);
    } else {
        m_executablePath = "";
    }
}

void MainController::initializeResourcesPath() {
    m_resourcesPath = fmt::format("{0}/resources/", m_executablePath);
}

void MainController::initializeUserConfigFolder() {
    m_userConfigFolder = SpecialFolders::getAppConfigDirectory("TheWarrior_MapEditor");
}

bool MainController::addTexture(const TextureDTO &textureDTO) {
    if (!m_map->addTexture(TextureUtils::TextureDTOToTextureInfo(textureDTO))) {
        this->m_lastError = m_map->getLastError();
        return false;
    }
    return true;
}

bool MainController::replaceTexture(const std::string &name, const TextureDTO &textureDTO) {
    std::string oldTextureName { name };
    if (!m_map->replaceTexture(name, TextureUtils::TextureDTOToTextureInfo(textureDTO))) {
        m_lastError = m_map->getLastError();
        return false;
    }
    // If the texture name has changed, update all tiles that was using the old texture name
    if (oldTextureName != textureDTO.name) {
        replaceTilesTextureName(oldTextureName, textureDTO.name);
    }
    return true;
}

bool MainController::removeTexture(const std::string &name) {
    if (!m_map->removeTexture(name)) {
        m_lastError = m_map->getLastError();
        return false;
    }
    return true;
}

void MainController::replaceTilesTextureName(const std::string &oldName, const std::string &newName) {
    for (int index = 0; index < static_cast<int>(m_map->getWidth() * m_map->getHeight()) - 1; index++) {
        auto &tile = m_map->getTileForEditing(index);
        if (tile.getTextureName() == oldName) {
            tile.setTextureName(newName);
        }
        if (tile.getObjectTextureName() == oldName) {
            tile.setObjectTextureName(newName);
        }
    }
}

bool MainController::addMonsterZone(const MonsterZoneDTO &monsterZoneDTO) {
    try {
        if (!m_map->addMonsterZone(MonsterZoneDTOUtils::toMonsterZone(monsterZoneDTO))) {
            this->m_lastError = m_map->getLastError();
            return false;
        }
    } catch(const std::invalid_argument &err) {
        this->m_lastError = err.what();
        return false;
    }
    return true;
}

bool MainController::replaceMonsterZone(const std::string &name, const MonsterZoneDTO &monsterZoneDTO) {
    try {
        if (!m_map->replaceMonsterZone(name, MonsterZoneDTOUtils::toMonsterZone(monsterZoneDTO))) {
            this->m_lastError = m_map->getLastError();
            return false;
        }
    } catch(const std::invalid_argument &err) {
        this->m_lastError = err.what();
        return false;
    }
    return true;
}

bool MainController::removeMonsterZone(const std::string &name) {
    if (!m_map->removeMonsterZone(name)) {
        m_lastError = m_map->getLastError();
        return false;
    }
    return true;
}

bool MainController::loadConfiguredMonsterStores() {
    if (m_monsterStores == nullptr) {
        m_monsterStores = std::make_shared<ContainerOfMonsterStore>();
    }
    ManageMonsterStoreController manageMonsterStoreController(m_resourcesPath, m_userConfigFolder);
    if (!manageMonsterStoreController.loadMonsterStore()) {
        m_lastError = fmt::format("Unable to load configured monster store list. {0}",
                                  manageMonsterStoreController.getLastError());
        return false;
    }
    m_monsterStores->clear();
    for (const auto &monsterStoreInfo : manageMonsterStoreController.getMonsterStores()) {
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

}  // namespace mapeditor::controllers
