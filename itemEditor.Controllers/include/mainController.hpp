#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"
#include <qicon.h>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace itemeditor::controllers {

struct ItemListDisplay
{
    std::string id;
    std::string name;
    std::string description;
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
    std::optional<ItemType> getItemTypeFromItemId(const std::string &id) const;
    std::map<std::string, QIcon> getIconsFromItemIds(std::vector<std::string> itemIds,
                                                     const std::string &resourcesPath) const;
    bool deleteItem(const std::string &id);
private:
    std::shared_ptr<ItemStore> m_itemStore;
    std::string m_lastError;
};

} // namespace itemeditor::controllers
