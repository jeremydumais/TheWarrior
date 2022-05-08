#include "addWeaponItemController.hpp"
#include <gtest/gtest.h>

class AddWeaponItemControllerEmptyStoreSample : public ::testing::Test
{
public:
    AddWeaponItemControllerEmptyStoreSample()
        : itemStore(std::make_shared<ItemStore>()),
          controller(itemStore)
    {
    }
    std::shared_ptr<ItemStore> itemStore;
    AddWeaponItemController controller;
};

TEST_F(AddWeaponItemControllerEmptyStoreSample, validateAttackGain_WithEmptyString_ReturnFalse)
{
    ASSERT_FALSE(controller.validateAttackGain(""));
    ASSERT_EQ("The attack gain value cannot be empty.", controller.getLastError());
}

TEST_F(AddWeaponItemControllerEmptyStoreSample, validateAttackGain_WithWhitespacesString_ReturnFalse)
{
    ASSERT_FALSE(controller.validateAttackGain("   "));
    ASSERT_EQ("The attack gain value cannot be empty.", controller.getLastError());
}

TEST_F(AddWeaponItemControllerEmptyStoreSample, validateAttackGain_WithAAA_ReturnFalse)
{
    ASSERT_FALSE(controller.validateAttackGain("AAA"));
    ASSERT_EQ("Unable to perform the float conversion of the attack gain.", controller.getLastError());
}

TEST_F(AddWeaponItemControllerEmptyStoreSample, validateAttackGain_WithNumber3_ReturnTrue)
{
    ASSERT_TRUE(controller.validateAttackGain("3"));
}

TEST_F(AddWeaponItemControllerEmptyStoreSample, validateAttackGain_WithNumber3Comma14_ReturnTrue)
{
    ASSERT_TRUE(controller.validateAttackGain("3,14"));
}

TEST_F(AddWeaponItemControllerEmptyStoreSample, validateAttackGain_WithNumber3Dot14_ReturnTrue)
{
    ASSERT_TRUE(controller.validateAttackGain("3.14"));
}

TEST_F(AddWeaponItemControllerEmptyStoreSample, validateAttackGain_WithNumberOutOfRangeNumber_ReturnFalse)
{
    ASSERT_FALSE(controller.validateAttackGain("72384723873248273927423897423893729827423984.3424782632"));
    ASSERT_EQ("The attack gain value is out of range.", controller.getLastError());

}