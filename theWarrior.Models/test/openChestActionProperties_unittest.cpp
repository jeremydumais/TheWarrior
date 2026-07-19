#include <gtest/gtest.h>
#include "openChestActionProperties.hpp"

using thewarrior::models::ChestContentType;
using thewarrior::models::OpenChestActionProperties;

TEST(OpenChestActionProperties_getContentType, DefaultConstructed_ReturnNothing) {
    OpenChestActionProperties properties;

    ASSERT_EQ(ChestContentType::Nothing, properties.getContentType());
}

TEST(OpenChestActionProperties_getItemId, DefaultConstructed_ReturnEmpty) {
    OpenChestActionProperties properties;

    ASSERT_TRUE(properties.getItemId().empty());
}

TEST(OpenChestActionProperties_getGoldAmount, DefaultConstructed_ReturnZero) {
    OpenChestActionProperties properties;

    ASSERT_EQ(0U, properties.getGoldAmount());
}

TEST(OpenChestActionProperties_getOpenedTextureIndex, DefaultConstructed_ReturnMinusOne) {
    OpenChestActionProperties properties;

    ASSERT_EQ(-1, properties.getOpenedTextureIndex());
}

TEST(OpenChestActionProperties_setContentType, WithGold_ReturnGold) {
    OpenChestActionProperties properties;

    properties.setContentType(ChestContentType::Gold);

    ASSERT_EQ(ChestContentType::Gold, properties.getContentType());
}

TEST(OpenChestActionProperties_setItemId, WithItemId_ReturnItemId) {
    OpenChestActionProperties properties;

    properties.setItemId("potion001");

    ASSERT_EQ("potion001", properties.getItemId());
}

TEST(OpenChestActionProperties_setGoldAmount, With100_Return100) {
    OpenChestActionProperties properties;

    properties.setGoldAmount(100U);

    ASSERT_EQ(100U, properties.getGoldAmount());
}

TEST(OpenChestActionProperties_setOpenedTextureIndex, With12_Return12) {
    OpenChestActionProperties properties;

    properties.setOpenedTextureIndex(12);

    ASSERT_EQ(12, properties.getOpenedTextureIndex());
}
