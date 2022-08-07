#include "manageArmorItemController.hpp"
#include <gtest/gtest.h>

using namespace itemeditor::controllers;
using namespace thewarrior::models;

class ManageArmorItemControllerEmptyStoreSample : public ::testing::Test
{
public:
    ManageArmorItemControllerEmptyStoreSample()
        : itemStore(std::make_shared<ItemStore>()),
          controller(itemStore)
    {
    }
    std::shared_ptr<ItemStore> itemStore;
    ManageArmorItemController controller;
};

TEST_F(ManageArmorItemControllerEmptyStoreSample, validateDefenseGain_WithEmptyString_ReturnFalse)
{
    ASSERT_FALSE(controller.validateDefenseGain(""));
    ASSERT_EQ("The defense gain value cannot be empty.", controller.getLastError());
}

TEST_F(ManageArmorItemControllerEmptyStoreSample, validateDefenseGain_WithWhitespacesString_ReturnFalse)
{
    ASSERT_FALSE(controller.validateDefenseGain("   "));
    ASSERT_EQ("The defense gain value cannot be empty.", controller.getLastError());
}

TEST_F(ManageArmorItemControllerEmptyStoreSample, validateDefenseGain_WithAAA_ReturnFalse)
{
    ASSERT_FALSE(controller.validateDefenseGain("AAA"));
    ASSERT_EQ("Unable to perform the float conversion of the defense gain.", controller.getLastError());
}

TEST_F(ManageArmorItemControllerEmptyStoreSample, validateDefenseGain_WithNumber3_ReturnTrue)
{
    ASSERT_TRUE(controller.validateDefenseGain("3"));
}

TEST_F(ManageArmorItemControllerEmptyStoreSample, validateDefenseGain_WithNumber3Comma14_ReturnTrue)
{
    ASSERT_TRUE(controller.validateDefenseGain("3,14"));
}

TEST_F(ManageArmorItemControllerEmptyStoreSample, validateDefenseGain_WithNumber3Dot14_ReturnTrue)
{
    ASSERT_TRUE(controller.validateDefenseGain("3.14"));
}

TEST_F(ManageArmorItemControllerEmptyStoreSample, validateDefenseGain_WithNumberOutOfRangeNumber_ReturnFalse)
{
    ASSERT_FALSE(controller.validateDefenseGain("72384723873248273927423897423893729827423984.3424782632"));
    ASSERT_EQ("The defense gain value is out of range.", controller.getLastError());

}
