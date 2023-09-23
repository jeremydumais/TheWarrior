#include "weaponItem.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace thewarrior::models;

class WeaponItemCreationInfoSamples
{
public:
    static WeaponItemCreationInfo getSample1() {
        return { "swd001", "weapon1", "tex1", 0, "test", 1.0F, WeaponBodyPart::SecondaryHand };
    }
};

class WeaponItemSample1 : public ::testing::Test
{
public:
    WeaponItemSample1()
        : item(WeaponItemCreationInfoSamples::getSample1())
    {
    }
    WeaponItem item;
};

TEST(WeaponItem_DefaultConstructor, CreateWithDefaultValues)
{
    WeaponItem item;
    ASSERT_EQ("tmp999", item.getId());
    ASSERT_EQ("<temp item>", item.getName());
    ASSERT_EQ("tmp", item.getTextureName());
    ASSERT_EQ(0, item.getTextureIndex());
    ASSERT_EQ("", item.getOptionalDescription());
    ASSERT_FLOAT_EQ(0.0F, item.getAttackGain());
    ASSERT_EQ(WeaponBodyPart::MainHand, item.getSlotInBodyPart());
}

TEST(WeaponItem_Constructor, WithNullId_ThrowInvalidArgument)
{
    try {
        WeaponItem weaponItem({ "", "weapon1", "tex1", 0, "", 1.0F, WeaponBodyPart::MainHand });
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    }
}

TEST(WeaponItem_Constructor, WithValidValues_ReturnSuccess)
{
    WeaponItem weaponItem(WeaponItemCreationInfoSamples::getSample1());
    ASSERT_EQ("swd001", weaponItem.getId());
    ASSERT_EQ("weapon1", weaponItem.getName());
    ASSERT_EQ("tex1", weaponItem.getTextureName());
    ASSERT_EQ(0, weaponItem.getTextureIndex());
    ASSERT_EQ("test", weaponItem.getOptionalDescription());
    ASSERT_FLOAT_EQ(1.0F, weaponItem.getAttackGain());
    ASSERT_EQ(WeaponBodyPart::SecondaryHand, weaponItem.getSlotInBodyPart());
}

TEST(WeaponItem_Equals, WithSameWeaponItemInstance_ReturnTrue)
{
    auto weaponItem1 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    ASSERT_TRUE(weaponItem1.equals(weaponItem1));
}

TEST(WeaponItem_Equals, WithDifferentWeaponItemInstanceSameValues_ReturnTrue)
{
    auto weaponItem1 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    auto weaponItem2 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    ASSERT_TRUE(weaponItem1.equals(weaponItem2));
}

TEST(WeaponItem_Equals, WithDifferentWeaponItemInstanceDifferentAttack_ReturnFalse)
{
    auto weaponItem1 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    auto weaponItem2 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    weaponItem2.setAttackGain(1.1F);
    ASSERT_FALSE(weaponItem1.equals(weaponItem2));
}

TEST(WeaponItem_Equals, WithDifferentWeaponItemInstanceDifferentBodyPart_ReturnFalse)
{
    auto weaponItem1 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    auto weaponItem2 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    weaponItem2.setSlotInBodyPart(WeaponBodyPart::MainHand);
    ASSERT_FALSE(weaponItem1.equals(weaponItem2));
}

TEST(WeaponItem_OperatorEqual, WithDifferentWeaponItemInstanceSameValues_ReturnTrue)
{
    auto weaponItem1 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    auto weaponItem2 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    ASSERT_TRUE(weaponItem1 == weaponItem2);
}

TEST(WeaponItem_OperatorEqual, WithDifferentWeaponItemInstanceDifferentAttack_ReturnFalse)
{
    auto weaponItem1 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    auto weaponItem2 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    weaponItem2.setAttackGain(1.1F);
    ASSERT_FALSE(weaponItem1 == weaponItem2);
}

TEST(WeaponItem_OperatorEqual, WithDifferentWeaponItemInstanceDifferentBodyPart_ReturnFalse)
{
    auto weaponItem1 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    auto weaponItem2 = WeaponItem(WeaponItemCreationInfoSamples::getSample1());
    weaponItem2.setSlotInBodyPart(WeaponBodyPart::MainHand);
    ASSERT_FALSE(weaponItem1 == weaponItem2);
}

TEST_F(WeaponItemSample1, getType_ReturnWeapon)
{
    ASSERT_EQ(ItemType::Weapon, item.getType());
}

TEST_F(WeaponItemSample1, getAttackGain_Return1_0F)
{
    ASSERT_FLOAT_EQ(1.0F, item.getAttackGain());
}

TEST_F(WeaponItemSample1, getSlotInBodyPart_ReturnSecondaryHand)
{
    ASSERT_EQ(WeaponBodyPart::SecondaryHand, item.getSlotInBodyPart());
}

TEST_F(WeaponItemSample1, setAttackGain_With1_4F_ReturnSuccess)
{
    item.setAttackGain(1.4F);
    ASSERT_FLOAT_EQ(1.4F, item.getAttackGain());
}

TEST_F(WeaponItemSample1, setSlotInBodyPart_WithMainHand_ReturnSuccess)
{
    item.setSlotInBodyPart(WeaponBodyPart::MainHand);
    ASSERT_EQ(WeaponBodyPart::MainHand, item.getSlotInBodyPart());
}
