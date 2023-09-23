#include "monsterZoneMonsterEncounter.hpp"
#include <gtest/gtest.h>

namespace thewarrior::models::monsterzonemonsterencounter::unittest {

class MonsterZoneMonsterEncounterSample : public ::testing::Test {
 public:
    MonsterZoneMonsterEncounterSample()
        : monsterEncounter("MON001", MonsterEncounterRatio::Rare)
    {}
    MonsterZoneMonsterEncounter monsterEncounter;
};

TEST(MonsterZoneMonsterEncounter, WithEmptyMonsterId_ThrowInvalidArgumentException) {
    try {
        auto item = MonsterZoneMonsterEncounter("", MonsterEncounterRatio::Normal);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    }
}

TEST(MonsterZoneMonsterEncounter, WithWhiteSpacesMonsterId_ThrowInvalidArgumentException) {
    try {
        auto item = MonsterZoneMonsterEncounter("   ", MonsterEncounterRatio::Normal);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    }
}

TEST(MonsterZoneMonsterEncounter, WithTestMonsterId_ThrowInvalidArgumentException) {
    try {
        auto item = MonsterZoneMonsterEncounter("Test", MonsterEncounterRatio::Normal);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id must be 6 characters long.", err.what());
    }
}

TEST(MonsterZoneMonsterEncounter, With6SpacesMonsterId_ThrowInvalidArgumentException) {
    try {
        auto item = MonsterZoneMonsterEncounter("      ", MonsterEncounterRatio::Normal);
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    }
}

TEST(MonsterZoneMonsterEncounter, WithValidValues_ReturnSuccess) {
    auto item = MonsterZoneMonsterEncounter("MON001", MonsterEncounterRatio::Normal);
}

TEST_F(MonsterZoneMonsterEncounterSample, operatorEqual_WithSame_ReturnTrue) {
    MonsterZoneMonsterEncounter monsterEncounter2("MON001", MonsterEncounterRatio::Rare);
    ASSERT_EQ(monsterEncounter, monsterEncounter2);
}

TEST_F(MonsterZoneMonsterEncounterSample, operatorEqual_WithDiffId_ReturnFalse) {
    MonsterZoneMonsterEncounter monsterEncounter2("MON002", MonsterEncounterRatio::Rare);
    ASSERT_FALSE(monsterEncounter == monsterEncounter2);
}

TEST_F(MonsterZoneMonsterEncounterSample, operatorEqual_WithDiffRatio_ReturnFalse) {
    MonsterZoneMonsterEncounter monsterEncounter2("MON001", MonsterEncounterRatio::Normal);
    ASSERT_FALSE(monsterEncounter == monsterEncounter2);
}

TEST_F(MonsterZoneMonsterEncounterSample, operatorNotEqual_WithSame_ReturnFalse) {
    MonsterZoneMonsterEncounter monsterEncounter2("MON001", MonsterEncounterRatio::Rare);
    ASSERT_FALSE(monsterEncounter != monsterEncounter2);
}

TEST_F(MonsterZoneMonsterEncounterSample, operatorNotEqual_WithDiffId_ReturnTrue) {
    MonsterZoneMonsterEncounter monsterEncounter2("MON002", MonsterEncounterRatio::Rare);
    ASSERT_NE(monsterEncounter, monsterEncounter2);
}

TEST_F(MonsterZoneMonsterEncounterSample, operatorNotEqual_WithDiffRatio_ReturnTrue) {
    MonsterZoneMonsterEncounter monsterEncounter2("MON001", MonsterEncounterRatio::Normal);
    ASSERT_NE(monsterEncounter, monsterEncounter2);
}

TEST_F(MonsterZoneMonsterEncounterSample, getMonsterId_ReturnMON001) {
    ASSERT_EQ("MON001", monsterEncounter.getMonsterId());
}

TEST_F(MonsterZoneMonsterEncounterSample, getEncounterRatio_ReturnRare) {
    ASSERT_EQ(MonsterEncounterRatio::Rare, monsterEncounter.getEncounterRatio());
}

TEST_F(MonsterZoneMonsterEncounterSample, setMonsterId_WithEmptyString_ThrowInvalidArgumentException) {
    try {
        monsterEncounter.setMonsterId("");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    }
}

TEST_F(MonsterZoneMonsterEncounterSample, setMonsterId_WithWhiteSpacesString_ThrowInvalidArgumentException) {
    try {
        monsterEncounter.setMonsterId("  ");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    }
}

TEST_F(MonsterZoneMonsterEncounterSample, setMonsterId_With6WhiteSpacesString_ThrowInvalidArgumentException) {
    try {
        monsterEncounter.setMonsterId("      ");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
    }
}

TEST_F(MonsterZoneMonsterEncounterSample, setMonsterId_WithTest_ThrowInvalidArgumentException) {
    try {
        monsterEncounter.setMonsterId("Test");
        FAIL();
    }
    catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id must be 6 characters long.", err.what());
    }
}

TEST_F(MonsterZoneMonsterEncounterSample, setMonsterId_WithABC123_ReturnSuccess) {
    auto newMonsterId = "ABC123";
    monsterEncounter.setMonsterId(newMonsterId);
    ASSERT_EQ(newMonsterId, monsterEncounter.getMonsterId());
}

TEST_F(MonsterZoneMonsterEncounterSample, setEncounterRatio_WithLessThanNormal_ReturnSuccess) {
    monsterEncounter.setEncounterRatio(MonsterEncounterRatio::LessThanNormal);
    ASSERT_EQ(MonsterEncounterRatio::LessThanNormal, monsterEncounter.getEncounterRatio());
}

}  // namespace thewarrior::models::monsterzonemonsterencounter::unittest
