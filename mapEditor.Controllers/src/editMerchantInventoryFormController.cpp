#include <fmt/format.h>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <vector>
#include "editMerchantInventoryFormController.hpp"
#include "merchantInventoryDTO.hpp"

using thewarrior::models::MerchantInventoryType;

namespace mapeditor::controllers {

EditMerchantInventoryFormController::EditMerchantInventoryFormController(const std::shared_ptr<ContainerOfItemStore> &itemStores,
    const std::string &resourcesPath,
    thewarrior::models::MerchantInventoryType inventoryType,
    const std::optional<MerchantInventoryDTO> &selectedMerchantInventory,
    const std::vector<std::string> &alreadyUsedMerchantInventoryNames) 
: m_itemStores(itemStores),
m_resourcesPath(resourcesPath),
m_inventoryType(inventoryType),
m_selectedMerchantInventory(selectedMerchantInventory),
m_alreadyUsedMerchantInventoryNames(alreadyUsedMerchantInventoryNames) {
    calculateItemsDefaultPrices();
}

const std::string &EditMerchantInventoryFormController::getLastError() const {
    return m_lastError;
}

const std::shared_ptr<ContainerOfItemStore> &EditMerchantInventoryFormController::getItemStores() const {
    return m_itemStores;
}

const std::string &EditMerchantInventoryFormController::getResourcesPath() const {
    return m_resourcesPath;
}

MerchantInventoryType EditMerchantInventoryFormController::getInventoryType() const {
    return m_inventoryType;
}

const std::vector<MerchantInventoryItemDTO> &EditMerchantInventoryFormController::getItems() const {
    return m_items;
}

const std::map<std::string, ItemDefaultPrice> &EditMerchantInventoryFormController::getItemsDefaultPrices() const {
    return m_itemsDefaultPrices;
}

bool EditMerchantInventoryFormController::addItem(const std::string &itemId) {
    if (std::ranges::find_if(m_items, [&itemId](const MerchantInventoryItemDTO &itemDTO) {
        return (itemDTO.itemId == itemId);
    }) != m_items.end()) {
        m_lastError = fmt::format("The item {} already exist in the merchant inventory", itemId);
        return false;
    }
    m_items.push_back({ 
        .itemId = itemId, 
        .buyPriceOverride = std::nullopt, 
        .sellPriceOverride = std::nullopt
    });
    return true;
}

void EditMerchantInventoryFormController::calculateItemsDefaultPrices() {
    //TODO: Code this
}

}  // namespace mapeditor::controllers
