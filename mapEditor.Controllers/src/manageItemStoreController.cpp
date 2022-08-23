#include "manageItemStoreController.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fmt/format.h>
#include <stdexcept>

using namespace boost::algorithm;

namespace mapeditor::controllers {

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

const std::vector<ItemStoreInfo> ManageItemStoreController::getItemStores() const
{
    return m_itemStores;
}

bool ManageItemStoreController::addItemStore(const ItemStoreInfo &value)
{
    const std::string nameToAdd = trim_copy(value.name);
    if (!validateName(nameToAdd)) {
        return false;
    }
    if (!validateFilename(value.filename)) {
        return false;
    }
    //Check if item store name already exists
    if (std::find_if(m_itemStores.begin(),
                     m_itemStores.end(),
                     [&nameToAdd] (const auto &itemStore) {
            return to_lower_copy(trim_copy(itemStore.name)) == to_lower_copy(nameToAdd);
        }) == m_itemStores.end()) {
        m_itemStores.push_back(value);
    }
    else {
        m_lastError = fmt::format("name {0} already exist.", nameToAdd);
        return false;
    }
    return true;
}

bool ManageItemStoreController::validateName(const std::string &name)
{
    if (trim_copy(name).empty()) {
        m_lastError = "name cannot be empty.";
        return false;
    }
    return true;
}

bool ManageItemStoreController::validateFilename(const std::string &filename)
{
    if (trim_copy(filename).empty()) {
        m_lastError = "filename cannot be empty.";
        return false;
    }
    return true;
}

} // namespace mapeditor::controllers
