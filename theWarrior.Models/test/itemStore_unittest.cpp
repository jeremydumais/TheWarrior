#include "itemStore.hpp"
#include <gtest/gtest.h>

using namespace std;

struct DefaultConstructedItemStore : public ::testing::Test
{
    ItemStore itemStore;
};

struct ItemStoreWithTwoItems : public ::testing::Test
{
    ItemStoreWithTwoItems()
    {
        itemStore.addItem(Item({ "shd001", "Wooden Shield", "tex1", 1}));
        itemStore.addItem(Item({ "shd002", "Iron Shield", "tex1", 2}));
    }
    ItemStore itemStore;
};

TEST_F(DefaultConstructedItemStore, ReturnSuccess)
{
    ItemStore store;
    ASSERT_EQ(0, itemStore.getTextureCount());
    ASSERT_EQ(0, itemStore.getItemCount());
}

TEST_F(DefaultConstructedItemStore, getTextureCount_ReturnZero)
{
    ASSERT_EQ(0, itemStore.getTextureCount());
}

TEST_F(DefaultConstructedItemStore, getItemCount_ReturnZero)
{
    ASSERT_EQ(0, itemStore.getItemCount());
}

TEST_F(ItemStoreWithTwoItems, findItem_NonExistingItem_ReturnEmpty)
{
    auto result = itemStore.findItem("non123");
    ASSERT_FALSE(result.has_value());
}

TEST_F(ItemStoreWithTwoItems, findItem_ExistingItem_ReturnItem)
{
    auto result = itemStore.findItem("shd001");
    ASSERT_TRUE(result.has_value());

    //Operator ==
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

/*TEST_F(ItemStoreWithTwoItems, replaceItem_ExistingItem_ReturnTrue)
{
    Item expected({ "fgh001", "Test", "tex1", 1});
    ASSERT_TRUE(itemStore.replaceItem("shd001", expected));
    //ASSERT_EQ(expected, itemStore.);
}*/