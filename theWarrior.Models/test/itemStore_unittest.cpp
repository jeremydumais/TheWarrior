#include "itemStore.hpp"
#include <gtest/gtest.h>

using namespace std;

struct DefaultConstructedItemStore : public ::testing::Test
{
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