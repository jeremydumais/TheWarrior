#include "mainController.hpp"
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
        itemStore->addItem(std::make_shared<WeaponItem>(WeaponItemCreationInfo { "swd001", "Sword1", "Tex2", 1, 2.1F, WeaponBodyPart::SecondaryHand}));
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
    ASSERT_EQ(2, itemCategories.size());
    ASSERT_TRUE(itemCategories.find("Item") != itemCategories.end());
    ASSERT_TRUE(itemCategories.find("Weapon") != itemCategories.end());
}

//TODO Complete tests for all item categories

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