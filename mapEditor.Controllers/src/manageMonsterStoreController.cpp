#include "manageMonsterStoreController.hpp"
#include "configurationManager.hpp"
#include "monsterStoreInfoJSONSerializer.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <optional>
#include <stdexcept>

using namespace thewarrior::storage;
using namespace boost::algorithm;

namespace mapeditor::controllers {

const std::string ManageMonsterStoreController::MONSTERSTORES_PATH{"MonsterStores"};

CompareMonsterStoreName::CompareMonsterStoreName(const std::string &name)
    : m_name(name) {}

bool CompareMonsterStoreName::operator() (const MonsterStoreInfo &monsterStore)
{
    return to_lower_copy(trim_copy(monsterStore.name)) == to_lower_copy(m_name);
}

ManageMonsterStoreController::ManageMonsterStoreController(const std::string &resourcesPath,
                                                     const std::string &userConfigFolder,
                                                     const std::vector<MonsterStoreInfo> &monsterStores)
    : m_resourcesPath(resourcesPath),
      m_userConfigFolder(userConfigFolder),
      m_monsterStores(monsterStores)
{
    if (trim_copy(resourcesPath).empty()) {
        throw std::invalid_argument("resourcesPath cannot be empty");
    }
    if (trim_copy(userConfigFolder).empty()) {
        throw std::invalid_argument("userConfigFolder cannot be empty");
    }
}

const std::string &ManageMonsterStoreController::getLastError() const
{
    return m_lastError;
}

const std::string &ManageMonsterStoreController::getResourcesPath() const
{
    return m_resourcesPath;
}

const std::string &ManageMonsterStoreController::getUserConfigFolder() const
{
    return m_userConfigFolder;
}

const std::vector<MonsterStoreInfo> &ManageMonsterStoreController::getMonsterStores() const
{
    return m_monsterStores;
}

std::optional<MonsterStoreInfo> ManageMonsterStoreController::findMonsterStore(const std::string &name) const
{
    const auto iter = getMonsterInStoreIterator(trim_copy(name));
    if (iter != m_monsterStores.cend()) {
        return *iter;
    }
    return std::nullopt;
}

bool ManageMonsterStoreController::addMonsterStore(const MonsterStoreInfo &value)
{
    const std::string nameToAdd = trim_copy(value.name);
    if (!validateName(nameToAdd, "name")) {
        return false;
    }
    if (!validateFilename(value.filename, "filename")) {
        return false;
    }
    //Check if monster store name already exists
    if (!isNameAlreadyExists(nameToAdd)) {
        m_monsterStores.push_back(value);
    }
    else {
        m_lastError = fmt::format("name {0} already exist.", nameToAdd);
        return false;
    }
    return true;
}

bool ManageMonsterStoreController::updateMonsterStore(const std::string &monsterStoreNameToEdit, const MonsterStoreInfo &newValue)
{
    auto sanitizeMonsterNameToEdit = trim_copy(monsterStoreNameToEdit);
    if (!validateName(sanitizeMonsterNameToEdit, "name to edit")) {
        return false;
    }
    const std::string updatedName = trim_copy(newValue.name);
    if (!validateName(updatedName, "updated name")) {
        return false;
    }
    if (!validateFilename(newValue.filename, "updated filename")) {
        return false;
    }
    auto monsterToUpdate = getMonsterInStoreIterator(sanitizeMonsterNameToEdit);
    if (monsterToUpdate == m_monsterStores.end()) {
        m_lastError = fmt::format("name {0} doesn't exist.", sanitizeMonsterNameToEdit);
        return false;
    }
    //Check if the new name is not already in the store
    if (to_lower_copy(updatedName) != to_lower_copy(sanitizeMonsterNameToEdit)
            && isNameAlreadyExists(updatedName)) {
        m_lastError = fmt::format("name {0} already exist.", updatedName);
        return false;
    }
    monsterToUpdate->name = updatedName;
    monsterToUpdate->filename = newValue.filename;
    return true;
}

bool ManageMonsterStoreController::deleteMonsterStore(const std::string &monsterStoreNameToDelete)
{
    auto sanitizeMonsterNameToDelete = trim_copy(monsterStoreNameToDelete);
    if (!validateName(sanitizeMonsterNameToDelete, "name to delete")) {
        return false;
    }
    auto monster = getMonsterInStoreIterator(sanitizeMonsterNameToDelete);
    if (monster == m_monsterStores.end()) {
        m_lastError = fmt::format("name {0} doesn't exist.", sanitizeMonsterNameToDelete);
        return false;
    }
    m_monsterStores.erase(monster);
    return true;
}

bool ManageMonsterStoreController::loadMonsterStore()
{
    ConfigurationManager configManager(m_userConfigFolder + "config.json");
    if (configManager.load()) {
        auto ptreeNode = configManager.getPTreeNode(ManageMonsterStoreController::MONSTERSTORES_PATH);
        m_monsterStores = MonsterStoreInfoJSONSerializer::deserialize(ptreeNode);
    }
    else {
        m_lastError = fmt::format("An error occurred while loading the configuration file. {0}",
                                  configManager.getLastError());
        return false;
    }
    return true;
}

bool ManageMonsterStoreController::saveMonsterStore()
{
    ConfigurationManager configManager(m_userConfigFolder + "config.json");
    if (configManager.load()) {
        configManager.setPTreeNode(ManageMonsterStoreController::MONSTERSTORES_PATH,
                                   MonsterStoreInfoJSONSerializer::serialize(m_monsterStores));
        if (!configManager.save()) {
            m_lastError = fmt::format("An error occurred while saving the configuration file. {0}",
                                      configManager.getLastError());
            return false;
        }
    }
    else {
        m_lastError = fmt::format("An error occurred while loading the configuration file. {0}",
                                  configManager.getLastError());
        return false;
    }
    return true;
}

bool ManageMonsterStoreController::validateName(const std::string &name, const std::string &field)
{
    if (trim_copy(name).empty()) {
        m_lastError = fmt::format("{0} cannot be empty.", field);
        return false;
    }
    return true;
}

bool ManageMonsterStoreController::validateFilename(const std::string &filename, const std::string &field)
{
    if (trim_copy(filename).empty()) {
        m_lastError = fmt::format("{0} cannot be empty.", field);
        return false;
    }
    return true;
}

std::vector<MonsterStoreInfo>::iterator ManageMonsterStoreController::getMonsterInStoreIterator(const std::string &name)
{
    CompareMonsterStoreName compareName(name);
    return std::find_if(m_monsterStores.begin(),
                        m_monsterStores.end(),
                        compareName);
}

std::vector<MonsterStoreInfo>::const_iterator ManageMonsterStoreController::getMonsterInStoreIterator(const std::string &name) const
{
    CompareMonsterStoreName compareName(name);
    return std::find_if(m_monsterStores.cbegin(),
                        m_monsterStores.cend(),
                        compareName);
}

bool ManageMonsterStoreController::isNameAlreadyExists(const std::string &name) const
{
    return getMonsterInStoreIterator(name) != m_monsterStores.end() ;
}

} // namespace mapeditor::controllers
