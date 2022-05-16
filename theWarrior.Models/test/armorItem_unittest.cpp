#include "armorItem.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

class ArmorItemCreationInfoSamples
{
public:
    static ArmorItemCreationInfo getSample1() {
        return { "swd001", "armor1", "tex1", 0, 1.0F, ArmorBodyPart::Head };
    }
};

class ArmorItemSample1 : public ::testing::Test
{
public:
    ArmorItemSample1()
        : item(ArmorItemCreationInfoSamples::getSample1())
    {
    }
    ArmorItem item;
};

TEST(ArmorItem_DefaultConstructor, CreateWithDefaultValues)
{
    ArmorItem item;
    ASSERT_EQ("tmp999", item.getId());
    ASSERT_EQ("<temp item>", item.getName());
    ASSERT_EQ("tmp", item.getTextureName());
    ASSERT_EQ(0, item.getTextureIndex());
    ASSERT_FLOAT_EQ(0.0F, item.getDefenseGain());
    ASSERT_EQ(ArmorBodyPart::Head, item.getSlotInBodyPart());
}

TEST(ArmorItem_Constructor, WithNullId_ThrowInvalidArgument)
{
    try {
        ArmorItem armorItem({ "", "armor1", "tex1", 0, 1.0F, ArmorBodyPart::Head });
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    }
}

TEST(ArmorItem_Constructor, WithValidValues_ReturnSuccess)
{
    ArmorItem armorItem(ArmorItemCreationInfoSamples::getSample1());
    ASSERT_EQ("swd001", armorItem.getId());
    ASSERT_EQ("armor1", armorItem.getName());
    ASSERT_EQ("tex1", armorItem.getTextureName());
    ASSERT_EQ(0, armorItem.getTextureIndex());
    ASSERT_FLOAT_EQ(1.0F, armorItem.getDefenseGain());
    ASSERT_EQ(ArmorBodyPart::Head, armorItem.getSlotInBodyPart());
}

TEST(ArmorItem_Equals, WithSameArmorItemInstance_ReturnTrue)
{
    auto armorItem1 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    ASSERT_TRUE(armorItem1.equals(armorItem1));
}

TEST(ArmorItem_Equals, WithDifferentArmorItemInstanceSameValues_ReturnTrue)
{
    auto armorItem1 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    auto armorItem2 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    ASSERT_TRUE(armorItem1.equals(armorItem2));
}

TEST(ArmorItem_Equals, WithDifferentArmorItemInstanceDifferentAttack_ReturnFalse)
{
    auto armorItem1 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    auto armorItem2 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    armorItem2.setDefenseGain(1.1F);
    ASSERT_FALSE(armorItem1.equals(armorItem2));
}

TEST(ArmorItem_Equals, WithDifferentArmorItemInstanceDifferentBodyPart_ReturnFalse)
{
    auto armorItem1 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    auto armorItem2 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    armorItem2.setSlotInBodyPart(ArmorBodyPart::UpperBody);
    ASSERT_FALSE(armorItem1.equals(armorItem2));
}

TEST(ArmorItem_OperatorEqual, WithDifferentArmorItemInstanceSameValues_ReturnTrue)
{
    auto armorItem1 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    auto armorItem2 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    ASSERT_TRUE(armorItem1 == armorItem2);
}

TEST(ArmorItem_OperatorEqual, WithDifferentArmorItemInstanceDifferentAttack_ReturnFalse)
{
    auto armorItem1 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    auto armorItem2 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    armorItem2.setDefenseGain(1.1F);
    ASSERT_FALSE(armorItem1 == armorItem2);
}

TEST(ArmorItem_OperatorEqual, WithDifferentArmorItemInstanceDifferentBodyPart_ReturnFalse)
{
    auto armorItem1 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    auto armorItem2 = ArmorItem(ArmorItemCreationInfoSamples::getSample1());
    armorItem2.setSlotInBodyPart(ArmorBodyPart::UpperBody);
    ASSERT_FALSE(armorItem1 == armorItem2);
}

TEST_F(ArmorItemSample1, getType_ReturnArmor) 
{
    ASSERT_EQ(ItemType::Armor, item.getType());
}

TEST_F(ArmorItemSample1, getAttackGain_Return1_0F)
{
    ASSERT_FLOAT_EQ(1.0F, item.getDefenseGain());
}

TEST_F(ArmorItemSample1, getSlotInBodyPart_ReturnHead)
{
    ASSERT_EQ(ArmorBodyPart::Head, item.getSlotInBodyPart());
}

TEST_F(ArmorItemSample1, setAttackGain_With1_4F_ReturnSuccess)
{
    item.setDefenseGain(1.4F);
    ASSERT_FLOAT_EQ(1.4F, item.getDefenseGain());
}

TEST_F(ArmorItemSample1, setSlotInBodyPart_WithUpperBody_ReturnSuccess)
{
    item.setSlotInBodyPart(ArmorBodyPart::UpperBody);
    ASSERT_EQ(ArmorBodyPart::UpperBody, item.getSlotInBodyPart());
}