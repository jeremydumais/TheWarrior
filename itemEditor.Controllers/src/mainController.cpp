#include "mainController.hpp"
#include "armorItem.hpp"
#include "itemStoreStorage.hpp"
#include "manageItemController.hpp"
#include "statsItem.hpp"
#include "textureUtils.hpp"
#include "weaponItem.hpp"
#include <fmt/format.h>
#include <qpixmap.h>
#include <algorithm>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <ranges>
#include <variant>

using namespace commoneditor::ui;
using namespace thewarrior::models;
using namespace thewarrior::storage;

namespace itemeditor::controllers {

MainController::MainController()
    : m_itemStore(std::make_shared<ItemStore>()),
      m_lastError("")
{
}

TextureContainer &MainController::getTextureContainerForEdition()
{
    return m_itemStore->getTextureContainerForEdition();
}

std::shared_ptr<ItemStore> MainController::getItemStore()
{
    return m_itemStore;
}

const std::string &MainController::getLastError() const
{
    return m_lastError;
}

bool MainController::openItemStore(const std::string &fileName) {
    ItemStoreStorage storage;
    try {
        storage.loadItemStore(fileName, m_itemStore);
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

bool MainController::saveItemStore(const std::string &fileName)
{
    ItemStoreStorage storage;
    try {
        storage.saveItemStore(fileName, m_itemStore);
        return true;
    }
    catch(const std::exception &err) {
        m_lastError = err.what();
    }
    return false;
}

std::set<std::string> MainController::getItemCategories() const
{
    std::set<std::string> categoriesFound {};
    auto items = m_itemStore->getItems();
    std::transform(items.begin(),
                   items.end(),
                   std::inserter(categoriesFound, categoriesFound.begin()),
                   [] (std::shared_ptr<Item> item) {
                       return itemTypeToString(item->getType());
                   });
    return categoriesFound;
}

std::vector<ItemListDisplay> MainController::getItemsFromCategory(const std::string &categoryName) const
{
    std::vector<ItemListDisplay> retval {};
    auto itemType = itemTypeFromString(categoryName);
    if (itemType.has_value()) {
        auto items = m_itemStore->getItems();
        auto filterByItemType = [itemType](const auto &item) { return item->getType() == itemType.value(); };
        auto createItemListDisplay = [type = itemType.value()](const auto &item) {
            ItemSpecificListFields specificFields = std::monostate();
            switch (type) {
                case ItemType::Weapon: {
                    const auto weapon = std::static_pointer_cast<WeaponItem>(item);
                    specificFields = WeaponListFields {
                        WeaponItem::getBodyPartAsString(weapon->getSlotInBodyPart()),
                        weapon->getAttackGain()
                    };
                    break;
                }
                case ItemType::Armor: {
                    const auto armor = std::static_pointer_cast<ArmorItem>(item);
                    specificFields = ArmorListFields {
                        ArmorItem::getBodyPartAsString(armor->getSlotInBodyPart()),
                        armor->getDefenseGain()
                    };
                    break;
                }
                case ItemType::StatsItem: {
                    const auto statsItem = std::static_pointer_cast<StatsItem>(item);
                    const std::string statChanging = [stat = statsItem->getStatChanging()] {
                        switch (stat) {
                        case Stats::Vitality: return std::string("Vitality");
                        case Stats::Strength: return std::string("Strength");
                        case Stats::Endurance: return std::string("Endurance");
                        }
                        return std::string();
                    }();
                    specificFields = StatsItemListFields {
                        statChanging,
                        statsItem->getGain(),
                        statsItem->getLimitOfOneApplied(),
                        statsItem->getDurationInTurn()
                    };
                    break;
                }
                case ItemType::Item:
                    break;
                }
            return ItemListDisplay {
            .id = item->getId(),
            .name = item->getName(),
            .defaultBuyPrice = item->getDefaultBuyPrice(),
            .defaultSellPrice = item->getDefaultSellPrice(),
            .specificFields = std::move(specificFields),
            .description = item->getOptionalDescription()
        }; };
        std::ranges::transform(items | std::views::filter(filterByItemType),
                               std::back_inserter(retval),
                               createItemListDisplay);
    }
    return retval;
}

std::optional<ItemType> MainController::getItemTypeFromCategoryName(const std::string &categoryName) const
{
    return itemTypeFromString(categoryName);
}


std::optional<ItemType> MainController::getItemTypeFromItemId(const std::string &id) const
{
    auto item = m_itemStore->findItem(id);
    if (item != nullptr) {
        return item->getType();
    }
    return std::nullopt;
}

std::map<std::string, QIcon> MainController::getIconsFromItemIds(std::vector<std::string> itemIds,
                                                                 const std::string &resourcesPath) const
{
    std::map<std::string, QIcon> retval;
    std::map<std::string, std::shared_ptr<QPixmap>> textures;

    for(const auto &itemId : itemIds) {
        //Find the item in the item store
        const auto &item = m_itemStore->findItem(itemId);
        if (item) {
            //Find the texture in the loaded pixmap collection
            const auto &textureName = item->getTextureName();
            auto textureIter = std::find_if(textures.begin(),
                                            textures.end(),
                                            [textureName] (const std::pair<std::string, std::shared_ptr<QPixmap>> texturePixmap) {
                return texturePixmap.first == textureName;
            });
            std::shared_ptr<QPixmap> pixmap = nullptr;
             //Find the texture
            auto texture = m_itemStore->getTextureContainer().getTextureByName(item->getTextureName());
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

bool MainController::deleteItem(const std::string &id)
{
    ManageItemController itemController(m_itemStore);
    if (!itemController.deleteItem(id)) {
        m_lastError = itemController.getLastError();
        return false;
    }
    return true;
}

} // namespace itemeditor::controllers
