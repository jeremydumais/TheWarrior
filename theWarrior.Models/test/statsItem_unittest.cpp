#include "statsItem.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace thewarrior::models;

class StatsItemCreationInfoSamples
{
public:
    static StatsItemCreationInfo getSample1() {
        return { "pot001", "Potion1", "tex1", 0, "test", Stats::Vitality, 1.0F, true, 30 };
    }
};

class StatsItemSample1 : public ::testing::Test
{
public:
    StatsItemSample1()
        : item(StatsItemCreationInfoSamples::getSample1())
    {
    }
    StatsItem item;
};

TEST(StatsItem_DefaultConstructor, CreateWithDefaultValues)
{
    StatsItem item;
    ASSERT_EQ("tmp999", item.getId());
    ASSERT_EQ("<temp item>", item.getName());
    ASSERT_EQ("tmp", item.getTextureName());
    ASSERT_EQ(0, item.getTextureIndex());
    ASSERT_EQ("", item.getOptionalDescription());
    ASSERT_EQ(Stats::Vitality, item.getStatChanging());
    ASSERT_FLOAT_EQ(1.1F, item.getGain());
    ASSERT_TRUE(item.getLimitOfOneApplied());
    ASSERT_EQ(0, item.getDurationInSecs());
}

TEST(StatsItem_Constructor, WithNullId_ThrowInvalidArgument)
{
    try {
        StatsItem statsItem({ "", "potion1", "tex1", 0, "", Stats::Vitality, 1.0F, true, 0 });
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    }
}

TEST(StatsItem_Constructor, WithValidValues_ReturnSuccess)
{
    StatsItem statsItem(StatsItemCreationInfoSamples::getSample1());
    ASSERT_EQ("pot001", statsItem.getId());
    ASSERT_EQ("Potion1", statsItem.getName());
    ASSERT_EQ("tex1", statsItem.getTextureName());
    ASSERT_EQ(0, statsItem.getTextureIndex());
    ASSERT_EQ("test", statsItem.getOptionalDescription());
    ASSERT_EQ(Stats::Vitality, statsItem.getStatChanging());
    ASSERT_FLOAT_EQ(1.0F, statsItem.getGain());
    ASSERT_TRUE(statsItem.getLimitOfOneApplied());
    ASSERT_EQ(30, statsItem.getDurationInSecs());
}

TEST(StatsItem_Equals, WithSameStatsItemInstance_ReturnTrue)
{
    auto statsItem1 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    ASSERT_TRUE(statsItem1.equals(statsItem1));
}

TEST(StatsItem_Equals, WithDifferentStatsItemInstanceSameValues_ReturnTrue)
{
    auto statsItem1 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    auto statsItem2 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    ASSERT_TRUE(statsItem1.equals(statsItem2));
}

TEST(StatsItem_Equals, WithDifferentStatsItemInstanceDifferentGain_ReturnFalse)
{
    auto statsItem1 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    auto statsItem2 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    statsItem2.setGain(1.1F);
    ASSERT_FALSE(statsItem1.equals(statsItem2));
}

TEST(StatsItem_Equals, WithDifferentStatsItemInstanceDifferentStats_ReturnFalse)
{
    auto statsItem1 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    auto statsItem2 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    statsItem2.setStatChanging(Stats::Strength);
    ASSERT_FALSE(statsItem1.equals(statsItem2));
}

TEST(StatsItem_OperatorEqual, WithDifferentStatsItemInstanceSameValues_ReturnTrue)
{
    auto statsItem1 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    auto statsItem2 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    ASSERT_TRUE(statsItem1 == statsItem2);
}

TEST(StatsItem_OperatorEqual, WithDifferentStatsItemInstanceDifferentGain_ReturnFalse)
{
    auto statsItem1 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    auto statsItem2 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    statsItem2.setGain(1.1F);
    ASSERT_FALSE(statsItem1 == statsItem2);
}

TEST(StatsItem_OperatorEqual, WithDifferentStatsItemInstanceDifferentStats_ReturnFalse)
{
    auto statsItem1 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    auto statsItem2 = StatsItem(StatsItemCreationInfoSamples::getSample1());
    statsItem2.setStatChanging(Stats::Strength);
    ASSERT_FALSE(statsItem1 == statsItem2);
}

TEST_F(StatsItemSample1, getStatChanging_ReturnVitality)
{
    ASSERT_EQ(Stats::Vitality, item.getStatChanging());
}

TEST_F(StatsItemSample1, getGain_Return1_0F)
{
    ASSERT_FLOAT_EQ(1.0F, item.getGain());
}

TEST_F(StatsItemSample1, getLimitOfOneApplied_ReturnTrue)
{
    ASSERT_TRUE(item.getLimitOfOneApplied());
}

TEST_F(StatsItemSample1, getDurationInSecs_Return0)
{
    ASSERT_EQ(30, item.getDurationInSecs());
}

TEST_F(StatsItemSample1, getType_ReturnItem)
{
    ASSERT_EQ(ItemType::StatsItem, item.getType());
}

TEST_F(StatsItemSample1, setStatsChanging_WithStrength_ReturnSuccess)
{
    item.setStatChanging(Stats::Strength);
    ASSERT_EQ(Stats::Strength, item.getStatChanging());
}

TEST_F(StatsItemSample1, setGain_With1_4F_ReturnSuccess)
{
    item.setGain(1.4F);
    ASSERT_FLOAT_EQ(1.4F, item.getGain());
}

TEST_F(StatsItemSample1, setLimitOfOneApplied_WithFalse_ReturnSuccess)
{
    item.setLimitOfOneApplied(false);
    ASSERT_FALSE(item.getLimitOfOneApplied());
}

TEST_F(StatsItemSample1, setDurationInSecs_With0_ReturnSuccess)
{
    item.setDurationInSecs(0);
    ASSERT_EQ(0, item.getDurationInSecs());
}
