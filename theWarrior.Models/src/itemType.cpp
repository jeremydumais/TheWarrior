#include "itemType.hpp"
#include <boost/algorithm/string.hpp>

namespace thewarrior::models {

std::string itemTypeToString(ItemType itemType) {
    switch (itemType) {
    case ItemType::Item:
        return "Item";
    case ItemType::Armor:
        return "Armor";
    case ItemType::StatsItem:
        return "Stats Item";
    case ItemType::Weapon:
        return "Weapon";
    }
    return "";
}

std::optional<ItemType> itemTypeFromString(const std::string &value) {
    auto sanitizedValue = boost::trim_copy(boost::to_lower_copy(value));
    if (sanitizedValue == "item") {
        return ItemType::Item;
    }
    if (sanitizedValue == "armor") {
        return ItemType::Armor;
    }
    if (sanitizedValue == "stats item") {
        return ItemType::StatsItem;
    }
    if (sanitizedValue == "weapon") {
        return ItemType::Weapon;
    }
    return std::nullopt;
}

}  // namespace thewarrior::models
