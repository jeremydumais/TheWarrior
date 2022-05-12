#include "addStatsItemController.hpp"
#include <gtest/gtest.h>

class AddStatsItemControllerEmptyStoreSample : public ::testing::Test
{
public:
    AddStatsItemControllerEmptyStoreSample()
        : itemStore(std::make_shared<ItemStore>()),
          controller(itemStore)
    {
    }
    std::shared_ptr<ItemStore> itemStore;
    AddStatsItemController controller;
};

TEST_F(AddStatsItemControllerEmptyStoreSample, validateGain_WithEmptyString_ReturnFalse)
{
    ASSERT_FALSE(controller.validateGain(""));
    ASSERT_EQ("The gain value cannot be empty.", controller.getLastError());
}

TEST_F(AddStatsItemControllerEmptyStoreSample, validateGain_WithWhitespacesString_ReturnFalse)
{
    ASSERT_FALSE(controller.validateGain("   "));
    ASSERT_EQ("The gain value cannot be empty.", controller.getLastError());
}

TEST_F(AddStatsItemControllerEmptyStoreSample, validateGain_WithAAA_ReturnFalse)
{
    ASSERT_FALSE(controller.validateGain("AAA"));
    ASSERT_EQ("Unable to perform the float conversion of the gain.", controller.getLastError());
}

TEST_F(AddStatsItemControllerEmptyStoreSample, validateGain_WithNumber3_ReturnTrue)
{
    ASSERT_TRUE(controller.validateGain("3"));
}

TEST_F(AddStatsItemControllerEmptyStoreSample, validateGain_WithNumber3Comma14_ReturnTrue)
{
    ASSERT_TRUE(controller.validateGain("3,14"));
}

TEST_F(AddStatsItemControllerEmptyStoreSample, validateGain_WithNumber3Dot14_ReturnTrue)
{
    ASSERT_TRUE(controller.validateGain("3.14"));
}

TEST_F(AddStatsItemControllerEmptyStoreSample, validateGain_WithNumberOutOfRangeNumber_ReturnFalse)
{
    ASSERT_FALSE(controller.validateGain("72384723873248273927423897423893729827423984.3424782632"));
    ASSERT_EQ("The gain value is out of range.", controller.getLastError());
}

TEST_F(AddStatsItemControllerEmptyStoreSample, validateDurationInSecs_WithEmptyString_ReturnFalse)
{
    ASSERT_FALSE(controller.validateDurationInSecs(""));
    ASSERT_EQ("The duration value cannot be empty.", controller.getLastError());
}

TEST_F(AddStatsItemControllerEmptyStoreSample, validateDurationInSecs_WithWhitespacesString_ReturnFalse)
{
    ASSERT_FALSE(controller.validateDurationInSecs("   "));
    ASSERT_EQ("The duration value cannot be empty.", controller.getLastError());
}

