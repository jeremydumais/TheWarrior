#include "openChestActionPropertiesConverter.hpp"
#include <gtest/gtest.h>
#include <map>
#include <string>

using namespace thewarrior::models;

TEST(OpenChestActionPropertiesConverter_fromMap, WithAllProperties_ReturnPopulatedProperties) {
    const std::map<std::string, std::string> values {
        {openchestproperty::ContentType, "1"},
        {openchestproperty::ItemId, "potion001"},
        {openchestproperty::GoldAmount, "125"},
        {openchestproperty::OpenedTextureIndex, "12"}
    };

    const auto properties = OpenChestActionPropertiesConverter::fromMap(values);

    ASSERT_EQ(ChestContentType::Item, properties.getContentType());
    ASSERT_EQ("potion001", properties.getItemId());
    ASSERT_EQ(125U, properties.getGoldAmount());
    ASSERT_EQ(12, properties.getOpenedTextureIndex());
}

TEST(OpenChestActionPropertiesConverter_fromMap, WithEmptyMap_ReturnDefaultProperties) {
    const auto properties = OpenChestActionPropertiesConverter::fromMap({});

    ASSERT_EQ(ChestContentType::Nothing, properties.getContentType());
    ASSERT_TRUE(properties.getItemId().empty());
    ASSERT_EQ(0U, properties.getGoldAmount());
    ASSERT_EQ(-1, properties.getOpenedTextureIndex());
}

TEST(OpenChestActionPropertiesConverter_fromMap, WithInvalidNumbers_KeepNumericDefaults) {
    const std::map<std::string, std::string> values {
        {openchestproperty::ContentType, "invalid"},
        {openchestproperty::GoldAmount, "invalid"},
        {openchestproperty::OpenedTextureIndex, "invalid"}
    };

    const auto properties = OpenChestActionPropertiesConverter::fromMap(values);

    ASSERT_EQ(ChestContentType::Nothing, properties.getContentType());
    ASSERT_EQ(0U, properties.getGoldAmount());
    ASSERT_EQ(-1, properties.getOpenedTextureIndex());
}

TEST(OpenChestActionPropertiesConverter_fromMap, WithContentTypeAboveValidRange_ReturnNothing) {
    const std::map<std::string, std::string> values {
        {openchestproperty::ContentType, "3"}
    };

    const auto properties = OpenChestActionPropertiesConverter::fromMap(values);

    ASSERT_EQ(ChestContentType::Nothing, properties.getContentType());
}

TEST(OpenChestActionPropertiesConverter_fromMap, WithNegativeContentType_ReturnNothing) {
    const std::map<std::string, std::string> values {
        {openchestproperty::ContentType, "-1"}
    };

    const auto properties = OpenChestActionPropertiesConverter::fromMap(values);

    ASSERT_EQ(ChestContentType::Nothing, properties.getContentType());
}

TEST(OpenChestActionPropertiesConverter_fromMap, WithPartiallyNumericContentType_ReturnNothing) {
    const std::map<std::string, std::string> values {
        {openchestproperty::ContentType, "1invalid"}
    };

    const auto properties = OpenChestActionPropertiesConverter::fromMap(values);

    ASSERT_EQ(ChestContentType::Nothing, properties.getContentType());
}

TEST(OpenChestActionPropertiesConverter_toMap, WithProperties_ReturnAllSerializedValues) {
    OpenChestActionProperties properties;
    properties.setContentType(ChestContentType::Gold);
    properties.setItemId("elixir001");
    properties.setGoldAmount(250U);
    properties.setOpenedTextureIndex(18);

    const auto values = OpenChestActionPropertiesConverter::toMap(properties);

    ASSERT_EQ(4U, values.size());
    ASSERT_EQ("2", values.at(openchestproperty::ContentType));
    ASSERT_EQ("elixir001", values.at(openchestproperty::ItemId));
    ASSERT_EQ("250", values.at(openchestproperty::GoldAmount));
    ASSERT_EQ("18", values.at(openchestproperty::OpenedTextureIndex));
}

TEST(OpenChestActionPropertiesConverter_roundTrip, PreserveAllProperties) {
    OpenChestActionProperties expected;
    expected.setContentType(ChestContentType::Gold);
    expected.setItemId("goldBag001");
    expected.setGoldAmount(500U);
    expected.setOpenedTextureIndex(24);

    const auto actual = OpenChestActionPropertiesConverter::fromMap(
        OpenChestActionPropertiesConverter::toMap(expected));

    ASSERT_EQ(expected.getContentType(), actual.getContentType());
    ASSERT_EQ(expected.getItemId(), actual.getItemId());
    ASSERT_EQ(expected.getGoldAmount(), actual.getGoldAmount());
    ASSERT_EQ(expected.getOpenedTextureIndex(), actual.getOpenedTextureIndex());
}
