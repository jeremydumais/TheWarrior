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
    const std::vector<ItemStoreInfo> &getItemStores() const;
    bool addItemStore(const ItemStoreInfo &value);
    bool updateItemStore(const std::string &itemNameToEdit, const ItemStoreInfo &newValue);
    bool deleteItemStore(const std::string &itemNameToDelete);
    bool loadItemStore();
    bool saveItemStore();
private:
    std::string m_lastError = "";
    std::string m_resourcesPath = "";
    std::string m_userConfigFolder = "";
    std::vector<ItemStoreInfo> m_itemStores;
    static const std::string ITEMSTORES_PATH;
    bool validateName(const std::string &name, const std::string &field);
    bool validateFilename(const std::string &filename, const std::string &field);
    std::vector<ItemStoreInfo>::iterator findItemInStore(const std::string &name);
    bool isNameAlreadyExists(const std::string &name);
};

} // namespace mapeditor::controllers
