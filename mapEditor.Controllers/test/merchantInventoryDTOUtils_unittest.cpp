#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "merchantInventory.hpp"
#include "merchantInventoryDTO.hpp"
#include "merchantInventoryDTOUtils.hpp"

using mapeditor::controllers::MerchantInventoryDTO;
using mapeditor::controllers::MerchantInventoryItemDTO;
using mapeditor::controllers::MerchantInventoryDTOUtils;
using thewarrior::models::ItemPriceOverride;
using thewarrior::models::MerchantInventory;
using thewarrior::models::MerchantInventoryType;


namespace mapeditor::controllers::merchantinventorydtoutils::unittest {

TEST(MerchantInventoryDTOUtils_fromMerchantInventory, WithEmptyInventory_ReturnsNameTypeAndNoItems) {
    const MerchantInventory inventory("Blacksmith", MerchantInventoryType::WeaponsAndArmors);

    const auto dto = MerchantInventoryDTOUtils::fromMerchantInventory(inventory);

    EXPECT_EQ("Blacksmith", dto.name);
    EXPECT_EQ(MerchantInventoryType::WeaponsAndArmors, dto.inventoryType);
    EXPECT_TRUE(dto.items.empty());
}

TEST(MerchantInventoryDTOUtils_fromMerchantInventory, WithItemsAndPriceOverrides_ReturnsCompleteDTOInInsertionOrder) {
    MerchantInventory inventory("Apothecary", MerchantInventoryType::StatsItems);
    ASSERT_TRUE(inventory.addItem("potion001"));
    ASSERT_TRUE(inventory.addItem("potion002"));
    ASSERT_TRUE(inventory.addItem("potion003"));
    ASSERT_TRUE(inventory.setPriceOverride(
        "potion001", ItemPriceOverride {.buyPrice = 100U, .sellPrice = 40U}));
    ASSERT_TRUE(inventory.setPriceOverride(
        "potion002", ItemPriceOverride {.buyPrice = std::nullopt, .sellPrice = 25U}));

    const auto dto = MerchantInventoryDTOUtils::fromMerchantInventory(inventory);

    ASSERT_EQ(3U, dto.items.size());
    EXPECT_EQ("potion001", dto.items[0].itemId);
    EXPECT_EQ(100U, dto.items[0].buyPriceOverride);
    EXPECT_EQ(40U, dto.items[0].sellPriceOverride);
    EXPECT_EQ("potion002", dto.items[1].itemId);
    EXPECT_EQ(std::nullopt, dto.items[1].buyPriceOverride);
    EXPECT_EQ(25U, dto.items[1].sellPriceOverride);
    EXPECT_EQ("potion003", dto.items[2].itemId);
    EXPECT_EQ(std::nullopt, dto.items[2].buyPriceOverride);
    EXPECT_EQ(std::nullopt, dto.items[2].sellPriceOverride);
}

TEST(MerchantInventoryDTOUtils_toMerchantInventory, WithValidDTO_ReturnsCompleteInventory) {
    const MerchantInventoryDTO dto {
        .name = "Apothecary",
        .inventoryType = MerchantInventoryType::StatsItems,
        .items = {
            MerchantInventoryItemDTO {
                .itemId = "potion001",
                .buyPriceOverride = 100U,
                .sellPriceOverride = 40U
            },
            MerchantInventoryItemDTO {
                .itemId = "potion002",
                .buyPriceOverride = std::nullopt,
                .sellPriceOverride = 25U
            },
            MerchantInventoryItemDTO {.itemId = "potion003"}
        }
    };

    const auto result = MerchantInventoryDTOUtils::toMerchantInventory(dto);

    ASSERT_TRUE(result.success());
    ASSERT_TRUE(result.merchantInventory.has_value());
    EXPECT_TRUE(result.errorMessage.empty());
    const auto &inventory = result.merchantInventory.value();
    EXPECT_EQ(dto.name, inventory.getName());
    EXPECT_EQ(dto.inventoryType, inventory.getInventoryType());
    ASSERT_EQ(3U, inventory.getItemIds().size());
    EXPECT_EQ("potion001", inventory.getItemIds()[0]);
    EXPECT_EQ("potion002", inventory.getItemIds()[1]);
    EXPECT_EQ("potion003", inventory.getItemIds()[2]);
    const auto firstPrice = inventory.getPriceOverridebyItemId("potion001");
    ASSERT_TRUE(firstPrice.has_value());
    EXPECT_EQ(100U, firstPrice->buyPrice);
    EXPECT_EQ(40U, firstPrice->sellPrice);
    const auto secondPrice = inventory.getPriceOverridebyItemId("potion002");
    ASSERT_TRUE(secondPrice.has_value());
    EXPECT_EQ(std::nullopt, secondPrice->buyPrice);
    EXPECT_EQ(25U, secondPrice->sellPrice);
    EXPECT_FALSE(inventory.getPriceOverridebyItemId("potion003").has_value());
}

TEST(MerchantInventoryDTOUtils_toMerchantInventory, WithInvalidName_ReturnsError) {
    const MerchantInventoryDTO dto {
        .name = "   ",
        .inventoryType = MerchantInventoryType::WeaponsAndArmors,
        .items = {}
    };

    const auto result = MerchantInventoryDTOUtils::toMerchantInventory(dto);

    EXPECT_FALSE(result.success());
    EXPECT_FALSE(result.merchantInventory.has_value());
    EXPECT_EQ("name cannot be empty.", result.errorMessage);
}

TEST(MerchantInventoryDTOUtils_toMerchantInventory, WithDuplicateItemId_ReturnsErrorInsteadOfPartialInventory) {
    const MerchantInventoryDTO dto {
        .name = "Blacksmith",
        .inventoryType = MerchantInventoryType::WeaponsAndArmors,
        .items = {
            MerchantInventoryItemDTO {.itemId = "sword001"},
            MerchantInventoryItemDTO {.itemId = "sword001"}
        }
    };

    const auto result = MerchantInventoryDTOUtils::toMerchantInventory(dto);

    EXPECT_FALSE(result.success());
    EXPECT_FALSE(result.merchantInventory.has_value());
    EXPECT_EQ("The item sword001 already exist in the merchant inventory.", result.errorMessage);
}

}  // namespace mapeditor::controllers::merchantinventorydtoutils::unittest
