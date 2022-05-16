#pragma once

#include <optional>
#include <string>

enum class ItemType 
{
	Item,
	Weapon,
	Armor,
	StatsItem
};

std::string itemTypeToString(ItemType itemType);
std::optional<ItemType> itemTypeFromString(const std::string &value);