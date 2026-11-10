#pragma once

#include "itemStore.hpp"
#include "textureContainer.hpp"
#include <qicon.h>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <variant>
#include <vector>

namespace itemeditor::controllers {

struct WeaponListFields {
    std::string bodyPart;
    float attackGain;
};

struct ArmorListFields {
    std::string bodyPart;
    float defenseGain;
};

struct StatsItemListFields {
    std::string statChanging;
    float gain;
    bool limitOfOneApplied;
    unsigned int durationInTurns;
};

using ItemSpecificListFields =
    std::variant<
        std::monostate,
        WeaponListFields,
        ArmorListFields,
        StatsItemListFields>;

struct ItemListDisplay {
    std::string id;
    std::string name;
    unsigned int defaultBuyPrice;
    unsigned int defaultSellPrice;
    ItemSpecificListFields specificFields;
    std::string description;
};

class MainController {
 public:
    MainController();
    thewarrior::models::TextureContainer &getTextureContainerForEdition();
    std::shared_ptr<thewarrior::models::ItemStore> getItemStore();
    const std::string &getLastError() const;
    bool openItemStore(const std::string &fileName);
    bool saveItemStore(const std::string &fileName);
    std::set<std::string> getItemCategories() const;
    std::vector<ItemListDisplay> getItemsFromCategory(const std::string &categoryName) const;
    std::optional<thewarrior::models::ItemType> getItemTypeFromCategoryName(const std::string &categoryName) const;
    std::optional<thewarrior::models::ItemType> getItemTypeFromItemId(const std::string &id) const;
    std::map<std::string, QIcon> getIconsFromItemIds(std::vector<std::string> itemIds,
                                                     const std::string &resourcesPath) const;
    bool deleteItem(const std::string &id);
 private:
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    std::string m_lastError;
};

} // namespace itemeditor::controllers
