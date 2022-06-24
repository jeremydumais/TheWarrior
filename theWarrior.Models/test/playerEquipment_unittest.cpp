#include "playerEquipment.hpp"
#include <gtest/gtest.h>

WeaponItem getWeaponSample1() { return WeaponItem({ "swd001", "Sword1", "tex1", 0, 
                                                    "", 5.0F, WeaponBodyPart::MainHand }); };
ArmorItem getArmorSample1() { return ArmorItem({ "shd001", "Shield1", "tex1", 0, 
                                                "", 3.0F, ArmorBodyPart::SecondaryHand }); };
ArmorItem getArmorSample2() { return ArmorItem({ "hel001", "Helmet1", "tex1", 0, 
                                                "", 1.2F, ArmorBodyPart::Head }); };
ArmorItem getArmorSample3() { return ArmorItem({ "chs001", "Chest1", "tex1", 0, 
                                                "", 1.3F, ArmorBodyPart::UpperBody }); };
ArmorItem getArmorSample4() { return ArmorItem({ "leg001", "Legs1", "tex1", 0, 
                                                "", 1.4F, ArmorBodyPart::LowerBody }); };
ArmorItem getArmorSample5() { return ArmorItem({ "glv001", "Gloves1", "tex1", 0, 
                                                "", 1.5F, ArmorBodyPart::Hands }); };
ArmorItem getArmorSample6() { return ArmorItem({ "boo001", "Boots1", "tex1", 0, 
                                                "", 1.6F, ArmorBodyPart::Feet }); };

class PlayerEquipmentSample1 : public ::testing::Test
{
public:
    PlayerEquipmentSample1()
        : playerEquipment(PlayerEquipment(getWeaponSample1(),
                                          VariantEquipment(getArmorSample1()),
                                          getArmorSample2(),
                                          getArmorSample3(),
                                          getArmorSample4(),
                                          getArmorSample5(),
                                          getArmorSample6()))
    {
    }
    PlayerEquipment playerEquipment;
};

TEST(PlayerEquipment_DefaultConstructor, ReturnEmptyPlayerEquipment)
{
    PlayerEquipment playerEquipment;
    ASSERT_EQ(boost::none, playerEquipment.getMainHand());
    ASSERT_EQ(boost::none, playerEquipment.getSecondaryHand());
    ASSERT_EQ(boost::none, playerEquipment.getHead());
    ASSERT_EQ(boost::none, playerEquipment.getUpperBody());
    ASSERT_EQ(boost::none, playerEquipment.getLowerBody());
    ASSERT_EQ(boost::none, playerEquipment.getHands());
    ASSERT_EQ(boost::none, playerEquipment.getFeet());
}

TEST_F(PlayerEquipmentSample1, Constructor_ReturnPlayerEquipmentSample)
{
    ASSERT_EQ(getWeaponSample1(), playerEquipment.getMainHand().get());
    ASSERT_EQ(getArmorSample1(), boost::get<ArmorItem>(playerEquipment.getSecondaryHand().get()));
    ASSERT_EQ(getArmorSample2(), playerEquipment.getHead().get());
    ASSERT_EQ(getArmorSample3(), playerEquipment.getUpperBody().get());
    ASSERT_EQ(getArmorSample4(), playerEquipment.getLowerBody().get());
    ASSERT_EQ(getArmorSample5(), playerEquipment.getHands().get());
    ASSERT_EQ(getArmorSample6(), playerEquipment.getFeet().get());
}

TEST(PlayerEquipment_Constructor, WithSecondaryWeaponInMainHand_ThrowInvalidArgument)
{
    auto weaponTest = getWeaponSample1();
    weaponTest.setSlotInBodyPart(WeaponBodyPart::SecondaryHand);
    try {
        PlayerEquipment(weaponTest,
                        VariantEquipment(getArmorSample1()),
                        getArmorSample2(),
                        getArmorSample3(),
                        getArmorSample4(),
                        getArmorSample5(),
                        getArmorSample6());
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Sword1 in main hand.", err.what());
    }
}

TEST(PlayerEquipment_Constructor, WithMainHandNone_ReturnSuccess)
{
    auto playerEquipment = PlayerEquipment(boost::none,
                                           VariantEquipment(getArmorSample1()),
                                           getArmorSample2(),
                                           getArmorSample3(),
                                           getArmorSample4(),
                                           getArmorSample5(),
                                           getArmorSample6());
    ASSERT_FALSE(playerEquipment.getMainHand().has_value());
}

TEST(PlayerEquipment_Constructor, WithMainWeaponInSecondaryHand_ThrowInvalidArgument)
{
    try {
        PlayerEquipment(getWeaponSample1(),
                        VariantEquipment(getWeaponSample1()),
                        getArmorSample2(),
                        getArmorSample3(),
                        getArmorSample4(),
                        getArmorSample5(),
                        getArmorSample6());
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Sword1 in secondary hand.", err.what());
    }
}

TEST(PlayerEquipment_Constructor, WithGlovesInSecondaryHand_ThrowInvalidArgument)
{
    try {
        PlayerEquipment(getWeaponSample1(),
                        VariantEquipment(getArmorSample5()),
                        getArmorSample2(),
                        getArmorSample3(),
                        getArmorSample4(),
                        getArmorSample5(),
                        getArmorSample6());
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Gloves1 in secondary hand.", err.what());
    }
}

TEST(PlayerEquipment_Constructor, WithSecondaryHandNone_ReturnSuccess)
{
    auto playerEquipment = PlayerEquipment(getWeaponSample1(),
                                           boost::none,
                                           getArmorSample2(),
                                           getArmorSample3(),
                                           getArmorSample4(),
                                           getArmorSample5(),
                                           getArmorSample6());
    ASSERT_FALSE(playerEquipment.getSecondaryHand().has_value());
}

TEST(PlayerEquipment_Constructor, WithGlovesOnHead_ThrowInvalidArgument)
{
    try {
        PlayerEquipment(getWeaponSample1(),
                        VariantEquipment(getArmorSample1()),
                        getArmorSample5(),
                        getArmorSample3(),
                        getArmorSample4(),
                        getArmorSample5(),
                        getArmorSample6());
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Gloves1 on head.", err.what());
    }
}

TEST(PlayerEquipment_Constructor, WithHeadNone_ReturnSuccess)
{
    auto playerEquipment = PlayerEquipment(getWeaponSample1(),
                                           VariantEquipment(getArmorSample1()),
                                           boost::none,
                                           getArmorSample3(),
                                           getArmorSample4(),
                                           getArmorSample5(),
                                           getArmorSample6());
    ASSERT_FALSE(playerEquipment.getHead().has_value());
}

TEST(PlayerEquipment_Constructor, WithGlovesOnUpperBody_ThrowInvalidArgument)
{
    try {
        PlayerEquipment(getWeaponSample1(),
                        VariantEquipment(getArmorSample1()),
                        getArmorSample2(),
                        getArmorSample5(),
                        getArmorSample4(),
                        getArmorSample5(),
                        getArmorSample6());
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Gloves1 on upper body.", err.what());
    }
}

TEST(PlayerEquipment_Constructor, WithUpperBodyNone_ReturnSuccess)
{
    auto playerEquipment = PlayerEquipment(getWeaponSample1(),
                                           VariantEquipment(getArmorSample1()),
                                           getArmorSample2(),
                                           boost::none,
                                           getArmorSample4(),
                                           getArmorSample5(),
                                           getArmorSample6());
    ASSERT_FALSE(playerEquipment.getUpperBody().has_value());
}

TEST(PlayerEquipment_Constructor, WithGlovesOnLowerBody_ThrowInvalidArgument)
{
    try {
        PlayerEquipment(getWeaponSample1(),
                        VariantEquipment(getArmorSample1()),
                        getArmorSample2(),
                        getArmorSample3(),
                        getArmorSample5(),
                        getArmorSample5(),
                        getArmorSample6());
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Gloves1 on lower body.", err.what());
    }
}

TEST(PlayerEquipment_Constructor, WithLowerBodyNone_ReturnSuccess)
{
    auto playerEquipment = PlayerEquipment(getWeaponSample1(),
                                           VariantEquipment(getArmorSample1()),
                                           getArmorSample2(),
                                           getArmorSample3(),
                                           boost::none,
                                           getArmorSample5(),
                                           getArmorSample6());
    ASSERT_FALSE(playerEquipment.getLowerBody().has_value());
}

TEST(PlayerEquipment_Constructor, WithHelmetInHands_ThrowInvalidArgument)
{
    try {
        PlayerEquipment(getWeaponSample1(),
                        VariantEquipment(getArmorSample1()),
                        getArmorSample2(),
                        getArmorSample3(),
                        getArmorSample4(),
                        getArmorSample2(),
                        getArmorSample6());
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Helmet1 in hands.", err.what());
    }
}

TEST(PlayerEquipment_Constructor, WithHandsNone_ReturnSuccess)
{
    auto playerEquipment = PlayerEquipment(getWeaponSample1(),
                                           VariantEquipment(getArmorSample1()),
                                           getArmorSample2(),
                                           getArmorSample3(),
                                           getArmorSample4(),
                                           boost::none,
                                           getArmorSample6());
    ASSERT_FALSE(playerEquipment.getHands().has_value());
}

TEST(PlayerEquipment_Constructor, WithGlovesInFeet_ThrowInvalidArgument)
{
    try {
        PlayerEquipment(getWeaponSample1(),
                        VariantEquipment(getArmorSample1()),
                        getArmorSample2(),
                        getArmorSample3(),
                        getArmorSample4(),
                        getArmorSample5(),
                        getArmorSample5());
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Gloves1 in feet.", err.what());
    }
}

TEST(PlayerEquipment_Constructor, WithFeetNone_ReturnSuccess)
{
    auto playerEquipment = PlayerEquipment(getWeaponSample1(),
                                           VariantEquipment(getArmorSample1()),
                                           getArmorSample2(),
                                           getArmorSample3(),
                                           getArmorSample4(),
                                           getArmorSample5(),
                                           boost::none);
    ASSERT_FALSE(playerEquipment.getFeet().has_value());
}

TEST_F(PlayerEquipmentSample1, getMainHand_ReturnWeapon1)
{
    ASSERT_EQ(getWeaponSample1(), playerEquipment.getMainHand().get());
}

TEST_F(PlayerEquipmentSample1, getSecondaryHand_ReturnArmor1)
{
    ASSERT_EQ(getArmorSample1(), boost::get<ArmorItem>(playerEquipment.getSecondaryHand().get()));
}

TEST_F(PlayerEquipmentSample1, getHead_ReturnArmor2)
{
    ASSERT_EQ(getArmorSample2(), playerEquipment.getHead().get());
}

TEST_F(PlayerEquipmentSample1, getUpperBody_ReturnArmor3)
{
    ASSERT_EQ(getArmorSample3(), playerEquipment.getUpperBody().get());
}

TEST_F(PlayerEquipmentSample1, getLowerBody_ReturnArmor4)
{
    ASSERT_EQ(getArmorSample4(), playerEquipment.getLowerBody().get());
}

TEST_F(PlayerEquipmentSample1, getHands_ReturnArmor5)
{
    ASSERT_EQ(getArmorSample5(), playerEquipment.getHands().get());
}

TEST_F(PlayerEquipmentSample1, getFeet_ReturnArmor6)
{
    ASSERT_EQ(getArmorSample6(), playerEquipment.getFeet().get());
}

TEST_F(PlayerEquipmentSample1, setMainHand_WithSecondaryWeapon_ThrowInvalidArgument)
{
    try {
        auto weapon = getWeaponSample1();
        weapon.setSlotInBodyPart(WeaponBodyPart::SecondaryHand);
        playerEquipment.setMainHand(weapon);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Sword1 in main hand.", err.what());
    }
}

TEST_F(PlayerEquipmentSample1, setMainHand_WithNone_ReturnSuccess)
{
    playerEquipment.setMainHand(boost::none);
    ASSERT_FALSE(playerEquipment.getMainHand().has_value());
}


TEST_F(PlayerEquipmentSample1, setMainHand_WithAnotherMainWeapon_ReturnSuccess)
{
    auto swd2 = WeaponItem({"swd009", "Sword2", "tex1", 0, "", 1.5F, WeaponBodyPart::MainHand });
    playerEquipment.setMainHand(swd2);
    ASSERT_TRUE(playerEquipment.getMainHand().has_value());
    ASSERT_EQ("Sword2", playerEquipment.getMainHand()->getName());
}

TEST_F(PlayerEquipmentSample1, setSecondaryHand_WithMainWeapon_ThrowInvalidArgument)
{
    try {
        playerEquipment.setSecondaryHand(VariantEquipment(getWeaponSample1()));
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Sword1 in secondary hand.", err.what());
    }
}

TEST_F(PlayerEquipmentSample1, setSecondaryHand_WithGloves_ThrowInvalidArgument)
{
    try {
        playerEquipment.setSecondaryHand(VariantEquipment(getArmorSample5()));
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("Cannot slot Gloves1 in secondary hand.", err.what());
    }
}

TEST_F(PlayerEquipmentSample1, setSecondaryHand_WithNone_ReturnSuccess)
{
    playerEquipment.setSecondaryHand(boost::none);
    ASSERT_FALSE(playerEquipment.getSecondaryHand().has_value());
}
