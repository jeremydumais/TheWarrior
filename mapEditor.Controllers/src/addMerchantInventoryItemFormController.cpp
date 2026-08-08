#include <map>
#include <memory>
#include <set>
#include <ranges>
#include <string>
#include <vector>
#include "addMerchantInventoryItemFormController.hpp"
#include "item.hpp"
#include "itemIconProvider.hpp"
#include "itemType.hpp"
#include "merchantInventory.hpp"
#include "types.hpp"

using thewarrior::models::Item;
using thewarrior::models::ItemType;
using thewarrior::models::MerchantInventoryType;

namespace mapeditor::controllers {

AddMerchantInventoryItemFormController::AddMerchantInventoryItemFormController(const std::shared_ptr<ContainerOfItemStore> &itemStores,
                                        const std::string &resourcesPath,
                                        MerchantInventoryType inventoryType)
: m_itemStores(itemStores),
  m_resourcesPath(resourcesPath),
  m_inventoryType(inventoryType) {}

const std::string &AddMerchantInventoryItemFormController::getResourcesPath() const {
    return m_resourcesPath;
}

std::set<std::string> AddMerchantInventoryItemFormController::getItemCategories() const {
    std::set<std::string> categoriesFound {};
    if (m_itemStores->empty()) {
        return {};
    }
    auto items = m_itemStores->begin()->second->getItems();  // Only support one store for now
    for (const auto &item : items) {
        const auto itemType = item->getType();
        const bool isAccepted = m_inventoryType == MerchantInventoryType::WeaponsAndArmors
            ? itemType == ItemType::Weapon || itemType == ItemType::Armor
            : itemType == ItemType::StatsItem;
        if (isAccepted) {
            categoriesFound.insert(itemTypeToString(itemType));
        }
    }
    return categoriesFound;
}

std::vector<AddMerchantInventoryItemFormController::ItemListDisplay> AddMerchantInventoryItemFormController::getItemsFromCategory(const std::string &categoryName) const
{
    if (m_itemStores->empty()) {
        return {};
    }

    std::vector<ItemListDisplay> retval {};
    auto itemType = thewarrior::models::itemTypeFromString(categoryName);
    if (itemType.has_value()) {
        auto items = m_itemStores->begin()->second->getItems();  // Only support one store for now
        auto filterByItemType = [itemType](const auto &item) { return item->getType() == itemType.value(); };
        auto createItemListDisplay = [](const auto &item) { return ItemListDisplay { item->getId(),
                                                                                     item->getName(),
                                                                                     item->getOptionalDescription() }; };
        std::ranges::transform(items | std::views::filter(filterByItemType),
                               std::back_inserter(retval),
                               createItemListDisplay);
    }
    return retval;
}

std::map<std::string, QIcon> AddMerchantInventoryItemFormController::getIconsFromItemIds(const std::vector<std::string> &itemIds,
                                                                                         const std::string &resourcesPath) const {
    return ItemIconProvider::getIconsFromItemIds(m_itemStores, itemIds, resourcesPath);
}


}  // namespace mapeditor::controllers
