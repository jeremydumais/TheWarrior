#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "merchantInventory.hpp"
#include "merchantInventoryDTO.hpp"
#include "types.hpp"

namespace mapeditor::controllers {

class EditMerchantInventoryFormController {
 public:
    EditMerchantInventoryFormController(const std::shared_ptr<ContainerOfItemStore> &itemStores,
                                        const std::string &resourcesPath,
                                        thewarrior::models::MerchantInventoryType inventoryType,
                                        const std::optional<MerchantInventoryDTO> &selectedMerchantInventory,
                                        const std::vector<std::string> &alreadyUsedMerchantInventoryNames);
    const std::string &getLastError() const;
    const std::shared_ptr<ContainerOfItemStore> &getItemStores() const;
    const std::string &getResourcesPath() const;
    thewarrior::models::MerchantInventoryType getInventoryType() const;
    const std::vector<MerchantInventoryItemDTO> &getItems() const;
    const std::map<std::string, ItemDefaultPrice> &getItemsDefaultPrices() const;
    bool addItem(const std::string &itemId);

 private:
    std::string m_lastError;
    std::shared_ptr<ContainerOfItemStore> m_itemStores;
    std::string m_resourcesPath;
    thewarrior::models::MerchantInventoryType m_inventoryType;
    std::optional<MerchantInventoryDTO> m_selectedMerchantInventory;
    std::vector<std::string> m_alreadyUsedMerchantInventoryNames;
    std::vector<MerchantInventoryItemDTO> m_items;
    std::map<std::string, ItemDefaultPrice> m_itemsDefaultPrices;
    void calculateItemsDefaultPrices();
};

}  // namespace mapeditor::controllers
