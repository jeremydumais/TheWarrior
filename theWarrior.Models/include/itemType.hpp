#pragma once

#include <optional>
#include <string>

namespace thewarrior::models {

enum class ItemType {
    Item,
    Weapon,
    Armor,
    StatsItem
};

std::string itemTypeToString(ItemType itemType);
std::optional<ItemType> itemTypeFromString(const std::string &value);

}  // namespace thewarrior::models
