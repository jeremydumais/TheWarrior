#include "gameData.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace thewarrior::models;

TEST(GameData, DefaultConstructedStoreIsEmptyAndErrorIsEmpty) {
    GameData data;
    ASSERT_NE(nullptr, data.getItemStoreForEdition());
    EXPECT_EQ(0, data.getItemStore().getItemCount());
    EXPECT_TRUE(data.getLastError().empty());
}

TEST(GameData, EditingAccessorSharesTheStoreWithReadAccessor) {
    GameData data;
    auto store = data.getItemStoreForEdition();
    EXPECT_EQ(store, data.getItemStoreForEdition());
    EXPECT_EQ(store.get(), &data.getItemStore());
    ASSERT_TRUE(store->addItem(std::make_shared<Item>(
        ItemCreationInfo{"ite001", "Item1", "Tex1", 0})));
    EXPECT_NE(nullptr, data.getItemStore().findItem("ite001"));
    ASSERT_TRUE(store->removeItem("ite001"));
    EXPECT_EQ(0, data.getItemStore().getItemCount());
}

TEST(GameData, SeparateInstancesHaveIndependentStores) {
    GameData first;
    GameData second;
    ASSERT_TRUE(first.getItemStoreForEdition()->addItem(std::make_shared<Item>(
        ItemCreationInfo{"ite001", "Item1", "Tex1", 0})));
    EXPECT_EQ(0, second.getItemStore().getItemCount());
}

TEST(GameData, SharedStoreRemainsUsableAfterGameDataIsDestroyed) {
    std::shared_ptr<ItemStore> store;
    {
        GameData data;
        store = data.getItemStoreForEdition();
    }
    ASSERT_TRUE(store->addItem(std::make_shared<Item>(
        ItemCreationInfo{"ite001", "Item1", "Tex1", 0})));
    EXPECT_EQ(1, store->getItemCount());
}

TEST(GameData, SetItemStoreReplacesContentsWithoutInvalidatingEditingAccess) {
    GameData data;
    auto editorStore = data.getItemStoreForEdition();
    ASSERT_TRUE(editorStore->addItem(std::make_shared<Item>(
        ItemCreationInfo{"old001", "Old item", "Tex1", 0})));
    ItemStore replacement;
    ASSERT_TRUE(replacement.addItem(std::make_shared<Item>(
        ItemCreationInfo{"new001", "New item", "Tex1", 0})));
    ASSERT_TRUE(replacement.getTextureContainerForEdition().addTexture(
        TextureInfo{"Tex1", "items.png", 32, 32, 16, 16}));

    data.setItemStore(replacement);

    EXPECT_EQ(editorStore, data.getItemStoreForEdition());
    EXPECT_EQ(nullptr, editorStore->findItem("old001"));
    EXPECT_NE(nullptr, editorStore->findItem("new001"));
    EXPECT_EQ(1, editorStore->getTextureContainer().getCount());
    ASSERT_TRUE(replacement.removeItem("new001"));
    EXPECT_NE(nullptr, editorStore->findItem("new001"));
}

TEST(GameData, SetEmptyItemStoreClearsContents) {
    GameData data;
    ASSERT_TRUE(data.getItemStoreForEdition()->addItem(std::make_shared<Item>(
        ItemCreationInfo{"ite001", "Item1", "Tex1", 0})));
    data.setItemStore(ItemStore{});
    EXPECT_EQ(0, data.getItemStore().getItemCount());
}
