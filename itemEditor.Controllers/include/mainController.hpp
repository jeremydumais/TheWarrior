#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"
#include <memory>
#include <set>
#include <string>
#include <vector>

struct ItemListDisplay 
{
    std::string id;
    std::string name;
};

class MainController
{
public:
    MainController();
    TextureContainer &getTextureContainerForEdition();
    std::shared_ptr<ItemStore> getItemStore();
    const std::string &getLastError() const;
    bool openItemStore(const std::string &fileName);
    bool saveItemStore(const std::string &fileName);
    std::set<std::string> getItemCategories() const;
    std::vector<ItemListDisplay> getItemsFromCategory(const std::string &categoryName) const;
private:
    std::shared_ptr<ItemStore> m_itemStore;
    std::string m_lastError;
};