#pragma once

#include "itemStoreInfo.hpp"
#include <string>
#include <vector>

namespace mapeditor::controllers {

class ManageItemStoreController
{
public:
    ManageItemStoreController(const std::string &resourcesPath,
                              const std::string &userConfigFolder,
                              const std::vector<ItemStoreInfo> &itemStores = {});
    const std::string &getLastError() const;
    const std::string &getResourcesPath() const;
    const std::string &getUserConfigFolder() const;
    const std::vector<ItemStoreInfo> getItemStores() const;
    bool addItemStore(const ItemStoreInfo &value);
    //TODO updateItemStore()
    //TODO deleteItemStore()
private:
    std::string m_lastError = "";
    std::string m_resourcesPath = "";
    std::string m_userConfigFolder = "";
    std::vector<ItemStoreInfo> m_itemStores;
    bool validateName(const std::string &name);
    bool validateFilename(const std::string &filename);
};

} // namespace mapeditor::controllers
