#pragma once

#include <qicon.h>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "itemPricingDTO.hpp"
#include "merchantInventory.hpp"
#include "merchantInventoryDTO.hpp"
#include "types.hpp"

namespace mapeditor::controllers {

class EditMerchantInventoryFormController {
 public:
    struct ItemPropertiesDTO : public ItemPricingDTO {
      std::string name;
    };
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
    bool isMerchantInventoryNameAlreadyUsed(const std::string &name) const;
    std::optional<std::reference_wrapper<ItemPropertiesDTO>> getItemPropertiesById(const std::string &itemId) const;
    std::map<std::string, QIcon> getIconsFromItemIds(const std::vector<std::string> &itemIds,
                                                    const std::string &resourcesPath) const;
                              
    bool addItem(const std::string &itemId);
    bool removeItem(const std::string &itemId);
    void setOverridePrices(const std::string &itemId,
                           std::optional<unsigned int> buyPrice, 
                           std::optional<unsigned int> sellPrice);

 private:
    std::string m_lastError;
    std::shared_ptr<ContainerOfItemStore> m_itemStores;
    std::string m_resourcesPath;
    thewarrior::models::MerchantInventoryType m_inventoryType;
    std::optional<MerchantInventoryDTO> m_selectedMerchantInventory;
    std::vector<std::string> m_alreadyUsedMerchantInventoryNames;
    std::vector<MerchantInventoryItemDTO> m_items;

    std::map<std::string, ItemPropertiesDTO> m_itemsProperties;
    void generateItemsProperties();
};

}  // namespace mapeditor::controllers
