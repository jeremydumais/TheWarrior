#include "manageItemStoreController.hpp"
#include "configurationManager.hpp"
#include "itemStoreInfoJSONSerializer.hpp"
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

const std::string ManageItemStoreController::ITEMSTORES_PATH{"ItemStores"};

CompareItemStoreName::CompareItemStoreName(const std::string &name)
    : m_name(name) {}

bool CompareItemStoreName::operator() (const ItemStoreInfo &itemStore)
{
    return to_lower_copy(trim_copy(itemStore.name)) == to_lower_copy(m_name);
}

ManageItemStoreController::ManageItemStoreController(const std::string &resourcesPath,
                                                     const std::string &userConfigFolder,
                                                     const std::vector<ItemStoreInfo> &itemStores)
    : m_resourcesPath(resourcesPath),
      m_userConfigFolder(userConfigFolder),
      m_itemStores(itemStores)
{
    if (trim_copy(resourcesPath).empty()) {
        throw std::invalid_argument("resourcesPath cannot be empty");
    }
    if (trim_copy(userConfigFolder).empty()) {
        throw std::invalid_argument("userConfigFolder cannot be empty");
    }
}

const std::string &ManageItemStoreController::getLastError() const
{
    return m_lastError;
}

const std::string &ManageItemStoreController::getResourcesPath() const
{
    return m_resourcesPath;
}

const std::string &ManageItemStoreController::getUserConfigFolder() const
{
    return m_userConfigFolder;
}

const std::vector<ItemStoreInfo> &ManageItemStoreController::getItemStores() const
{
    return m_itemStores;
}

std::optional<ItemStoreInfo> ManageItemStoreController::findItemStore(const std::string &name) const
{
    const auto iter = getItemInStoreIterator(trim_copy(name));
    if (iter != m_itemStores.cend()) {
        return *iter;
    }
    return std::nullopt;
}

bool ManageItemStoreController::addItemStore(const ItemStoreInfo &value)
{
    const std::string nameToAdd = trim_copy(value.name);
    if (!validateName(nameToAdd, "name")) {
        return false;
    }
    if (!validateFilename(value.filename, "filename")) {
        return false;
    }
    //Check if item store name already exists
    if (!isNameAlreadyExists(nameToAdd)) {
        m_itemStores.push_back(value);
    }
    else {
        m_lastError = fmt::format("name {0} already exist.", nameToAdd);
        return false;
    }
    return true;
}

bool ManageItemStoreController::updateItemStore(const std::string &itemNameToEdit, const ItemStoreInfo &newValue)
{
    auto sanitizeItemNameToEdit = trim_copy(itemNameToEdit);
    if (!validateName(sanitizeItemNameToEdit, "name to edit")) {
        return false;
    }
    const std::string updatedName = trim_copy(newValue.name);
    if (!validateName(updatedName, "updated name")) {
        return false;
    }
    if (!validateFilename(newValue.filename, "updated filename")) {
        return false;
    }
    auto itemToUpdate = getItemInStoreIterator(sanitizeItemNameToEdit);
    if (itemToUpdate == m_itemStores.end()) {
        m_lastError = fmt::format("name {0} doesn't exist.", sanitizeItemNameToEdit);
        return false;
    }
    //Check if the new name is not already in the store
    if (to_lower_copy(updatedName) != to_lower_copy(sanitizeItemNameToEdit)
            && isNameAlreadyExists(updatedName)) {
        m_lastError = fmt::format("name {0} already exist.", updatedName);
        return false;
    }
    itemToUpdate->name = updatedName;
    itemToUpdate->filename = newValue.filename;
    return true;
}

bool ManageItemStoreController::deleteItemStore(const std::string &itemNameToDelete)
{
    auto sanitizeItemNameToDelete = trim_copy(itemNameToDelete);
    if (!validateName(sanitizeItemNameToDelete, "name to delete")) {
        return false;
    }
    auto item = getItemInStoreIterator(sanitizeItemNameToDelete);
    if (item == m_itemStores.end()) {
        m_lastError = fmt::format("name {0} doesn't exist.", sanitizeItemNameToDelete);
        return false;
    }
    m_itemStores.erase(item);
    return true;
}

bool ManageItemStoreController::loadItemStore()
{
    ConfigurationManager configManager(m_userConfigFolder + "config.json");
    if (configManager.load()) {
        auto ptreeNode = configManager.getPTreeNode(ManageItemStoreController::ITEMSTORES_PATH);
        m_itemStores = ItemStoreInfoJSONSerializer::deserialize(ptreeNode);
    }
    else {
        m_lastError = fmt::format("An error occurred while loading the configuration file. {0}",
                                  configManager.getLastError());
        return false;
    }
    return true;
}

bool ManageItemStoreController::saveItemStore()
{
    ConfigurationManager configManager(m_userConfigFolder + "config.json");
    if (configManager.load()) {
        configManager.setPTreeNode(ManageItemStoreController::ITEMSTORES_PATH,
                                   ItemStoreInfoJSONSerializer::serialize(m_itemStores));
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

bool ManageItemStoreController::validateName(const std::string &name, const std::string &field)
{
    if (trim_copy(name).empty()) {
        m_lastError = fmt::format("{0} cannot be empty.", field);
        return false;
    }
    return true;
}

bool ManageItemStoreController::validateFilename(const std::string &filename, const std::string &field)
{
    if (trim_copy(filename).empty()) {
        m_lastError = fmt::format("{0} cannot be empty.", field);
        return false;
    }
    return true;
}



std::vector<ItemStoreInfo>::iterator ManageItemStoreController::getItemInStoreIterator(const std::string &name)
{
    CompareItemStoreName compareName(name);
    return std::find_if(m_itemStores.begin(),
                        m_itemStores.end(),
                        compareName);
}

std::vector<ItemStoreInfo>::const_iterator ManageItemStoreController::getItemInStoreIterator(const std::string &name) const
{
    CompareItemStoreName compareName(name);
    return std::find_if(m_itemStores.cbegin(),
                        m_itemStores.cend(),
                        compareName);
}

bool ManageItemStoreController::isNameAlreadyExists(const std::string &name) const
{
    return getItemInStoreIterator(name) != m_itemStores.end() ;
}

} // namespace mapeditor::controllers
