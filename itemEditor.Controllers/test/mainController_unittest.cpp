#include "mainController.hpp"
#include "armorItem.hpp"
#include "statsItem.hpp"
#include "weaponItem.hpp"
#include <gtest/gtest.h>
#include <memory>

class MainControllerSampleWithOneItem : public ::testing::Test
{
public:
    MainControllerSampleWithOneItem()
    {
        auto itemStore = controller.getItemStore();
        itemStore->addItem(std::make_shared<Item>(ItemCreationInfo { "ite001", "Item1", "Tex1", 0}));
    }
    MainController controller;
};

class MainControllerSampleWithOneItemPerCategory : public ::testing::Test
{
public:
    MainControllerSampleWithOneItemPerCategory()
    {
        auto itemStore = controller.getItemStore();
        itemStore->addItem(std::make_shared<Item>(ItemCreationInfo { "ite001", "Item1", "Tex1", 0}));
        itemStore->addItem(std::make_shared<WeaponItem>(WeaponItemCreationInfo { "swd001", "Sword1", "Tex2", 1, "", 2.1F, WeaponBodyPart::SecondaryHand}));
        itemStore->addItem(std::make_shared<ArmorItem>(ArmorItemCreationInfo { "arm001", "Armor1", "Tex3", 2, "", 3.2F, ArmorBodyPart::Hands}));
        itemStore->addItem(std::make_shared<StatsItem>(StatsItemCreationInfo { "pot001", "Potion1", "Tex4", 3, "", Stats::Endurance, 4.1F, true, 0}));
    }
    MainController controller;
};

TEST(MainController_getItemCategories, With_EmptyItemStore_ReturnEmpty) 
{
    MainController controller;
    ASSERT_EQ(0, controller.getItemCategories().size());
}

TEST_F(MainControllerSampleWithOneItem, getItemCategories_ReturnOneCategory)
{
    auto itemCategories = controller.getItemCategories();
    ASSERT_EQ(1, itemCategories.size());
    ASSERT_TRUE(itemCategories.find("Item") != itemCategories.end());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemCategories_ReturnTwoCategory)
{
    auto itemCategories = controller.getItemCategories();
    ASSERT_EQ(4, itemCategories.size());
    ASSERT_TRUE(itemCategories.find("Item") != itemCategories.end());
    ASSERT_TRUE(itemCategories.find("Weapon") != itemCategories.end());
    ASSERT_TRUE(itemCategories.find("Armor") != itemCategories.end());
    ASSERT_TRUE(itemCategories.find("Stats Item") != itemCategories.end());
}

TEST_F(MainControllerSampleWithOneItem, getItemsFromCategory_WithEmptyCategory_ReturnEmpty)
{
    auto items = controller.getItemsFromCategory("");
    ASSERT_EQ(0, items.size());
}

TEST_F(MainControllerSampleWithOneItem, getItemsFromCategory_WithItem_Return1Item)
{
    auto items = controller.getItemsFromCategory("Item");
    ASSERT_EQ(1, items.size());
    ASSERT_EQ("ite001", items[0].id);
    ASSERT_EQ("Item1", items[0].name);
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemsFromCategory_WithStatsItem_Return1Item)
{
    auto items = controller.getItemsFromCategory("Stats Item");
    ASSERT_EQ(1, items.size());
    ASSERT_EQ("pot001", items[0].id);
    ASSERT_EQ("Potion1", items[0].name);
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemTypeFromItemId_WithNotExistantId_ReturnNullOpt)
{
    ASSERT_FALSE(controller.getItemTypeFromItemId("ite000").has_value());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemTypeFromItemId_WithIte001_ReturnItem)
{
    auto actual = controller.getItemTypeFromItemId("ite001");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(ItemType::Item, actual.value());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemTypeFromItemId_WithSwd001_ReturnWeapon)
{
    auto actual = controller.getItemTypeFromItemId("swd001");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(ItemType::Weapon, actual.value());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemTypeFromItemId_WithArm001_ReturnArmor)
{
    auto actual = controller.getItemTypeFromItemId("arm001");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(ItemType::Armor, actual.value());
}

TEST_F(MainControllerSampleWithOneItemPerCategory, getItemTypeFromItemId_WithPot001_ReturnPotion)
{
    auto actual = controller.getItemTypeFromItemId("pot001");
    ASSERT_TRUE(actual.has_value());
    ASSERT_EQ(ItemType::StatsItem, actual.value());
}