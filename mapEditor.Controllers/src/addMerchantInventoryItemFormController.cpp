#include <fmt/format.h>
#include <qicon.h>
#include <qpixmap.h>
#include <qstring.h>
#include <map>
#include <memory>
#include <set>
#include <ranges>
#include <string>
#include <vector>
#include "addMerchantInventoryItemFormController.hpp"
#include "item.hpp"
#include "itemType.hpp"
#include "merchantInventory.hpp"
#include "textureUtils.hpp"
#include "types.hpp"

using commoneditor::ui::TextureUtils;
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
                                                                                         const std::string &resourcesPath) const
{
    if (m_itemStores->empty()) {
        return {};
    }
    std::map<std::string, QIcon> retval;
    std::map<std::string, std::shared_ptr<QPixmap>> textures;

    auto itemStore = m_itemStores->begin()->second;
    for(const auto &itemId : itemIds) {
        //Find the item in the item store
        const auto &item = itemStore->findItem(itemId);
        if (item) {
            //Find the texture in the loaded pixmap collection
            const auto &textureName = item->getTextureName();
            auto textureIter = std::ranges::find_if(textures,
                                                    [textureName] (const std::pair<std::string, std::shared_ptr<QPixmap>> &texturePixmap) {
                return texturePixmap.first == textureName;
            });
            std::shared_ptr<QPixmap> pixmap = nullptr;
             //Find the texture
            auto texture = itemStore->getTextureContainer().getTextureByName(item->getTextureName());
            if (texture.has_value()) {
                //If not found, load it
                if (textureIter == textures.end()) {
                    auto completeTexturePath = fmt::format("{0}/textures/{1}", resourcesPath, texture->get().getFilename());
                    pixmap = std::make_shared<QPixmap>(QString(completeTexturePath.c_str()));
                    textures.insert({textureName, pixmap});
                }
                else {
                    pixmap = textureIter->second;
                }

                if (pixmap) {
                    auto iconPixmap = TextureUtils::getTextureTileImageFromTexture(pixmap.get(),
                                                                                   item->getTextureIndex(),
                                                                                   texture.value());
                    retval.insert({itemId, QIcon(iconPixmap)});
                }
            }
        }
    }
    return retval;
}


}  // namespace mapeditor::controllers
