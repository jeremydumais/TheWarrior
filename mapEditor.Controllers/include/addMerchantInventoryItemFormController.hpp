#pragma once

#include <qicon.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "merchantInventory.hpp"
#include "types.hpp"

namespace mapeditor::controllers {

class AddMerchantInventoryItemFormController {
 public:
    struct ItemListDisplay {
        std::string id;
        std::string name;
        std::string description;
    };
    AddMerchantInventoryItemFormController(const std::shared_ptr<ContainerOfItemStore> &itemStores,
                                          const std::string &resourcesPath,
                                          thewarrior::models::MerchantInventoryType inventoryType);
    const std::string &getResourcesPath() const;
    std::set<std::string> getItemCategories() const;
    std::vector<ItemListDisplay> getItemsFromCategory(const std::string &categoryName) const;
    std::map<std::string, QIcon> getIconsFromItemIds(const std::vector<std::string> &itemIds,
                                                    const std::string &resourcesPath) const;

 private:
    std::shared_ptr<ContainerOfItemStore> m_itemStores;
    std::string m_resourcesPath;
    thewarrior::models::MerchantInventoryType m_inventoryType;
   
};

}  // namespace mapeditor::controllers
