#include "addItemController.hpp"
#include <gtest/gtest.h>
#include <memory>

class ItemCreationInfoSamples
{
public:
    static ItemCreationInfo getSample1() { return { "ite001", "item1", "tex1", 0 }; };
    static ItemCreationInfo getSample2() { return { "ite002", "item2", "tex2", 1 }; };
};

class ItemStoreWith2Items : public ::testing::Test
{
public:
    ItemStoreWith2Items() 
        : m_itemStore(std::make_shared<ItemStore>()),
          m_controller(m_itemStore)
    {
        m_controller.addItem(ItemCreationInfoSamples::getSample1());
        m_controller.addItem(ItemCreationInfoSamples::getSample2());
    }
    std::shared_ptr<ItemStore> m_itemStore;
    AddItemController m_controller;

};

TEST(AddItemController_addItem, WithEmptyStore_ReturnTrue)
{
    auto store = std::make_shared<ItemStore>();
    AddItemController controller(store);
    ASSERT_TRUE(controller.addItem(ItemCreationInfoSamples::getSample1()));
    ASSERT_EQ(1, store->getItemCount());
}

TEST(AddItemController_addItem, WithInvalidItemCreationInfo_ReturnFalse)
{
    auto store = std::make_shared<ItemStore>();
    AddItemController controller(store);
    ASSERT_FALSE(controller.addItem({ "a", "b", "c", 0}));
    ASSERT_EQ("id must be 6 characters long.", controller.getLastError());
}

TEST_F(ItemStoreWith2Items, addItem_WithExistingId_ReturnFalse)
{
    ASSERT_FALSE(m_controller.addItem(ItemCreationInfoSamples::getSample1()));
    ASSERT_EQ("Item ite001 already exist in the store.", m_controller.getLastError());
}