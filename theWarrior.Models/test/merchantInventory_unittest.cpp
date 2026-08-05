#include "merchantInventory.hpp"
#include <gtest/gtest.h>

using namespace thewarrior::models;

struct MerchantInventoryWithTwoItems : public ::testing::Test {
    MerchantInventoryWithTwoItems() {
        inventory.addItem("item001");
        inventory.addItem("item002");
    }

    MerchantInventory inventory { "Merchant", MerchantInventoryType::WeaponsAndArmors };
};

TEST(MerchantInventory, ConstructorWithValidNameCreatesEmptyInventory) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);

    EXPECT_EQ("Merchant", inventory.getName());
    EXPECT_EQ(MerchantInventoryType::WeaponsAndArmors, inventory.getInventoryType());
    EXPECT_TRUE(inventory.getItemIds().empty());
    EXPECT_TRUE(inventory.getLastError().empty());
}

TEST(MerchantInventory, ConstructorWithEmptyNameThrowsInvalidArgument) {
    EXPECT_THROW(MerchantInventory("", MerchantInventoryType::WeaponsAndArmors), std::invalid_argument);
}

TEST(MerchantInventory, ConstructorWithWhitespaceNameThrowsInvalidArgument) {
    EXPECT_THROW(MerchantInventory("   ", MerchantInventoryType::WeaponsAndArmors), std::invalid_argument);
}

TEST(MerchantInventory, SetNameChangesName) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);

    EXPECT_TRUE(inventory.setName("Blacksmith"));
    EXPECT_EQ("Blacksmith", inventory.getName());
}

TEST(MerchantInventory, SetNameWithEmptyNameFailsWithError) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);

    EXPECT_FALSE(inventory.setName(""));
    EXPECT_EQ("name cannot be empty.", inventory.getLastError());
    EXPECT_EQ("Merchant", inventory.getName());
}

TEST(MerchantInventory, SetNameWithWhitespaceNameFailsWithError) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);

    EXPECT_FALSE(inventory.setName("   "));
    EXPECT_EQ("name cannot be empty.", inventory.getLastError());
    EXPECT_EQ("Merchant", inventory.getName());
}

TEST(MerchantInventory, AddItemStoresItsId) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);

    ASSERT_TRUE(inventory.addItem("item001"));
    ASSERT_EQ(1U, inventory.getItemIds().size());
    EXPECT_EQ("item001", inventory.getItemIds().front());
}

TEST_F(MerchantInventoryWithTwoItems, GetItemIdsPreservesInsertionOrder) {
    ASSERT_EQ(2U, inventory.getItemIds().size());
    EXPECT_EQ("item001", inventory.getItemIds()[0]);
    EXPECT_EQ("item002", inventory.getItemIds()[1]);
}

TEST_F(MerchantInventoryWithTwoItems, AddDuplicateItemFailsWithError) {
    EXPECT_FALSE(inventory.addItem("item001"));
    EXPECT_EQ("The item item001 already exist in the merchant inventory.",
              inventory.getLastError());
    EXPECT_EQ(2U, inventory.getItemIds().size());
}

TEST_F(MerchantInventoryWithTwoItems, RemoveExistingItemSucceeds) {
    ASSERT_TRUE(inventory.removeItem("item001"));
    ASSERT_EQ(1U, inventory.getItemIds().size());
    EXPECT_EQ("item002", inventory.getItemIds().front());
}

TEST_F(MerchantInventoryWithTwoItems, RemoveMissingItemFailsWithError) {
    EXPECT_FALSE(inventory.removeItem("missing"));
    EXPECT_EQ("The item missing doesn'y exist in the merchant inventory.",
              inventory.getLastError());
    EXPECT_EQ(2U, inventory.getItemIds().size());
}

TEST(MerchantInventory, MissingPriceOverrideReturnsNullopt) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);

    EXPECT_FALSE(inventory.getPriceOverridebyItemId("item001").has_value());
}

TEST(MerchantInventory, SetPriceOverrideStoresBuyAndSellPrices) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);

    ASSERT_TRUE(inventory.setPriceOverride("item001", ItemPriceOverride { 125U, 40U }));
    const auto result = inventory.getPriceOverridebyItemId("item001");
    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result->buyPrice.has_value());
    ASSERT_TRUE(result->sellPrice.has_value());
    EXPECT_EQ(125U, *result->buyPrice);
    EXPECT_EQ(40U, *result->sellPrice);
}

TEST(MerchantInventory, SetPriceOverrideAllowsOnlyOnePrice) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);

    ASSERT_TRUE(inventory.setPriceOverride(
        "item001", ItemPriceOverride { std::nullopt, 40U }));
    const auto result = inventory.getPriceOverridebyItemId("item001");
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->buyPrice.has_value());
    ASSERT_TRUE(result->sellPrice.has_value());
    EXPECT_EQ(40U, *result->sellPrice);
}

TEST(MerchantInventory, SetDuplicatePriceOverrideFailsAndKeepsOriginal) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);
    ASSERT_TRUE(inventory.setPriceOverride("item001", ItemPriceOverride { 125U, 40U }));

    EXPECT_FALSE(inventory.setPriceOverride("item001", ItemPriceOverride { 200U, 75U }));
    EXPECT_EQ("The price override for the item item001 already exist in the merchant inventory.",
              inventory.getLastError());
    const auto result = inventory.getPriceOverridebyItemId("item001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(125U, result->buyPrice);
    EXPECT_EQ(40U, result->sellPrice);
}

TEST(MerchantInventory, RemoveExistingPriceOverrideSucceeds) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);
    ASSERT_TRUE(inventory.setPriceOverride("item001", ItemPriceOverride { 125U, 40U }));

    EXPECT_TRUE(inventory.removePriceOverride("item001"));
    EXPECT_FALSE(inventory.getPriceOverridebyItemId("item001").has_value());
}

TEST(MerchantInventory, RemoveMissingPriceOverrideFailsWithError) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);

    EXPECT_FALSE(inventory.removePriceOverride("item001"));
    EXPECT_EQ("The price override for the item item001 doesn't exist in the merchant inventory.",
              inventory.getLastError());
}

TEST(MerchantInventory, ValidateEmptyInventoryFailsWithError) {
    MerchantInventory inventory("Merchant", MerchantInventoryType::WeaponsAndArmors);

    EXPECT_FALSE(inventory.validate());
    EXPECT_EQ("The merchant inventory must contain at least one item.",
              inventory.getLastError());
}

TEST_F(MerchantInventoryWithTwoItems, ValidateInventoryContainingItemsSucceeds) {
    EXPECT_TRUE(inventory.validate());
}

TEST_F(MerchantInventoryWithTwoItems, ValidateAfterRemovingAllItemsFailsWithError) {
    ASSERT_TRUE(inventory.removeItem("item001"));
    ASSERT_TRUE(inventory.removeItem("item002"));

    EXPECT_FALSE(inventory.validate());
    EXPECT_EQ("The merchant inventory must contain at least one item.",
              inventory.getLastError());
}
