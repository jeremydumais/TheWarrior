#include "manageItemStoreController.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <stdexcept>

using namespace boost::algorithm;

namespace mapeditor::controllers {

ManageItemStoreController::ManageItemStoreController(const std::string &resourcesPath,
                                                     const std::string &userConfigFolder)
    : m_resourcesPath(resourcesPath),
      m_userConfigFolder(userConfigFolder)
{
    if (trim_copy(resourcesPath).empty()) {
        throw std::invalid_argument("resourcesPath cannot be empty");
    }
    if (trim_copy(userConfigFolder).empty()) {
        throw std::invalid_argument("userConfigFolder cannot be empty");
    }
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

} // namespace mapeditor::controllers
