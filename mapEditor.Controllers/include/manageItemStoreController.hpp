#pragma once

#include "itemStoreInfo.hpp"
#include <string>
#include <vector>

namespace mapeditor::controllers {

class ManageItemStoreController
{
public:
    ManageItemStoreController(const std::string &resourcesPath,
                              const std::string &userConfigFolder);
    const std::string &getResourcesPath() const;
    const std::string &getUserConfigFolder() const;
    const std::vector<ItemStoreInfo> getItemStores() const;
private:
    std::string m_resourcesPath = "";
    std::string m_userConfigFolder = "";
    std::vector<ItemStoreInfo> m_itemStores;
};

} // namespace mapeditor::controllers
