#include "monster.hpp"
#include "monsterZone.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include <gtest/gtest.h>
#include <optional>

using namespace thewarrior::models;

namespace thewarrior::models::monsterzone::unittest {

RGBItemColor getWhiteColor() { return RGBItemColor("White", "#FFFFFF"); }
RGBItemColor getGreenColor() { return RGBItemColor("Green", "#00FF00"); }

std::vector<MonsterZoneMonsterEncounter> getTwoMonsterSample() {
    return {
        MonsterZoneMonsterEncounter("MON001", MonsterEncounterRatio::Normal),
        MonsterZoneMonsterEncounter("MON002", MonsterEncounterRatio::LessThanNormal)
    };
}

class MonsterZoneSample : public ::testing::Test
{
public:
    MonsterZoneSample()
        : zone("Test", getWhiteColor())
    {}
    MonsterZone zone;
};

class MonsterZoneWithTwoMonstersSample : public ::testing::Test
{
public:
    MonsterZoneWithTwoMonstersSample()
        : zone("Test", getWhiteColor(), 2, 5, getTwoMonsterSample())
    {}
    MonsterZone zone;
};


TEST(MonsterZone_Constructor, WithEmptyName_ThrowInvalidArgument)
{
	try {
        auto zone = MonsterZone("", getWhiteColor());
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(MonsterZone_Constructor, WithWhiteSpacesName_ThrowInvalidArgument)
{
	try {
        auto zone = MonsterZone("   ", getWhiteColor());
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST(MonsterZone_Constructor, WithTestName_ReturnSuccess)
{
    auto zone = MonsterZone("test", getWhiteColor());
    ASSERT_EQ("test", zone.getName());
    ASSERT_EQ(getWhiteColor(), zone.getColor());
    ASSERT_EQ(1, zone.getRatioEncounter());
    ASSERT_EQ(10, zone.getRatioEncounterOn());
    ASSERT_EQ(0, zone.getMonsterEncounters().size());
}

TEST(MonsterZone_Constructor, WithRatioEncounterHigherThanRatioEncounterOn_ThrowInvalidArgument)
{
	try {
        auto zone = MonsterZone("Test", getWhiteColor(), 2, 1);
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("ratio encounter cannot be greater than ratio encounter on.", err.what());
	}
}

TEST(MonsterZone_Constructor, WithRatioEncounterOfZero_ThrowInvalidArgument)
{
	try {
        auto zone = MonsterZone("Test", getWhiteColor(), 0, 1);
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("ratio encounter must be greater than zero.", err.what());
	}
}

TEST(MonsterZone_Constructor, WithRatioEncounterOnOfZero_ThrowInvalidArgument)
{
	try {
        auto zone = MonsterZone("Test", getWhiteColor(), 1, 0);
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("ratio encounter on must be greater than zero.", err.what());
	}
}

TEST(MonsterZone_Constructor, WithRatioEncounterEqualsRatioEncounterOn_ReturnSuccess)
{
    auto zone = MonsterZone("test", getWhiteColor(), 1, 1);
    ASSERT_EQ("test", zone.getName());
    ASSERT_EQ(getWhiteColor(), zone.getColor());
    ASSERT_EQ(1, zone.getRatioEncounter());
    ASSERT_EQ(1, zone.getRatioEncounterOn());
}

TEST_F(MonsterZoneSample, GetName_ReturnTest)
{
    ASSERT_EQ("Test", zone.getName());
}

TEST_F(MonsterZoneSample, GetColor_ReturnTest)
{
    ASSERT_EQ(getWhiteColor(), zone.getColor());
}

TEST_F(MonsterZoneSample, GetRatioEncounter_ReturnOne)
{
    ASSERT_EQ(1, zone.getRatioEncounter());
}

TEST_F(MonsterZoneSample, GetRatioEncounterOn_ReturnTen)
{
    ASSERT_EQ(10, zone.getRatioEncounterOn());
}

TEST_F(MonsterZoneSample, GetMonsterEncounter_WithEmptyMonsterId_ReturnNullOpt)
{
    ASSERT_FALSE(zone.getMonsterEncounter("").has_value());
}

TEST_F(MonsterZoneSample, GetMonsterEncounter_WithWhiteSpacesMonsterId_ReturnNullOpt)
{
    ASSERT_FALSE(zone.getMonsterEncounter("   ").has_value());
}

TEST_F(MonsterZoneSample, GetMonsterEncounter_WithNotExistingMonsterId_ReturnNullOpt)
{
    ASSERT_FALSE(zone.getMonsterEncounter("AAA111").has_value());
}

TEST_F(MonsterZoneWithTwoMonstersSample, GetMonsterEncounter_WithMON001MonsterId_ReturnFirstElement)
{
    const auto item = zone.getMonsterEncounter("MON001");
    ASSERT_TRUE(item.has_value());
    ASSERT_EQ("MON001", item->getMonsterId());
    ASSERT_EQ(MonsterEncounterRatio::Normal, item->getEncounterRatio());
}

TEST_F(MonsterZoneWithTwoMonstersSample, GetMonsterEncounter_WithMON002MonsterId_ReturnSecondElement)
{
    const auto item = zone.getMonsterEncounter("MON002");
    ASSERT_TRUE(item.has_value());
    ASSERT_EQ("MON002", item->getMonsterId());
    ASSERT_EQ(MonsterEncounterRatio::LessThanNormal, item->getEncounterRatio());
}

TEST_F(MonsterZoneSample, GetMonsters_Return0Monster)
{
    ASSERT_EQ(0, zone.getMonsterEncounters().size());
}

TEST_F(MonsterZoneWithTwoMonstersSample, GetMonsters_Return2Monsters)
{
    const auto &monsters = zone.getMonsterEncounters();
    ASSERT_EQ(2, monsters.size());
    ASSERT_EQ("MON001", monsters[0].getMonsterId());
    ASSERT_EQ(MonsterEncounterRatio::Normal, monsters[0].getEncounterRatio());
    ASSERT_EQ("MON002", monsters[1].getMonsterId());
    ASSERT_EQ(MonsterEncounterRatio::LessThanNormal, monsters[1].getEncounterRatio());
}

TEST_F(MonsterZoneSample, SetNameWithEmptyString_ThrowInvalidArgument)
{
	try {
        zone.setName("");
        FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(MonsterZoneSample, SetNameWithWhiteSpacesString_ThrowInvalidArgument)
{
	try {
        zone.setName("   ");
        FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("name cannot be null or empty.", err.what());
	}
}

TEST_F(MonsterZoneSample, SetNameWithTest2_ReturnSuccess)
{
    zone.setName("test2");
    ASSERT_EQ("test2", zone.getName());
}

TEST_F(MonsterZoneSample, SetColorWithGreen_ReturnSuccess)
{
    zone.setColor(getGreenColor());
    ASSERT_EQ(getGreenColor(), zone.getColor());
}

TEST_F(MonsterZoneSample, SetRatioEncounterWith2_ReturnSuccess)
{
    zone.setRatioEncounter(2);
    ASSERT_EQ(2, zone.getRatioEncounter());
}

TEST_F(MonsterZoneSample, SetRatioEncounterWith11_ReturnSuccess)
{
	try {
        zone.setRatioEncounter(11);
        FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("ratio encounter cannot be greater than ratio encounter on.", err.what());
	}
}

TEST_F(MonsterZoneWithTwoMonstersSample, SetRatioEncounterOnWith3_ReturnSuccess)
{
    zone.setRatioEncounterOn(3);
    ASSERT_EQ(3, zone.getRatioEncounterOn());
}

TEST_F(MonsterZoneWithTwoMonstersSample, SetRatioEncounterOnWith1_ReturnSuccess)
{
	try {
        zone.setRatioEncounterOn(1);
        FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("ratio encounter cannot be greater than ratio encounter on.", err.what());
	}
}

TEST_F(MonsterZoneSample, SetRatioEncounterWithZero_ThrowInvalidArgument)
{
	try {
        zone.setRatioEncounter(0);
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("ratio encounter must be greater than zero.", err.what());
	}
}

TEST_F(MonsterZoneSample, SetRatioEncounterOnWithZero_ThrowInvalidArgument)
{
	try {
        zone.setRatioEncounterOn(0);
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("ratio encounter on must be greater than zero.", err.what());
	}
}

TEST_F(MonsterZoneWithTwoMonstersSample, AddMonsterEncounterWithExistingMonsterId_ReturnFalse)
{
    ASSERT_FALSE(zone.addMonsterEncounter({"MON001", MonsterEncounterRatio::Normal}));
    ASSERT_EQ("id MON001 already exists.", zone.getLastError());
}

TEST_F(MonsterZoneWithTwoMonstersSample, AddMonsterEncounterWithExistingMonsterIdDifferentCaps_ReturnFalse)
{
    ASSERT_FALSE(zone.addMonsterEncounter({"Mon001", MonsterEncounterRatio::Normal}));
    ASSERT_EQ("id Mon001 already exists.", zone.getLastError());
}

TEST_F(MonsterZoneSample, AddMonsterEncounterWithNonExistantId_ReturnTrue)
{
    ASSERT_TRUE(zone.addMonsterEncounter({ "MON001", MonsterEncounterRatio::Rare}));
    const auto &monsterEncounters = zone.getMonsterEncounters();
    ASSERT_EQ(1, monsterEncounters.size());
    ASSERT_EQ("MON001", monsterEncounters[0].getMonsterId());
    ASSERT_EQ(MonsterEncounterRatio::Rare, monsterEncounters[0].getEncounterRatio());
}

TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterEncounterWithOldIdNotInList_ReturnFalse)
{
    ASSERT_FALSE(zone.replaceMonsterEncounter({ "MON003", MonsterEncounterRatio::Normal },
                                 { "MON004", MonsterEncounterRatio::Normal }));
    ASSERT_EQ("oldId MON003 doesn't exist in the list.", zone.getLastError());
}

TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterEncounterWithNewIdAlreadyInList_ReturnFalse)
{
    ASSERT_FALSE(zone.replaceMonsterEncounter({ "MON001", MonsterEncounterRatio::Normal },
                                 { "MON002", MonsterEncounterRatio::Normal }));
    ASSERT_EQ("newId MON002 is already in the list.", zone.getLastError());
}

TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterEncounterWithSameId_ReturnFalse)
{
    ASSERT_FALSE(zone.replaceMonsterEncounter({ "MON002", MonsterEncounterRatio::Normal },
                                 { "MON002", MonsterEncounterRatio::Normal }));
    ASSERT_EQ("newId MON002 is already in the list.", zone.getLastError());
}

TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterEncounterWithNewIdNotAlreadyInList_ReturnTrue)
{
    zone.replaceMonsterEncounter({ "MON001", MonsterEncounterRatio::Normal },
                                 { "MON003", MonsterEncounterRatio::Rare });
    const auto &monsterEncounters = zone.getMonsterEncounters();
    ASSERT_EQ(2, monsterEncounters.size());
    ASSERT_EQ("MON003", monsterEncounters[0].getMonsterId());
    ASSERT_EQ(MonsterEncounterRatio::Rare, monsterEncounters[0].getEncounterRatio());
    ASSERT_EQ("MON002", monsterEncounters[1].getMonsterId());
    ASSERT_EQ(MonsterEncounterRatio::LessThanNormal, monsterEncounters[1].getEncounterRatio());
}

TEST_F(MonsterZoneWithTwoMonstersSample, RemoveMonsterEncounter_WithEmptyMonsterId_ReturnFalse)
{
    ASSERT_FALSE(zone.removeMonsterEncounter(""));
    ASSERT_EQ("monsterId  doesn't exist in the list.", zone.getLastError());
}

TEST_F(MonsterZoneWithTwoMonstersSample, RemoveMonsterEncounter_WithWhiteSpacesMonsterId_ReturnFalse)
{
    ASSERT_FALSE(zone.removeMonsterEncounter("   "));
    ASSERT_EQ("monsterId     doesn't exist in the list.", zone.getLastError());
}

TEST_F(MonsterZoneWithTwoMonstersSample, RemoveMonsterEncounter_WithInexistantMonsterId_ReturnFalse)
{
    ASSERT_FALSE(zone.removeMonsterEncounter("ABC123"));
    ASSERT_EQ("monsterId ABC123 doesn't exist in the list.", zone.getLastError());
}

TEST_F(MonsterZoneWithTwoMonstersSample, RemoveMonsterEncounter_WithMON001MonsterId_ReturnTrue)
{
    ASSERT_TRUE(zone.removeMonsterEncounter("MON001"));
    const auto &monsterEncounters = zone.getMonsterEncounters();
    ASSERT_EQ(1, monsterEncounters.size());
    ASSERT_EQ("MON002", monsterEncounters[0].getMonsterId());
    ASSERT_EQ(MonsterEncounterRatio::LessThanNormal, monsterEncounters[0].getEncounterRatio());
}

TEST_F(MonsterZoneWithTwoMonstersSample, RemoveMonsterEncounter_WithMON002MonsterId_ReturnTrue)
{
    ASSERT_TRUE(zone.removeMonsterEncounter("MON002"));
    const auto &monsterEncounters = zone.getMonsterEncounters();
    ASSERT_EQ(1, monsterEncounters.size());
    ASSERT_EQ("MON001", monsterEncounters[0].getMonsterId());
    ASSERT_EQ(MonsterEncounterRatio::Normal, monsterEncounters[0].getEncounterRatio());
}

} // namespace thewarrior::models::monsterzone::unittest
