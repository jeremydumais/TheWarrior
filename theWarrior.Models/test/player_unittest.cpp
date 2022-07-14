#include "player.hpp"
#include <gtest/gtest.h>

class PlayerSample : public ::testing::Test
{
public:
    PlayerSample()
        : player("player1")
    {}
    Player player;
};

class FakePlayer : public Player
{
public:
    FakePlayer() : Player("FakeName")
    {
        m_level = 2;
        m_bonusAttackFromLevel = 2.0F;
        m_bonusDefenseFromLevel = 2.1F;
        m_bonusHealthFromLevel = 5;
        getEquipment().setMainHand(WeaponItem({"swd001",
                                               "Sword1",
                                               "Tex1",
                                               1,
                                               "",
                                               1.6F,
                                               WeaponBodyPart::MainHand}));
        getEquipment().setSecondaryHand(VariantEquipment(FakePlayer::getFakeArmor(1.7F, ArmorBodyPart::SecondaryHand)));
        getEquipment().setHead(FakePlayer::getFakeArmor(0.8F, ArmorBodyPart::Head));
        getEquipment().setUpperBody(FakePlayer::getFakeArmor(0.7F, ArmorBodyPart::UpperBody));
        getEquipment().setLowerBody(FakePlayer::getFakeArmor(0.6F, ArmorBodyPart::LowerBody));
        getEquipment().setHands(FakePlayer::getFakeArmor(0.5F, ArmorBodyPart::Hands));
        getEquipment().setFeet(FakePlayer::getFakeArmor(0.4F, ArmorBodyPart::Feet));
    }

    static ArmorItem getFakeArmor(float defenseGain, ArmorBodyPart bodyPart) {
        return ArmorItem({"arm001",
                          "Armor1",
                          "Tex1",
                          1,
                          "",
                          defenseGain,
                          bodyPart});
    }
};

class PlayerLvl2WithEquipmentsSample : public ::testing::Test
{
public:
    PlayerLvl2WithEquipmentsSample() {};
    FakePlayer player;
};

TEST(Player_Constructor, WithEmptyName_ThrowInvalidArgument)
{
    try {
        Player actual("");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(Player_Constructor, WithWhitespacesName_ThrowInvalidArgument)
{
    try {
        Player actual("   ");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(Player_Constructor, WithJedName_ReturnSuccess)
{
    Player actual("Jed");
    ASSERT_EQ("Jed", actual.getName());
}

TEST_F(PlayerSample, getName_ReturnPlayer1) 
{
    ASSERT_EQ("player1", player.getName());
}

TEST_F(PlayerSample, getLevel_Return1) 
{
    ASSERT_EQ(1, player.getLevel());
}

TEST_F(PlayerSample, getStats_ReturnLvl1Stats)
{
    auto stats = player.getStats();
    ASSERT_FLOAT_EQ(0.5F, stats.attack);
    ASSERT_FLOAT_EQ(0.5F, stats.defense);
    ASSERT_EQ(10, stats.health);
    ASSERT_EQ(10, stats.maxHealth);
}

TEST_F(PlayerLvl2WithEquipmentsSample, getStats_ReturnLvl2Stats)
{
    auto stats = player.getStats();
    ASSERT_FLOAT_EQ(4.1F, stats.attack);
    ASSERT_FLOAT_EQ(7.3F, stats.defense);
    ASSERT_EQ(15, stats.health);
    ASSERT_EQ(15, stats.maxHealth);
}

TEST_F(PlayerSample, SetName_WithJohn_ReturnSuccess)
{
    auto expected = "John";
    player.setName(expected);
    ASSERT_EQ(expected, player.getName());
}

TEST_F(PlayerSample, SetName_WithEmptyName_ThrowInvalidArgument)
{
    try {
        player.setName("");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(PlayerSample, SetName_WithWhitespacesName_ThrowInvalidArgument)
{
    try {
        player.setName("   ");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(PlayerSample, SetLevel_With10_ReturnSuccess)
{
    auto expected = 10;
    player.setLevel(expected);
    ASSERT_EQ(expected, player.getLevel());
}

TEST_F(PlayerSample, IncrementLevel_ReturnSuccess)
{
    player.incrementLevel();
    ASSERT_EQ(2, player.getLevel());
}
