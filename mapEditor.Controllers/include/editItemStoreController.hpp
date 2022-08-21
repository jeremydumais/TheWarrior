#pragma once
#include "itemStoreInfo.hpp"
#include <vector>

namespace mapeditor::controllers {

class EditItemStoreController
{
public:
    explicit EditItemStoreController(const std::string &resourcesPath,
                                     const std::string &userConfigFolder,
                                     const std::vector<ItemStoreInfo> &itemStores);
    const std::string &getLastError() const;
    const std::string &getResourcesPath() const;
    bool saveItemStore();
private:
    std::string m_lastError = "";
    std::string m_resourcesPath = "";
    std::string m_userConfigFolder = "";
    std::vector<ItemStoreInfo> m_itemStores;
};

} // namespace mapeditor::controllers
