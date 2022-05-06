#include "itemStore.hpp"
#include <gtest/gtest.h>

using namespace std;

const Item itemSample1({ "shd001", "Wooden Shield", "tex1", 1});
const Item itemSample2({ "shd002", "Iron Shield", "tex1", 2});

struct DefaultConstructedItemStore : public ::testing::Test
{
    ItemStore itemStore;
};

struct ItemStoreWithTwoItems : public ::testing::Test
{
    ItemStoreWithTwoItems()
    {
        itemStore.addItem(itemSample1);
        itemStore.addItem(itemSample2);
    }
    ItemStore itemStore;
};

TEST_F(DefaultConstructedItemStore, ReturnSuccess)
{
    ItemStore store;
    ASSERT_EQ(0, itemStore.getItemCount());
}

TEST_F(DefaultConstructedItemStore, getItemCount_ReturnZero)
{
    ASSERT_EQ(0, itemStore.getItemCount());
}

TEST_F(DefaultConstructedItemStore, getItems_ReturnEmpty)
{
    ASSERT_EQ(0, itemStore.getItems().size());
}

TEST_F(ItemStoreWithTwoItems, getItems_Return2Items)
{
    auto items = itemStore.getItems();
    ASSERT_EQ(2, items.size());
    ASSERT_EQ("shd001", items[0].get().getId());
    ASSERT_EQ("shd002", items[1].get().getId());
}

TEST_F(ItemStoreWithTwoItems, findItem_NonExistingItem_ReturnEmpty)
{
    auto result = itemStore.findItem("non123");
    ASSERT_FALSE(result.has_value());
}

TEST_F(ItemStoreWithTwoItems, findItem_ExistingItem1_ReturnItem)
{
    auto result = itemStore.findItem("shd001");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(itemSample1, *result);
}

TEST_F(ItemStoreWithTwoItems, findItem_ExistingItem2_ReturnItem)
{
    auto result = itemStore.findItem("shd002");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(itemSample2, *result);
}

TEST_F(ItemStoreWithTwoItems, isItemExists_NonExistingItem_ReturnFalse)
{
    ASSERT_FALSE(itemStore.isItemExists("non123"));
}

TEST_F(ItemStoreWithTwoItems, isItemExists_ExistingItem1_ReturnTrue)
{
    ASSERT_TRUE(itemStore.isItemExists("shd001"));
}

TEST_F(ItemStoreWithTwoItems, isItemExists_ExistingItem2_ReturnTrue)
{
    ASSERT_TRUE(itemStore.isItemExists("shd002"));
}

TEST_F(DefaultConstructedItemStore, addItem_NonExistingItem_ReturnTrue)
{
    ASSERT_TRUE(itemStore.addItem(Item({ "shd001", "Wooden Shield", "tex1", 1})));
}

TEST_F(ItemStoreWithTwoItems, addItem_ExistingItem_ReturnFalse)
{
    ASSERT_FALSE(itemStore.addItem(Item({ "shd001", "Wooden Shield", "tex1", 1})));
}

TEST_F(ItemStoreWithTwoItems, addItem_NonExistingItem_ReturnTrue)
{
    ASSERT_TRUE(itemStore.addItem(Item({ "shd003", "Diamond Shield", "tex2", 3})));
}

TEST_F(ItemStoreWithTwoItems, replaceItem_NonExistingItem_ReturnFalse)
{
    ASSERT_FALSE(itemStore.replaceItem("abc123", Item({ "fgh001", "Test", "tex1", 1})));
}

TEST_F(ItemStoreWithTwoItems, replaceItem_ExistingItem_ReturnTrue)
{
    Item expected({ "fgh001", "Test", "tex1", 1});
    ASSERT_TRUE(itemStore.replaceItem("shd001", expected));
    ASSERT_EQ(2, itemStore.getItemCount());
    ASSERT_EQ(expected, itemStore.findItem("fgh001").value());
    ASSERT_FALSE(itemStore.findItem("shd001").has_value());
}

TEST_F(ItemStoreWithTwoItems, replaceItem_ExistingItemSameId_ReturnTrue)
{
    Item expected({ "shd001", "Test1", "tex10", 10});
    ASSERT_TRUE(itemStore.replaceItem("shd001", expected));
    ASSERT_EQ(2, itemStore.getItemCount());
    ASSERT_EQ(expected, itemStore.findItem("shd001").value());
}

TEST_F(ItemStoreWithTwoItems, removeItem_ExistingItem_ReturnTrue)
{
    ASSERT_TRUE(itemStore.removeItem("shd001"));
}

TEST_F(ItemStoreWithTwoItems, removeItem_NonExistingItem_ReturnFalse)
{
    ASSERT_FALSE(itemStore.removeItem("abc123"));
}