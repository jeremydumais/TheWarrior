#include "monster.hpp"
#include <monsterZone.hpp>
#include <gtest/gtest.h>

using namespace thewarrior::models;

namespace thewarrior::models::monsterzone::unittest {

RGBItemColor getWhiteColor() { return RGBItemColor("White", "#FFFFFF"); }
RGBItemColor getGreenColor() { return RGBItemColor("Green", "#00FF00"); }

std::vector<std::string> getTwoMonsterSample() {
    return { "MON001", "MON002" };
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
    ASSERT_EQ(0, zone.getMonsterIds().size());
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

TEST_F(MonsterZoneSample, GetMonsters_Return0Monster)
{
    ASSERT_EQ(0, zone.getMonsterIds().size());
}

TEST_F(MonsterZoneWithTwoMonstersSample, GetMonsters_Return2Monsters)
{
    const auto &monsters = zone.getMonsterIds();
    ASSERT_EQ(2, monsters.size());
    ASSERT_EQ("MON001", monsters[0]);
    ASSERT_EQ("MON002", monsters[1]);
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

TEST_F(MonsterZoneSample, AddMonsterIdWithEmptyId_ThrowInvalidArgument)
{
	try {
        zone.addMonsterId("");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
	}
}

TEST_F(MonsterZoneSample, AddMonsterIdWithWhiteSpacesId_ThrowInvalidArgument)
{
	try {
        zone.addMonsterId("  ");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id cannot be empty.", err.what());
	}
}

TEST_F(MonsterZoneSample, AddMonsterIdWithOnlyFiveChars_ThrowInvalidArgument)
{
	try {
        zone.addMonsterId("MON00");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id must be 6 characters long.", err.what());
	}
}

TEST_F(MonsterZoneWithTwoMonstersSample, AddMonsterIdWithExistingMonsterId_ThrowInvalidArgument)
{
	try {
        zone.addMonsterId("MON001");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id MON001 already exists.", err.what());
	}
}

TEST_F(MonsterZoneWithTwoMonstersSample, AddMonsterIdWithExistingMonsterIdDifferentCaps_ThrowInvalidArgument)
{
	try {
        zone.addMonsterId("Mon001");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("id Mon001 already exists.", err.what());
	}
}

TEST_F(MonsterZoneSample, AddMonsterIdWithValidId_ReturnSuccess)
{
    ASSERT_NO_THROW(zone.addMonsterId("MON001"));
    const auto &monsterIds = zone.getMonsterIds();
    ASSERT_EQ(1, monsterIds.size());
    ASSERT_EQ("MON001", monsterIds[0]);
}

TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterIdWithEmptyOldId_ThrowInvalidArgument)
{
	try {
        zone.replaceMonsterId("", "MON003");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("oldId cannot be empty.", err.what());
	}
}

TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterIdWithWhiteSpacesOldId_ThrowInvalidArgument)
{
	try {
        zone.replaceMonsterId("  ", "MON003");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("oldId cannot be empty.", err.what());
	}
}

TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterIdWithOnlyFiveCharsOldId_ThrowInvalidArgument)
{
	try {
        zone.replaceMonsterId("MON00", "MON003");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("oldId must be 6 characters long.", err.what());
	}
}

TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterIdWithEmptyNewId_ThrowInvalidArgument)
{
	try {
        zone.replaceMonsterId("MON001", "");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("newId cannot be empty.", err.what());
	}
}

TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterIdWithWhiteSpacesNewId_ThrowInvalidArgument)
{
	try {
        zone.replaceMonsterId("MON001", "   ");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("newId cannot be empty.", err.what());
	}
}

TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterIdWithOnlyFiveCharsNewId_ThrowInvalidArgument)
{
	try {
        zone.replaceMonsterId("MON001", "MON00");
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("newId must be 6 characters long.", err.what());
	}
}

//TEST_F(MonsterZoneWithTwoMonstersSample, ReplaceMonsterIdInexistantId_ThrowInvalidArgument)
//{
	//try {
        //zone.replaceMonsterId("MON003", "MON004");
		//FAIL();
	//}
	//catch(const std::invalid_argument &err) {
        //ASSERT_STREQ("oldId MON003 doesn't exist in the list.", err.what());
	//}
//}

} // namespace thewarrior::models::monsterzone::unittest
