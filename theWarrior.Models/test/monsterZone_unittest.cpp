#include "monster.hpp"
#include <monsterZone.hpp>
#include <gtest/gtest.h>

using namespace thewarrior::models;

namespace thewarrior::models::monsterzone::unittest {

RGBItemColor getWhiteColor() { return RGBItemColor("White", "#FFFFFF"); }
RGBItemColor getGreenColor() { return RGBItemColor("Green", "#00FF00"); }
Monster getMonsterSample1() {
    return Monster({
            .id = "MON001",
            .name = "Monster1",
            .textureName = "Tex1",
            .textureIndex = 1,
            .health = 10,
            .attack = 1.0F,
            .defense = 1.0F,
            .goldMinimum = 1,
            .goldMaximum = 2
            });
}
Monster getMonsterSample2() {
    return Monster({
            .id = "MON002",
            .name = "Monster2",
            .textureName = "Tex2",
            .textureIndex = 2,
            .health = 11,
            .attack = 1.1F,
            .defense = 1.2F,
            .goldMinimum = 3,
            .goldMaximum = 4
            });
}

std::vector<Monster> getTwoMonsterSample() {
    return { getMonsterSample1(), getMonsterSample2() };
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
    ASSERT_EQ(0, zone.getMonsters().size());
}

TEST(MonsterZone_Constructor, WithRatioEncounterHigherThanRatioEncounterOn_ThrowInvalidArgument)
{
	try {
        auto zone = MonsterZone("Test", getWhiteColor(), 2, 1);
		FAIL();
	}
	catch(const std::invalid_argument &err) {
        ASSERT_STREQ("ratio encounter cannot be grater then ratio encounter on.", err.what());
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

//TEST_F(MonsterZoneSample, SetRatioEncounterWith2_ReturnSuccess)
//{
    //zone.setRatioEncounter(2);
    //ASSERT_EQ(2, zone.getRatioEncounter());
//}

} // namespace thewarrior::models::monsterzone::unittest
