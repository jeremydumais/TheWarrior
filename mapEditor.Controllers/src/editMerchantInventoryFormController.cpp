#include <fmt/format.h>
#include <qicon.h>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <vector>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "editMerchantInventoryFormController.hpp"
#include "itemIconProvider.hpp"
#include "itemPricingDTO.hpp"
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
    if (selectedMerchantInventory.has_value()) {
        m_items = selectedMerchantInventory->items;
    }
    generateItemsProperties();
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

bool EditMerchantInventoryFormController::isMerchantInventoryNameAlreadyUsed(const std::string &name) const {
    auto sanitizedName = boost::to_lower_copy(boost::trim_copy(name));
    bool nameFound = std::ranges::find_if(m_alreadyUsedMerchantInventoryNames,
                                        [&sanitizedName](const std::string &merchantInventoryName) {
                                            return boost::to_lower_copy(boost::trim_copy(merchantInventoryName)) == sanitizedName;
                                        }) != m_alreadyUsedMerchantInventoryNames.end();
    return (!m_selectedMerchantInventory.has_value() && nameFound) ||
        (m_selectedMerchantInventory.has_value() && (boost::to_lower_copy(boost::trim_copy(m_selectedMerchantInventory->name)) != sanitizedName && nameFound));
}

std::optional<std::reference_wrapper<EditMerchantInventoryFormController::ItemPropertiesDTO>> EditMerchantInventoryFormController::getItemPropertiesById(const std::string &itemId) const {
    const auto iter = m_itemsProperties.find(itemId);
    if (iter == m_itemsProperties.end()) {
        return std::nullopt;
    }
    return std::ref(const_cast<ItemPropertiesDTO &>(iter->second));
}

std::map<std::string, QIcon> EditMerchantInventoryFormController::getIconsFromItemIds(const std::vector<std::string> &itemIds,
                                                                                      const std::string &resourcesPath) const {
    return ItemIconProvider::getIconsFromItemIds(m_itemStores, itemIds, resourcesPath);
}

bool EditMerchantInventoryFormController::addItem(const std::string &itemId) {
    if (std::ranges::find_if(m_items, [&itemId](const MerchantInventoryItemDTO &itemDTO) {
        return (itemDTO.id == itemId);
    }) != m_items.end()) {
        m_lastError = fmt::format("The item {} already exist in the merchant inventory", itemId);
        return false;
    }
    m_items.push_back({ 
        .id = itemId, 
        .buyPriceOverride = std::nullopt, 
        .sellPriceOverride = std::nullopt
    });
    return true;
}

bool EditMerchantInventoryFormController::removeItem(const std::string &itemId) {
    const auto iter = std::ranges::find_if(m_items, [&itemId](const MerchantInventoryItemDTO &itemDTO) {
        return (itemDTO.id == itemId);
    });
    if (iter == m_items.end()) {
        m_lastError = fmt::format("The item {} doesn't exist in the merchant inventory", itemId);
        return false;
    }
    m_items.erase(iter);
    return true;
}

void EditMerchantInventoryFormController::setOverridePrices(const std::string &itemId,
                                                            std::optional<unsigned int> buyPrice, 
                                                            std::optional<unsigned int> sellPrice) {
    const auto iter = std::ranges::find_if(m_items, [&itemId](const MerchantInventoryItemDTO &itemDTO) {
        return (itemDTO.id == itemId);
    });
    if (iter == m_items.end()) {
        m_lastError = fmt::format("The item {} doesn't exist in the merchant inventory", itemId);
        return;
    }
    iter->buyPriceOverride = buyPrice;
    iter->sellPriceOverride = sellPrice;

    const auto propertiesIter = m_itemsProperties.find(itemId);
    if (propertiesIter != m_itemsProperties.end()) {
        propertiesIter->second.buyPriceOverride = buyPrice;
        propertiesIter->second.sellPriceOverride = sellPrice;
    }
}


void EditMerchantInventoryFormController::generateItemsProperties() {
    if (m_itemStores->empty()) {
        return;
    }

    const auto items = m_itemStores->begin()->second->getItems();
    for (const auto &item : items) {
        const auto iter = std::ranges::find_if(m_items, [&item](const MerchantInventoryItemDTO &itemDTO) {
            return itemDTO.id == item->getId();
        });
        std::optional<unsigned int> buyPriceOverride;
        std::optional<unsigned int> sellPriceOverride;
        if (iter != m_items.end()) {
            buyPriceOverride = iter->buyPriceOverride;
            sellPriceOverride = iter->sellPriceOverride;
        }

        m_itemsProperties.emplace(item->getId(), EditMerchantInventoryFormController::ItemPropertiesDTO {
            ItemPricingDTO {
                .itemId = item->getId(),
                .defaultBuyPrice = item->getDefaultBuyPrice(),
                .defaultSellPrice = item->getDefaultSellPrice(),
                .buyPriceOverride = buyPriceOverride,
                .sellPriceOverride = sellPriceOverride
            },
            item->getName(),
        });
    }
}

}  // namespace mapeditor::controllers
