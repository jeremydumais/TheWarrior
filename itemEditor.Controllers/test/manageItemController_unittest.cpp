#include "manageItemController.hpp"
#include <gtest/gtest.h>
#include <memory>

class ItemDTOSamples
{
public:
    static ItemDTO getSample1() { 
        ItemDTO retVal;
        retVal.id = "ite001";
        retVal.name = "item1";
        retVal.textureName = "tex1";
        retVal.textureIndex = 0;
        return retVal;
    };
    static ItemDTO getSample2() { 
        ItemDTO retVal;
        retVal.id = "ite002";
        retVal.name = "item2";
        retVal.textureName = "tex2";
        retVal.textureIndex = 1;
        return retVal;
    };
};

class ItemStoreWith2Items : public ::testing::Test
{
public:
    ItemStoreWith2Items() 
        : m_itemStore(std::make_shared<ItemStore>()),
          m_controller(m_itemStore)
    {
        m_controller.addItem(std::make_unique<ItemDTO>(ItemDTOSamples::getSample1()));
        m_controller.addItem(std::make_unique<ItemDTO>(ItemDTOSamples::getSample2()));
    }
    std::shared_ptr<ItemStore> m_itemStore;
    ManageItemController m_controller;

};

TEST_F(ItemStoreWith2Items, getItem_WithEmptyId_ReturnNullPtr)
{
    ASSERT_EQ(nullptr, m_controller.getItem(""));
}

TEST_F(ItemStoreWith2Items, getItem_WithWhiteSpacesId_ReturnNullPtr)
{
    ASSERT_EQ(nullptr, m_controller.getItem("   "));
}

TEST_F(ItemStoreWith2Items, getItem_WithInexistantId_ReturnNullPtr)
{
    ASSERT_EQ(nullptr, m_controller.getItem("eee444"));
}

TEST_F(ItemStoreWith2Items, getItem_WithIte001_ReturnItemDTO)
{
    auto actual = m_controller.getItem("ite001");
    ASSERT_NE(nullptr, actual);
    ASSERT_EQ("ite001", actual->id);
    ASSERT_EQ("item1", actual->name);
    ASSERT_EQ("tex1", actual->textureName);
    ASSERT_EQ(0, actual->textureIndex);
}

TEST(ManageItemController_addItem, WithEmptyStore_ReturnTrue)
{
    auto store = std::make_shared<ItemStore>();
    ManageItemController controller(store);
    ASSERT_TRUE(controller.addItem(std::make_unique<ItemDTO>(ItemDTOSamples::getSample1())));
    ASSERT_EQ(1, store->getItemCount());
}

TEST(ManageItemController_addItem, WithInvalidItemCreationInfo_ReturnFalse)
{
    auto store = std::make_shared<ItemStore>();
    ManageItemController controller(store);
    auto item = std::make_unique<ItemDTO>();
    item->id = "a";
    item->name = "b";
    item->textureName = "c";
    item->textureIndex = 0;
    ASSERT_FALSE(controller.addItem(std::move(item)));
    ASSERT_EQ("id must be 6 characters long.", controller.getLastError());
}

TEST_F(ItemStoreWith2Items, addItem_WithExistingId_ReturnFalse)
{
    ASSERT_FALSE(m_controller.addItem(std::make_unique<ItemDTO>(ItemDTOSamples::getSample1())));
    ASSERT_EQ("Item ite001 already exist in the store.", m_controller.getLastError());
}

TEST_F(ItemStoreWith2Items, updateItem_WithNoItemDTO_ReturnFalse)
{
    ASSERT_FALSE(m_controller.updateItem(nullptr, "ite001"));
    ASSERT_EQ("No itemInfo structure has been provided.", m_controller.getLastError());
}

TEST_F(ItemStoreWith2Items, updateItem_WithExistingId_ReturnFalse)
{
    auto sampleWithSameId = ItemDTOSamples::getSample1();
    auto originalId = sampleWithSameId.id;
    sampleWithSameId.id = ItemDTOSamples::getSample2().id;
    ASSERT_FALSE(m_controller.updateItem(std::make_unique<ItemDTO>(sampleWithSameId), originalId));
    ASSERT_EQ("Item ite002 already exist in the store.", m_controller.getLastError());
}

TEST_F(ItemStoreWith2Items, updateItem_WithNonExistingId_ReturnTrue)
{
    auto sampleWithSameId = ItemDTOSamples::getSample1();
    auto originalId = sampleWithSameId.id;
    auto newId = "tes111";
    sampleWithSameId.id = newId;
    ASSERT_TRUE(m_controller.updateItem(std::make_unique<ItemDTO>(sampleWithSameId), originalId));
    auto actual = m_controller.getItem(newId);
    ASSERT_EQ(newId, actual->id);
    ASSERT_EQ("item1", actual->name);
    ASSERT_EQ("tex1", actual->textureName);
    ASSERT_EQ(0, actual->textureIndex);
}