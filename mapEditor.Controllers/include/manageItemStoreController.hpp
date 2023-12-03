#pragma once

#include <optional>
#include <string>
#include <vector>
#include "itemStoreInfo.hpp"

namespace mapeditor::controllers {

struct CompareItemStoreName {
 public:
    CompareItemStoreName(const std::string &name);
    bool operator() (const ItemStoreInfo &itemStore);

 private:
    std::string m_name;
};

class ManageItemStoreController {
 public:
    ManageItemStoreController(const std::string &resourcesPath,
                              const std::string &userConfigFolder,
                              const std::vector<ItemStoreInfo> &itemStores = {});
    const std::string &getLastError() const;
    const std::string &getResourcesPath() const;
    const std::string &getUserConfigFolder() const;
    const std::vector<ItemStoreInfo> &getItemStores() const;
    std::optional<ItemStoreInfo> findItemStore(const std::string &name) const;
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
    std::vector<ItemStoreInfo>::iterator getItemInStoreIterator(const std::string &name);
    std::vector<ItemStoreInfo>::const_iterator getItemInStoreIterator(const std::string &name) const;
    bool isNameAlreadyExists(const std::string &name) const;
};

}  // namespace mapeditor::controllers
