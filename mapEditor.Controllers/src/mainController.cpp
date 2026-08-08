#include <fmt/format.h>
#include <linux/limits.h>   // PATH_MAX
#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include "configurationManager.hpp"
#include "itemStore.hpp"
#include "itemStoreStorage.hpp"
#include "mainController.hpp"
#include "manageItemStoreController.hpp"
#include "manageMonsterStoreController.hpp"
#include "merchantInventoryDTO.hpp"
#include "monsterStore.hpp"
#include "monsterStoreStorage.hpp"
#include "specialFolders.hpp"
#include "textureDTO.hpp"
#include "types.hpp"

using commoneditor::ui::TextureDTO;
using thewarrior::models::GameMap;
using thewarrior::models::ItemStore;
using thewarrior::models::MonsterStore;
using thewarrior::models::Texture;
using thewarrior::storage::ConfigurationManager;
using thewarrior::storage::ItemStoreStorage;
using thewarrior::storage::MonsterStoreStorage;
using thewarrior::utils::SpecialFolders;

namespace mapeditor::controllers {

static constexpr const char DisplayThemeConfigItem[] = "Display.Theme";
static constexpr const char DisplayGridConfigItem[] = "Display.Grid";
static constexpr const char DisplayNPCsConfigItem[] = "Display.NPCs";
static constexpr const char DisplayEditorsMapConfigItem[] = "Display.Editors.MapConfiguration";
static constexpr const char DisplayEditorsTextureSelectionItem[] = "Display.Editors.TextureSelection";
static constexpr const char DisplayEditorsDebuggingInfoItem[] = "Display.Editors.DebuggingInfo";
static constexpr const char DisplayToolbarsMonsterZoneItem[] = "Display.Toolbars.MonsterZone";
static constexpr const char DisplayToolbarsNPCWanderingZoneItem[] = "Display.Toolbars.NPCWanderingZone";
static constexpr const char RecentMapsConfigItem[] = "Map.Recents";

MainController::MainController()
    : m_configFilename("config.json") {
}

const std::string &MainController::getLastError() const {
    return this->m_lastError;
}

std::shared_ptr<GameMap> MainController::getMap() const {
    return m_glComponentController->getMap();
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

const std::vector<Texture>& MainController::getTextures() const {
    return m_glComponentController->getMap()->getTextures();
}

const std::shared_ptr<ContainerOfMonsterStore> &MainController::getMonsterStores() const {
    return m_monsterStores;
}

const std::shared_ptr<ContainerOfItemStore> &MainController::getItemStores() const {
    return m_itemStores;
}

bool MainController::canDisableCanSteppedOnForSelectedTiles() const {
    return m_glComponentController->canDisableCanSteppedOnForSelectedTiles();
}

void MainController::setGLComponentController(GLComponentController *controller) {
    m_glComponentController = controller;
}

bool MainController::createMap(unsigned int width, unsigned int height) {
    try {
        auto map = std::make_shared<GameMap>(width, height);
        m_glComponentController->setCurrentMap(map);
    }
    catch(std::invalid_argument &err) {
        m_lastError = err.what();
        return false;
    }
    return true;
}

void MainController::saveMap(const std::string &filePath) {
    std::ofstream ofs(filePath, std::ofstream::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << *m_glComponentController->getMap();
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

void MainController::initializeUserConfigFolder(const std::string &organizationName,
                                                const std::string &applicationName) {
    m_userConfigFolder = SpecialFolders::getAppConfigDirectory(fmt::format("{0}/{1}",
                                                               organizationName,
                                                               applicationName));
}

bool MainController::loadConfigurationFile() {
    // Check if the user configuration folder exist
    if (!boost::filesystem::exists(m_userConfigFolder)) {
        if (!boost::filesystem::create_directories(m_userConfigFolder)) {
            m_lastError = fmt::format("Unable to create the folder {0}", m_userConfigFolder);
            return false;
        }
    }

    auto fullConfigFilePath = std::filesystem::path(m_userConfigFolder) / m_configFilename;
    try {
        m_configManager = std::make_unique<ConfigurationManager>(fullConfigFilePath);
    }
    catch (std::invalid_argument &err) {
        m_lastError = fmt::format("Config file {0} error: {1}",
                                  fullConfigFilePath.string(),
                                  err.what());
        return false;
    }
    if (!m_configManager->fileExists()) {
        // Try to create a default configuration
        if (!m_configManager->save()) {
            m_lastError = fmt::format("An error occurred while creation a default the configuration file. {0}",
                                      m_configManager->getLastError());
            return false;
        }
    }
    if (!m_configManager->load()) {
        m_lastError = m_configManager->getLastError();
        return false;
    }
    return true;
}

bool MainController::saveConfigurationFile() {
    if (!m_configManager->save()) {
        m_lastError = fmt::format("An error occurred while saving the configuration file : {0}",
                                  m_configManager->getLastError());
        return false;
    }
    return true;
}

bool MainController::addTexture(const TextureDTO &textureDTO) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->addTexture(textureDTO)) {
        this->m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;
}

bool MainController::replaceTexture(const std::string &name, const TextureDTO &textureDTO) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->replaceTexture(name, textureDTO)) {
        m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;
}

bool MainController::removeTexture(const std::string &name) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->removeTexture(name)) {
        m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;
}

bool MainController::addMonsterZone(const MonsterZoneDTO &monsterZoneDTO) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->addMonsterZone(monsterZoneDTO)) {
        this->m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;
}

bool MainController::replaceMonsterZone(const std::string &name, const MonsterZoneDTO &monsterZoneDTO) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->replaceMonsterZone(name, monsterZoneDTO)) {
        this->m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;
}

bool MainController::removeMonsterZone(const std::string &name) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->removeMonsterZone(name)) {
        m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;
}

bool MainController::loadConfiguredItemStores() {
    if (m_itemStores == nullptr) {
        m_itemStores = std::make_shared<ContainerOfItemStore>();
    }
    ManageItemStoreController manageItemStoreController(m_resourcesPath, m_userConfigFolder);
    if (!manageItemStoreController.loadItemStore()) {
        m_lastError = fmt::format("Unable to load configured item store list. {0}",
                                  manageItemStoreController.getLastError());
        return false;
    }
    m_itemStores->clear();
    for (const auto &itemStoreInfo : manageItemStoreController.getItemStores()) {
        ItemStoreStorage storage;
        std::shared_ptr<ItemStore> store = std::make_shared<ItemStore>();
        try {
            storage.loadItemStore(fmt::format("{0}/items/{1}", m_resourcesPath, itemStoreInfo.filename), store);
            m_itemStores->insert({ itemStoreInfo.name, store });
        }
        catch(const std::exception &err) {
            m_lastError = err.what();
            return false;
        }
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

bool MainController::addNPC(const NPCDTO &npcDTO) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->addNPC(npcDTO)) {
        this->m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;
}

bool MainController::replaceNPC(const std::string &id, const NPCDTO &npcDTO) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->replaceNPC(id, npcDTO)) {
        this->m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;
}

bool MainController::removeNPC(const std::string &id) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->removeNPC(id)) {
        m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;
}

bool MainController::addMerchantInventory(const MerchantInventoryDTO &merchantInventoryDTO) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->addMerchantInventory(merchantInventoryDTO)) {
        this->m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;
}

bool MainController::replaceMerchantInventory(const std::string &name, const MerchantInventoryDTO &merchantInventoryDTO) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->replaceMerchantInventory(name, merchantInventoryDTO)) {
        this->m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;    
}

bool MainController::removeMerchantInventory(const std::string &name) {
    m_glComponentController->pushCurrentStateToHistory();
    if (!m_glComponentController->removeMerchantInventory(name)) {
        this->m_lastError = m_glComponentController->getLastError();
        return false;
    }
    return true;   
}

std::vector<std::string> MainController::getRecentMapsFromConfig() const {
    return m_configManager->getVectorOfStringValue(RecentMapsConfigItem);
}

bool MainController::addNewRecentMap(const std::string &filePath) {
    auto recents = m_configManager->getVectorOfStringValue(RecentMapsConfigItem);
    // Scan to find the currentMap, if found remove it from the list
    auto iter = std::find(recents.begin(), recents.end(), filePath);
    if (iter != recents.end()) {
        recents.erase(iter);
    }
    // Add it at the beginning of the vector
    recents.insert(recents.begin(), filePath);
    if (recents.size() > 5) {
        recents.resize(5);
    }
    m_configManager->setVectorOfStringValue(RecentMapsConfigItem, recents);
    return saveConfigurationFile();
}

bool MainController::getDisplayGridConfigState() const {
    return m_configManager->getBoolValue(DisplayGridConfigItem, true);
}

bool MainController::setDisplayGridConfigState(bool value) {
    m_configManager->setBoolValue(DisplayGridConfigItem, value);
    return saveConfigurationFile();
}

bool MainController::getDisplayNPCsConfigState() const {
    return m_configManager->getBoolValue(DisplayNPCsConfigItem, true);
}

bool MainController::setDisplayNPCsConfigState(bool value) {
    m_configManager->setBoolValue(DisplayNPCsConfigItem, value);
    return saveConfigurationFile();
}

std::string MainController::getThemeConfigValue() const {
    return m_configManager->getStringValue(DisplayThemeConfigItem);
}

bool MainController::setThemeConfigValue(const std::string &theme) {
    m_configManager->setStringValue(DisplayThemeConfigItem, theme);
    return saveConfigurationFile();
}

bool MainController::getDisplayEditorsMapConfigState() const {
    auto mapConfigItem = std::string(DisplayEditorsMapConfigItem);
    return m_configManager->getBoolValue(mapConfigItem, true);
}

bool MainController::setDisplayEditorsMapConfigState(bool value) {
auto mapConfigItem = std::string(DisplayEditorsMapConfigItem);
m_configManager->setBoolValue(mapConfigItem, value);
return saveConfigurationFile();
}

bool MainController::getDisplayEditorsTextureSelectionState() const {
    auto textureSelectionItem = std::string(DisplayEditorsTextureSelectionItem);
    return m_configManager->getBoolValue(textureSelectionItem, true);
}

bool MainController::setDisplayEditorsTextureSelectionState(bool value) {
    auto textureSelectionItem = std::string(DisplayEditorsTextureSelectionItem);
    m_configManager->setBoolValue(textureSelectionItem, value);
    return saveConfigurationFile();
}

bool MainController::getDisplayEditorsDebuggingInfoState() const {
    auto debuggingInfoItem = std::string(DisplayEditorsDebuggingInfoItem);
    return m_configManager->getBoolValue(debuggingInfoItem, false);
}

bool MainController::setDisplayEditorsDebuggingInfoState(bool value) {
    auto debuggingInfoItem = std::string(DisplayEditorsDebuggingInfoItem);
    m_configManager->setBoolValue(debuggingInfoItem, value);
    return saveConfigurationFile();
}

bool MainController::getDisplayToolbarsMonsterZoneState() const {
    auto monsterZoneItem = std::string(DisplayToolbarsMonsterZoneItem);
    return m_configManager->getBoolValue(monsterZoneItem, true);
}

bool MainController::setDisplayToolbarsMonsterZoneState(bool value) {
    auto monsterZoneItem = std::string(DisplayToolbarsMonsterZoneItem);
    m_configManager->setBoolValue(monsterZoneItem, value);
    return saveConfigurationFile();
}

bool MainController::getDisplayToolbarsNPCWanderingZoneState() const {
    auto npcWanderingZoneItem = std::string(DisplayToolbarsNPCWanderingZoneItem);
    return m_configManager->getBoolValue(npcWanderingZoneItem, true);
}

bool MainController::setDisplayToolbarsNPCWanderingZoneState(bool value) {
    auto npcWanderingZoneItem = std::string(DisplayToolbarsNPCWanderingZoneItem);
    m_configManager->setBoolValue(npcWanderingZoneItem, value);
    return saveConfigurationFile();
}

bool MainController::setCustomKey(const std::string &path, const std::string &s) {
    m_configManager->setStringValue(path, s);
    return saveConfigurationFile();
}

}  // namespace mapeditor::controllers
