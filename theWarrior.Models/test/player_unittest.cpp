#include <gtest/gtest.h>
#include <limits>
#include "player.hpp"

using namespace thewarrior::models;

class PlayerSample : public ::testing::Test {
 public:
    PlayerSample()
        : player("player1")
    {}
    Player player;
};

class FakePlayer : public Player {
 public:
    FakePlayer() : Player("FakeName") {
        m_level = 2;
        m_health = 22;
        m_maxHealth = 22;
        m_strength = 5.0F;
        m_agility = 5.0F;
        m_bonusHealthFromLevel = 5;
        m_gold = 24;
        m_experience = 6;
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

class PlayerLvl2WithEquipmentsSample : public ::testing::Test {
 public:
    PlayerLvl2WithEquipmentsSample() {}
    FakePlayer player;
};

TEST(Player_Constructor, WithEmptyName_ThrowInvalidArgument) {
    try {
        Player actual("");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
    }
}

TEST(Player_Constructor, WithWhitespacesName_ThrowInvalidArgument) {
    try {
        Player actual("   ");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
    }
}

TEST(Player_Constructor, WithJedName_ReturnSuccess) {
    Player actual("Jed");
    ASSERT_EQ("Jed", actual.getName());
}

TEST_F(PlayerSample, getName_ReturnPlayer1) {
    ASSERT_EQ("player1", player.getName());
}

TEST_F(PlayerSample, getLevel_Return1) {
    ASSERT_EQ(1, player.getLevel());
}

TEST_F(PlayerSample, getFacing_ReturnUp) {
    ASSERT_EQ(PlayerFacing::Up, player.getFacing());
}

TEST_F(PlayerSample, getStats_ReturnLvl1Stats) {
    auto stats = player.getStats();
    ASSERT_FLOAT_EQ(4.0F, stats.attack);
    ASSERT_FLOAT_EQ(4.0F, stats.defense);
    ASSERT_EQ(15, stats.health);
    ASSERT_EQ(15, stats.maxHealth);
}

TEST_F(PlayerSample, getGold_Return0) {
    ASSERT_EQ(0, player.getGold());
}

TEST_F(PlayerSample, getExperience_Return0) {
    ASSERT_EQ(0, player.getExperience());
}

TEST_F(PlayerLvl2WithEquipmentsSample, getExperience_Return6) {
    ASSERT_EQ(6, player.getExperience());
}

TEST_F(PlayerLvl2WithEquipmentsSample, getStats_ReturnLvl2Stats) {
    auto stats = player.getStats();
    ASSERT_FLOAT_EQ(6.6F, stats.attack);
    ASSERT_FLOAT_EQ(9.7F, stats.defense);
    ASSERT_EQ(22, stats.health);
    ASSERT_EQ(22, stats.maxHealth);
}

TEST_F(PlayerLvl2WithEquipmentsSample, isDeadWidth22HP_ReturnFalse) {
    ASSERT_FALSE(player.isDead());
}

TEST_F(PlayerLvl2WithEquipmentsSample, isDeadWidth0HP_ReturnTrue) {
    player.reduceHealth(22);
    ASSERT_TRUE(player.isDead());
}

TEST_F(PlayerLvl2WithEquipmentsSample, getGold_Return24) {
    ASSERT_EQ(24, player.getGold());
}

TEST_F(PlayerSample, SetName_WithJohn_ReturnSuccess) {
    auto expected = "John";
    player.setName(expected);
    ASSERT_EQ(expected, player.getName());
}

TEST_F(PlayerSample, SetName_WithEmptyName_ThrowInvalidArgument) {
    try {
        player.setName("");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
    }
}

TEST_F(PlayerSample, SetName_WithWhitespacesName_ThrowInvalidArgument) {
    try {
        player.setName("   ");
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
    }
}

TEST_F(PlayerSample, SetLevel_With10_ReturnSuccess) {
    unsigned int expected = 10;
    player.setLevel(expected);
    ASSERT_EQ(expected, player.getLevel());
}

TEST_F(PlayerSample, IncrementLevel_ReturnSuccess) {
    player.incrementLevel();
    ASSERT_EQ(2, player.getLevel());
}

TEST_F(PlayerSample, IncrementLevelOnMaxLevel_ReturnSuccessUnChanged) {
    player.setLevel(10);
    player.incrementLevel();
    ASSERT_EQ(10, player.getLevel());
}

TEST_F(PlayerSample, SetFacing_ReturnSuccess) {
    player.setFacing(PlayerFacing::Left);
    ASSERT_EQ(PlayerFacing::Left, player.getFacing());
}

TEST_F(PlayerSample, AddGoldWith15_ReturnSuccess) {
    player.addGold(15);
    ASSERT_EQ(15, player.getGold());
}

TEST_F(PlayerSample, AddGoldWithIntMax_ReturnSuccess) {
    int max = std::numeric_limits<int>::max();
    player.addGold(max);
    ASSERT_EQ(max, player.getGold());
}

TEST_F(PlayerSample, AddGoldWithIntMaxAndPlayerHad1Gold_ReturnSuccessAndMaxGold) {
    player.addGold(1);
    int max = std::numeric_limits<int>::max();
    player.addGold(max);
    ASSERT_EQ(max, player.getGold());
}

TEST_F(PlayerSample, AddGoldWithIntMaxMinus1AndPlayerHad1Gold_ReturnSuccess) {
    int max = std::numeric_limits<int>::max();
    player.addGold(max - 1);
    player.addGold(1);
    ASSERT_EQ(max, player.getGold());
}

TEST_F(PlayerSample, RemoveGoldWith0_ReturnSuccessAndStay0) {
    player.removeGold(0);
    ASSERT_EQ(0, player.getGold());
}

TEST_F(PlayerSample, RemoveGoldWith3_ReturnSuccessAndStay0) {
    player.removeGold(3);
    ASSERT_EQ(0, player.getGold());
}

TEST_F(PlayerLvl2WithEquipmentsSample, RemoveGoldWith3_ReturnSuccess) {
    player.removeGold(3);
    ASSERT_EQ(21, player.getGold());
}

TEST_F(PlayerLvl2WithEquipmentsSample, RemoveGoldWith24_ReturnSuccess) {
    player.removeGold(24);
    ASSERT_EQ(0, player.getGold());
}

TEST_F(PlayerLvl2WithEquipmentsSample, RemoveGoldWith25_ReturnSuccess) {
    player.removeGold(25);
    ASSERT_EQ(0, player.getGold());
}

TEST_F(PlayerSample, AddExperienceWith15_ReturnSuccess) {
    player.addExperience(5);
    ASSERT_EQ(5, player.getExperience());
    ASSERT_EQ(1, player.getLevel());
}

TEST_F(PlayerSample, AddExperienceWithIntMax_ReturnSuccess) {
    int max = std::numeric_limits<int>::max();
    player.addExperience(max);
    ASSERT_EQ(max, player.getExperience());
}

TEST_F(PlayerSample, AddExperienceWithIntMaxAndPlayerHad1Experience_ReturnSuccessAndMaxExperience) {
    player.addExperience(1);
    int max = std::numeric_limits<int>::max();
    player.addExperience(max);
    ASSERT_EQ(max, player.getExperience());
}

TEST_F(PlayerSample, AddExperienceWithIntMaxMinus1AndPlayerHad1Experience_ReturnSuccess) {
    int max = std::numeric_limits<int>::max();
    player.addExperience(max - 1);
    player.addExperience(1);
    ASSERT_EQ(max, player.getExperience());
}

TEST_F(PlayerSample, AddExperienceWith7_ReturnSuccessAndLevelUp) {
    player.addExperience(7);
    ASSERT_EQ(7, player.getExperience());
    ASSERT_EQ(2, player.getLevel());
}

TEST_F(PlayerSample, AddExperienceWith8_ReturnSuccessAndLevelUp) {
    player.addExperience(8);
    ASSERT_EQ(8, player.getExperience());
    ASSERT_EQ(2, player.getLevel());
}

