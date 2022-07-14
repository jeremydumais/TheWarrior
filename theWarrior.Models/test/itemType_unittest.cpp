#include "itemType.hpp"
#include <gtest/gtest.h>

TEST(itemTypeToString, WithItem_ReturnItem) 
{
    ASSERT_EQ("Item", itemTypeToString(ItemType::Item));
}

TEST(itemTypeToString, WithArmor_ReturnArmor) 
{
    ASSERT_EQ("Armor", itemTypeToString(ItemType::Armor));
}

TEST(itemTypeToString, WithStatsItem_ReturnStatsItem) 
{
    ASSERT_EQ("Stats Item", itemTypeToString(ItemType::StatsItem));
}

TEST(itemTypeToString, WithWeapon_ReturnWeapon) 
{
    ASSERT_EQ("Weapon", itemTypeToString(ItemType::Weapon));
}

TEST(itemTypeFromString, WithEmptyString_ReturnNullOpt)
{
    ASSERT_FALSE(itemTypeFromString("").has_value());
}

TEST(itemTypeFromString, WithWhiteSpacesString_ReturnNullOpt)
{
    ASSERT_FALSE(itemTypeFromString("   ").has_value());
}

TEST(itemTypeFromString, WithItem_ReturnItem)
{
    auto result = itemTypeFromString("Item");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::Item, result.value());
}

TEST(itemTypeFromString, WithItemCaps_ReturnItem)
{
    auto result = itemTypeFromString("ITEM");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::Item, result.value());
}

TEST(itemTypeFromString, WithItemMixedWithSpaces_ReturnItem)
{
    auto result = itemTypeFromString("    IteM   ");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::Item, result.value());
}

TEST(itemTypeFromString, WithArmor_ReturnArmor)
{
    auto result = itemTypeFromString("Armor");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::Armor, result.value());
}

TEST(itemTypeFromString, WithArmorCaps_ReturnArmor)
{
    auto result = itemTypeFromString("ARMOR");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::Armor, result.value());
}

TEST(itemTypeFromString, WithArmorMixedWithSpaces_ReturnArmor)
{
    auto result = itemTypeFromString("    ArmOR   ");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::Armor, result.value());
}

TEST(itemTypeFromString, WithStatsItem_ReturnStatsItem)
{
    auto result = itemTypeFromString("Stats Item");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::StatsItem, result.value());
}

TEST(itemTypeFromString, WithStatsItemCaps_ReturnStatsItem)
{
    auto result = itemTypeFromString("STATS ITEM");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::StatsItem, result.value());
}

TEST(itemTypeFromString, WithStatsItemMixedWithSpaces_ReturnStatsItem)
{
    auto result = itemTypeFromString("   stATS iteM   ");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::StatsItem, result.value());
}

TEST(itemTypeFromString, WithWeapon_ReturnWeapon)
{
    auto result = itemTypeFromString("Weapon");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::Weapon, result.value());
}

TEST(itemTypeFromString, WithWeaponCaps_ReturnWeapon)
{
    auto result = itemTypeFromString("WEAPON");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::Weapon, result.value());
}

TEST(itemTypeFromString, WithWeaponMixedWithSpaces_ReturnWeapon)
{
    auto result = itemTypeFromString("  WeAPon   ");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(ItemType::Weapon, result.value());
}